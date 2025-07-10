import pygame
import socket
import threading
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from collections import deque
from ponycube import *

# Wi-Fi 配置
ESP32_IP = "0.0.0.0"  # 监听所有 IP
ESP32_PORT = 12345    # ESP32 的端口号

# 初始化 pygame
pygame.init()
screen = Screen(480, 400, scale=1.5)
pygame.display.set_caption("MPU6050 3D Visualization")
cube = Cube(30, 60, 10)  # 创建一个 30x60x10 的立方体

# 初始化加速度和角速度波形图窗口
fig_accel_gyro, (ax_accel, ax_gyro) = plt.subplots(2, 1, figsize=(10, 6))

# 数据存储
timestamps = deque(maxlen=500)  # 时间戳队列
accel_history = deque(maxlen=500)  # 加速度历史数据
gyro_history = deque(maxlen=500)  # 陀螺仪历史数据

gyro_data = [0, 0, 0]  # 陀螺仪数据
accel_data = [0, 0, 0]  # 加速度数据
latest_quaternion = Quaternion(1, 0, 0, 0)  # 初始四元数

# 接收数据函数
# @brief 接收来自 ESP32 的 UDP 数据包，并写入文件。
def receive_data():
    global latest_quaternion, gyro_data, accel_data, timestamps, accel_history, gyro_history
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
                    
                    if len(decoded_data) == 11:
                        # 解析数据
                        timestamp = int(decoded_data[0])  # 时间戳
                        accel_data = list(map(float, decoded_data[1:4]))  # 加速度数据
                        gyro_data = list(map(float, decoded_data[4:]))   # 陀螺仪数据
                        q_w, q_x, q_y, q_z = map(float, decoded_data[7:11])

                        # 更新历史数据
                        timestamps.append(timestamp)
                        accel_history.append(accel_data)
                        gyro_history.append(gyro_data)

                        latest_quaternion = Quaternion(q_w, q_x, q_y, q_z)


                        # 写入文件
                        print(f"接收到数据: {line}")
                        file.write(line + "\n")
                        file.flush()
            except Exception as e:
                print(f"数据解析错误: {e}")

# 启动接收数据线程
thread = threading.Thread(target=receive_data)
thread.daemon = True # 设置为守护线程
thread.start()

# 绘制 3D 图形
# @brief 显示陀螺仪的方向向量，未完成
# @param ax: 3D 图形的 Axes 对象
# @param gyro_data: 陀螺仪数据，格式为 [x, y, z]
def draw_3d(quaternion):
    cube.erase(screen) 
    cube.draw(screen, quaternion.normalized())  
    pygame.display.flip() 
    pygame.time.delay(10)



# 绘制 3 轴加速度和 3 轴角速度随时间变化的波形图
# @brief 绘制加速度和陀螺仪数据的波形图
# @param ax_accel: 加速度波形图的 Axes 对象
# @param ax_gyro: 陀螺仪波形图的 Axes 对象
# @param timestamps: 时间戳列表
# @param accel_history: 加速度历史数据，格式为 [[x, y, z], ...]
# @param gyro_history: 陀螺仪历史数据，格式为 [[x, y, z], ...]
def draw_accel_gyro(ax_accel, ax_gyro, timestamps, accel_history, gyro_history):
    ax_accel.clear()
    ax_gyro.clear()

    # 确保历史数据不为空
    if len(accel_history) > 0 and len(gyro_history) > 0:
        accel_array = np.array(accel_history)
        gyro_array = np.array(gyro_history)

        # 绘制加速度曲线
        ax_accel.plot(timestamps, accel_array[:, 0], label="Accel X", color="r")
        ax_accel.plot(timestamps, accel_array[:, 1], label="Accel Y", color="g")
        ax_accel.plot(timestamps, accel_array[:, 2], label="Accel Z", color="b")
        ax_accel.set_title("Acceleration Data")
        ax_accel.set_xlabel("Time")
        ax_accel.set_ylabel("Acceleration (g)")
        ax_accel.legend()
        ax_accel.set_ylim([-1, 1])  # 加速度的量程为 -1 到 1

        # 绘制角速度曲线
        ax_gyro.plot(timestamps, gyro_array[:, 0], label="Gyro X", color="c")
        ax_gyro.plot(timestamps, gyro_array[:, 1], label="Gyro Y", color="m")
        ax_gyro.plot(timestamps, gyro_array[:, 2], label="Gyro Z", color="y")
        ax_gyro.set_title("Gyroscope Data")
        ax_gyro.set_xlabel("Time")
        ax_gyro.set_ylabel("Angular Velocity (°/s)")
        ax_gyro.legend()
        ax_gyro.set_ylim([-125, 125])  # 角速度的量程为 -125 到 125
    else:
        print("加速度或角速度历史数据为空，无法绘制波形图")  # 可添加标志变量，避免频繁打印

    plt.pause(0.01)  # 更新图形

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
    draw_3d(latest_quaternion)

    # 更新加速度和角速度波形图窗口
    draw_accel_gyro(ax_accel, ax_gyro, timestamps, accel_history, gyro_history)

pygame.quit()