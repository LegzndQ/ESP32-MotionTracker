import pygame
import socket
import threading
import numpy as np
from collections import deque

# Wi-Fi 配置
ESP32_IP = "0.0.0.0"  # 监听所有 IP
ESP32_PORT = 12345    # ESP32 的端口号

# 初始化 pygame
pygame.init()
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("MPU6050 3D Visualization")

# 数据存储
gyro_data = [0, 0, 0]  # 陀螺仪数据
roll, pitch, yaw = 0, 0, 0  # 姿态角（单位：度）
timestamps = deque(maxlen=500)  # 时间戳队列

# 时间步长（假设数据接收频率为 100Hz）
dt = 0.01  # 每次更新的时间间隔（秒）

# 接收数据函数
# @brief 接收来自 ESP32 的 UDP 数据包，并写入文件。
def receive_data():
    global gyro_data, accel_data, timestamps, accel_history, gyro_history
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((ESP32_IP, ESP32_PORT))
    
    buffer = ""  # 用于存储未处理的接收数据

    with open("mpu6050_data.txt", "w") as file:
        while True:
            data, addr = sock.recvfrom(1024)
            try:
                # 将接收到的数据解码并添加到缓冲区
                buffer += data.decode("utf-8")

                # 按换行符分割数据
                lines = buffer.split("\n")
                buffer = lines[-1]  # 保留最后一个未完成的数据

                # 处理完整的行数据
                for line in lines[:-1]:
                    decoded_data = line.strip().split(",")
                    
                    # 验证数据格式
                    if len(decoded_data) != 7:  # 时间戳 + 3 轴加速度 + 3 轴角速度
                        print(f"数据格式错误: {decoded_data}")
                        continue
                    
                    # 解析数据
                    timestamp = int(decoded_data[0])  # 时间戳
                    accel_data = list(map(float, decoded_data[1:4]))  # 加速度数据
                    gyro_data = list(map(float, decoded_data[4:]))   # 陀螺仪数据

                    # 更新历史数据
                    timestamps.append(timestamp)
                    accel_history.append(accel_data)
                    gyro_history.append(gyro_data)

                    # 写入文件
                    file.write(line + "\n")
                    file.flush()
            except Exception as e:
                print(f"数据解析错误: {e}")

# 启动接收数据线程
thread = threading.Thread(target=receive_data)
thread.daemon = True  # 设置为守护线程
thread.start()

# 绘制 3D 图形
def draw_3d(screen, roll, pitch, yaw):
    screen.fill((0, 0, 0))  # 清空屏幕

    # 将姿态角转换为旋转矩阵
    roll_rad = np.radians(roll)
    pitch_rad = np.radians(pitch)
    yaw_rad = np.radians(yaw)

    # 绘制一个简单的立方体表示姿态
    center = (400, 300)  # 屏幕中心
    size = 100  # 立方体大小

    # 计算旋转后的顶点位置
    points = [
        (-size, -size, -size),
        (size, -size, -size),
        (size, size, -size),
        (-size, size, -size),
        (-size, -size, size),
        (size, -size, size),
        (size, size, size),
        (-size, size, size),
    ]

    # 旋转矩阵
    rotation_matrix = np.array([
        [np.cos(yaw_rad) * np.cos(pitch_rad), np.cos(yaw_rad) * np.sin(pitch_rad) * np.sin(roll_rad) - np.sin(yaw_rad) * np.cos(roll_rad), np.cos(yaw_rad) * np.sin(pitch_rad) * np.cos(roll_rad) + np.sin(yaw_rad) * np.sin(roll_rad)],
        [np.sin(yaw_rad) * np.cos(pitch_rad), np.sin(yaw_rad) * np.sin(pitch_rad) * np.sin(roll_rad) + np.cos(yaw_rad) * np.cos(roll_rad), np.sin(yaw_rad) * np.sin(pitch_rad) * np.cos(roll_rad) - np.cos(yaw_rad) * np.sin(roll_rad)],
        [-np.sin(pitch_rad), np.cos(pitch_rad) * np.sin(roll_rad), np.cos(pitch_rad) * np.cos(roll_rad)],
    ])

    # 旋转顶点
    rotated_points = []
    for point in points:
        rotated_point = np.dot(rotation_matrix, point)
        rotated_points.append(rotated_point)

    # 投影到屏幕
    projected_points = [(center[0] + p[0], center[1] - p[1]) for p in rotated_points]

    # 绘制立方体的边
    edges = [
        (0, 1), (1, 2), (2, 3), (3, 0),
        (4, 5), (5, 6), (6, 7), (7, 4),
        (0, 4), (1, 5), (2, 6), (3, 7),
    ]
    for edge in edges:
        pygame.draw.line(screen, (255, 255, 255), projected_points[edge[0]], projected_points[edge[1]], 2)

    pygame.display.flip()  # 更新屏幕

# 主循环
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # 更新姿态角
    roll += gyro_data[0] * dt
    pitch += gyro_data[1] * dt
    yaw += gyro_data[2] * dt

    # 绘制 3D 图形
    draw_3d(screen, roll, pitch, yaw)

pygame.quit()