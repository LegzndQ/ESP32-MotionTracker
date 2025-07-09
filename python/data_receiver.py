import socket

udp_ip = "0.0.0.0"  # 监听所有 IP
udp_port = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((udp_ip, udp_port))
n = 0

with open("mpu6050.txt", "w") as file:
    while True:
        data, addr = sock.recvfrom(1024)
        decoded_data = data.decode()
        print("收到数据:", decoded_data)
        file.write(decoded_data + "\n")