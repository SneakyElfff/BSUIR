import os
import select
import socket
import uuid
from datetime import datetime


def welcome(client_address):
    welcome_message = """
    Welcome!
    The list of commands you can use:
    - ECHO <message>: returns "message";
    - TIME: returns the current server time;
    - UPLOAD <filename>: uploads a file to the server;
    - DOWNLOAD <filename>: downloads a file from the server;
    - QUIT: closes the connection.\n"""

    server_socket.sendto(welcome_message.encode('utf-8'), client_address)


def echo(message):
    # split a message into parts by spaces
    command_parts = message.decode('utf-8').split(' ', 1)
    return command_parts[1].encode('utf-8')


def tellTime():
    current_time = datetime.now().strftime("%H:%M:%S")
    return current_time.encode('utf-8')


def upload(client_address, message):
    parts = message.decode('utf-8').split(' ', 1)
    filename = parts[1]
    file_size = int(server_socket.recvfrom(4096)[0].decode('utf-8'))
    received_size = 0

    if os.path.exists(filename + '.temp'):
        print(f"The client has continued to upload {filename}.")
        os.rename(filename + '.temp', filename)
        received_size = os.path.getsize(filename)
        server_socket.sendto(str(received_size).encode('utf-8'), client_address)
    else:
        server_socket.sendto(str(received_size).encode('utf-8'), client_address)

    with open(filename, "ab" if received_size > 0 else "wb") as file:
        while received_size < file_size:
            ready_to_read, _, _ = select.select([server_socket], [], [], 1)
            if not ready_to_read:
                print("\nUploading was interrupted.")
                os.rename(filename, filename + '.temp')
                return "File wasn't uploaded.".encode('utf-8')
            else:
                file_data, _ = server_socket.recvfrom(4096)
                file.write(file_data)
                received_size += len(file_data)

    print(f"File '{filename}' received")
    print(f"File '{filename}' saved")

    return "File uploaded successfully".encode('utf-8')


def download(client_address, message):
    parts = message.decode('utf-8').split(' ', 1)
    filename = parts[1]
    file_size = 0

    if os.path.exists(filename):
        with open(filename, "rb") as file:
            file_size = os.path.getsize(filename)
            server_socket.sendto(str(file_size).encode('utf-8'), client_address)

            data, _ = server_socket.recvfrom(4096)
            sent_size = int(data.decode('utf-8'))
            if sent_size != 0:
                print("The download continues")
                file.seek(sent_size)

            while True:
                file_data = file.read(4096)
                if not file_data:
                    break
                server_socket.sendto(file_data, client_address)

        print(f"File '{filename}' sent")

        client_response, _ = server_socket.recvfrom(4096)
        if client_response == b'File received':
            return "File downloaded successfully".encode('utf-8')
        else:
            return "File wasn't downloaded.".encode('utf-8')
    else:
        return str(file_size).encode('utf-8')


def remove_temp_files():
    for filename in os.listdir():
        if filename.endswith('.temp'):
            os.remove(filename)


# IPv4 addresses, UDP
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

host = input("Enter server IP address: ")
port = int(input("Enter server port: "))
server_address = (host, port)

session_ids = {}
expected_seq_nums = {}

server_socket.bind(server_address)

print(f"Server is listening on {host}:{port}")

while True:
    try:
        print("\nWaiting for connection...")
        server_socket.settimeout(None)

        data, client_address = server_socket.recvfrom(4096)
        print(f"\nConnection accepted from {client_address}")

        if client_address[0] not in session_ids:
            remove_temp_files()
            print("Temporary files have been removed for a new client")

        # OTHER POSSIBLE VARIANTS: 1) structure, storing client addresses, 2) adding client address to the filename
        session_id = str(uuid.uuid4())
        session_ids[client_address[0]] = session_id

        welcome(client_address)

        while True:
            # set a timeout for the server socket
            server_socket.settimeout(30)

            try:
                data, _ = server_socket.recvfrom(4096)
                sequence_number, _, command = data.decode('utf-8').partition(":")
            except socket.timeout:
                print("\nTimeout: no response from client.")  # occurs when there are no commands coming from the client
                break

            if client_address not in expected_seq_nums:
                expected_seq_nums[client_address] = 1
            else:
                if int(sequence_number) != expected_seq_nums[client_address]:
                    server_socket.sendto(b'Invalid sequence number.\n', client_address)
                    continue

            expected_seq_nums[client_address] += 1

            print(f"Data received: {data.decode('utf-8')}")
            print(f"Command received: {command}")

            if command.startswith("ECHO"):
                response = echo(data)
                server_socket.sendto(response, client_address)

            elif command.startswith("TIME"):
                response = tellTime()
                server_socket.sendto(response, client_address)

            elif command.startswith("UPLOAD"):
                response = upload(client_address, data)
                if response == "File wasn't uploaded.".encode('utf-8'):
                    break
                server_socket.sendto(response, client_address)

            elif command.startswith("DOWNLOAD"):
                response = download(client_address, data)
                if response == "File wasn't downloaded.".encode('utf-8'):
                    break
                server_socket.sendto(response, client_address)

            elif command.startswith("QUIT"):
                print("Closing the connection")
                break

            else:
                server_socket.sendto(b'Invalid command.\n', client_address)

    # occurs when a connection loss happens
    except socket.timeout:
        print("\nConnection is lost.")
        continue

    # in case an exception occurs, the server shuts down
    except Exception as e:
        print(f"\nException occurred: {e}.")

        print("Shutting down the server")
        remove_temp_files()
        server_socket.close()
        break
