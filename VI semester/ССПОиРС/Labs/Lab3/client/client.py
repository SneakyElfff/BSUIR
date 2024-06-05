import os
import socket


def main():
    # IPv4 addresses, UDP
    global parts
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    host = input("\nEnter server IP address: ")
    port = int(input("Enter server port: "))
    server_address = (host, port)

    print(f"Connected to the server {host}:{port}")

    sequence_number = 1

    try:
        client_socket.sendto(b'', server_address)

        welcome_message, _ = client_socket.recvfrom(4096)
        print(welcome_message.decode('utf-8'))

        while True:
            # set a timeout for the server socket
            client_socket.settimeout(30)

            command = input("Enter a command: ").strip()

            if command.startswith('ECHO') or command.startswith('UPLOAD') or command.startswith('DOWNLOAD'):
                parts = command.split(' ', 1)
                if len(parts) != 2:
                    print(f"Invalid command format. Usage: {parts[0]} <message>")
                    continue

            message = f"{sequence_number}:{command}"

            # Отправляем сообщение на сервер
            client_socket.sendto(message.encode('utf-8'), server_address)

            if command.upper() == 'QUIT':
                break

            elif command.startswith('UPLOAD'):
                filename = parts[1]
                if os.path.exists(filename):
                    with open(filename, "rb") as file:
                        file_size = os.path.getsize(filename)
                        client_socket.sendto(str(file_size).encode('utf-8'), server_address)

                        sent_size, _ = client_socket.recvfrom(4096)
                        sent_size = int(sent_size.decode('utf-8'))
                        if sent_size != 0:
                            print("The upload continues")
                        file.seek(sent_size)

                        while True:
                            data = file.read(4096)
                            if not data:
                                break
                            client_socket.sendto(data, server_address)

                        response, _ = client_socket.recvfrom(4096)
                        print(response.decode('utf-8'))
                else:
                    print("File not found.")

            elif command.startswith('DOWNLOAD'):
                filename = parts[1]
                file_size, _ = client_socket.recvfrom(4096)
                file_size = int(file_size.decode('utf-8'))
                if file_size == 0:
                    print("File not found on the server.")
                    continue

                received_size = 0

                if os.path.exists(filename):
                    print(f"The server has continued to send {filename}.")
                    received_size = os.path.getsize(filename)
                    client_socket.sendto(str(received_size).encode('utf-8'), server_address)
                else:
                    client_socket.sendto(str(received_size).encode('utf-8'), server_address)

                with open(filename, "ab" if received_size > 0 else "wb") as file:
                    while received_size < file_size:
                        file_data, _ = client_socket.recvfrom(4096)
                        file.write(file_data)
                        received_size += len(file_data)

                client_socket.sendto("File received".encode('utf-8'), server_address)

                response, _ = client_socket.recvfrom(4096)
                print(response.decode('utf-8'))

            else:
                response, _ = client_socket.recvfrom(4096)
                print(response.decode('utf-8'))

            sequence_number += 1

    # occurs when there is no response from the server after a time-out OR when downloading a file takes too long
    except socket.timeout:
        print("\nConnection is lost.")
        main()

    finally:
        print("\nClosing the connection")
        client_socket.close()


if __name__ == "__main__":
    main()
