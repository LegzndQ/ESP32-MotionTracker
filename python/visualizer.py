import pygame
import socket
import threading
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

# Wi-Fi 配置
# ESP32_IP = "192.168.1.100"  # ESP32 的 IP 地址
ESP32_IP = "0.0.0.0"  # 监听所有 IP
ESP32_PORT = 12345    # ESP32 的端口号

# 初始化 pygame
pygame.init()
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("MPU6050 3D Visualization")

# 初始化 3D 图形窗口
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 数据存储
gyro_data = [0, 0, 0]  # 陀螺仪数据
accel_data = [0, 0, 0]  # 加速度数据

# 接收数据线程
def receive_data():
    global gyro_data, accel_data
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((ESP32_IP, ESP32_PORT))
    while True:
        data, addr = sock.recvfrom(1024)
        try:
            decoded_data = data.decode("utf-8").strip().split(",")
            timestamp = int(decoded_data[0])  # 时间戳
            accel_data = list(map(float, decoded_data[1:4]))  # 加速度数据
            gyro_data = list(map(float, decoded_data[4:]))   # 陀螺仪数据
        except Exception as e:
            print(f"数据解析错误: {e}")

# 启动接收数据线程
thread = threading.Thread(target=receive_data)
thread.daemon = True
thread.start()

# 绘制 3D 图形
def draw_3d(ax, gyro_data):
    ax.clear()
    ax.quiver(0, 0, 0, gyro_data[0], gyro_data[1], gyro_data[2], length=1, color='r')
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    plt.pause(0.01)

# 主循环
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a:  # 按下 "a" 键
                print(f"加速度数据: {accel_data}")
            elif event.key == pygame.K_g:  # 按下 "g" 键
                print(f"陀螺仪数据: {gyro_data}")

    # 更新 3D 图形窗口
    draw_3d(ax, gyro_data)

    # 更新 pygame 窗口
    screen.fill((0, 0, 0))
    pygame.display.flip()

pygame.quit()