# 基于 MPU6050 的 motion_driver_6.12 中的 python 上位机改写
# 通过 udp 协议接收数据，并可显示传感器姿态

import socket
import pygame
import numpy as np
import threading
import matplotlib.pyplot as plt
from collections import deque
from ponycube import *
from euclid import Quaternion

# 数据存储
gyro_data = [0, 0, 0]  # 陀螺仪数据
accel_data = [0, 0, 0]  # 加速度数据
timestamps = deque(maxlen=500)  # 时间戳队列
accel_history = deque(maxlen=500)  # 加速度历史数据
gyro_history = deque(maxlen=500)  # 陀螺仪历史数据

class PacketReader: 
    def __init__(self, ip, port, quat_delegate=None, data_delegate=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((ip, port))

        # self.file_lock = threading.Lock()  # 文件写入锁
        self.file = open("mpu6050_data.txt", "w")  # 打开文件用于写入数据
        
        # 分发给对应处理器
        self.quat_delegate = quat_delegate if quat_delegate else EmptyPacketDelegate()
        self.data_delegate = data_delegate if data_delegate else EmptyPacketDelegate()

    def write_to_file(self, data):
        self.file.write(data + "\n")
        self.file.flush()  # 确保数据实时写入文件

    def read(self):
        global gyro_data, accel_data, timestamps, accel_history, gyro_history
        with open("mpu6050_data.txt", "w") as file:
            while True:
                try:
                    data, addr = self.sock.recvfrom(1024)
                    decoded_data = data.decode("utf-8").strip().split(",")
                    if len(decoded_data) == 11:
                        # 解析四元数
                        q_w, q_x, q_y, q_z = map(float, decoded_data[7:11]) # 左闭右开
                        packet = Quaternion(q_w, q_x, q_y, q_z)
                        self.quat_delegate.dispatch(packet) # 分发四元数数据包

                        # 解析加速度和陀螺仪
                        timestamps = int(decoded_data[0])  # 时间戳
                        accel_data = list(map(float, decoded_data[1:4]))  # 加速度数据
                        gyro_data = list(map(float, decoded_data[4:]))   # 陀螺仪数据
                        self.data_delegate.dispatch((timestamps, accel_data, gyro_data))  # 分发时间戳加速度陀螺仪数据包

                         # 写入文件
                        decoded_data_str = data.decode()
                        print("收到数据:", decoded_data_str)
                        threading.Thread(target=self.write_to_file, args=(decoded_data_str,)).start()

                    else:
                        print(f"数据包格式错误: {decoded_data}")

                except Exception as e:
                    print(f"数据解析错误: {e}")

    def close(self):
        self.sock.close()


class PacketDelegate: #基类，定义数据包处理的接口
    def loop(self, event):
        pass

    def dispatch(self, packet):
        pass


class EmptyPacketDelegate(PacketDelegate):
    def loop(self, event):
        pass

    def dispatch(self, packet):
        pass


class CubePacketViewer(PacketDelegate):
    def __init__(self):
        self.screen = Screen(480, 400, scale=1.5)
        self.cube = Cube(30, 60, 10)
        self.q = Quaternion(1, 0, 0, 0)
        self.previous = None
        self.latest = None

    def loop(self, event):
        if self.latest:
            self.cube.erase(self.screen)
            self.cube.draw(self.screen, self.latest)
            pygame.display.flip()
            self.latest = None

    def dispatch(self, packet):
        self.latest = packet

class DataPacketViewer(PacketDelegate):
    def __init__(self):
        # 初始化加速度和陀螺仪数据的历史记录
        self.timestamps = deque(maxlen=500)  # 时间戳队列
        self.accel_history = deque(maxlen=500)  # 加速度历史数据
        self.gyro_history = deque(maxlen=500)  # 陀螺仪历史数据

        # 初始化 Matplotlib 图形窗口
        self.fig, (self.ax_accel, self.ax_gyro) = plt.subplots(2, 1, figsize=(10, 6))
        self.ax_accel.set_title("Acceleration Data")
        self.ax_accel.set_xlabel("Time")
        self.ax_accel.set_ylabel("Acceleration (g)")
        self.ax_accel.set_ylim([-1, 1])  # 加速度的量程为 -1 到 1

        self.ax_gyro.set_title("Gyroscope Data")
        self.ax_gyro.set_xlabel("Time")
        self.ax_gyro.set_ylabel("Angular Velocity (°/s)")
        self.ax_gyro.set_ylim([-125, 125])  # 角速度的量程为 -125 到 125

    def loop(self, event):
        # 在主循环中更新波形图
        self.draw_accel_gyro()

    def dispatch(self, p):
        # 接收并处理加速度和陀螺仪数据包
        try:
            timestamp, accel_data, gyro_data = p
            self.timestamps.append(timestamp)
            self.accel_history.append(accel_data)
            self.gyro_history.append(gyro_data)
        except Exception as e:
            print(f"数据包解析错误: {e}")   
    
    def draw_accel_gyro(self):
        # 绘制加速度和陀螺仪数据的波形图
        self.ax_accel.clear()
        self.ax_gyro.clear()

        # 确保历史数据不为空
        if len(self.accel_history) > 0 and len(self.gyro_history) > 0:
            accel_array = np.array(self.accel_history)
            gyro_array = np.array(self.gyro_history)

            # 绘制加速度曲线
            self.ax_accel.plot(self.timestamps, accel_array[:, 0], label="Accel X", color="r")
            self.ax_accel.plot(self.timestamps, accel_array[:, 1], label="Accel Y", color="g")
            self.ax_accel.plot(self.timestamps, accel_array[:, 2], label="Accel Z", color="b")
            self.ax_accel.legend()

            # 绘制角速度曲线
            self.ax_gyro.plot(self.timestamps, gyro_array[:, 0], label="Gyro X", color="c")
            self.ax_gyro.plot(self.timestamps, gyro_array[:, 1], label="Gyro Y", color="m")
            self.ax_gyro.plot(self.timestamps, gyro_array[:, 2], label="Gyro Z", color="y")
            self.ax_gyro.legend()
        else:
            print("加速度或角速度历史数据为空，无法绘制波形图")  # 可添加标志变量，避免频繁打印

        # 更新图形
        plt.pause(0.01)

        

if __name__ == "__main__":

    pygame.init()

    # 初始化数据包处理器
    viewer = CubePacketViewer()
    data = DataPacketViewer()

    # 初始化数据读取器
    reader = PacketReader("0.0.0.0", 12345, quat_delegate=viewer, data_delegate=data)
    
    while 1:
        event = pygame.event.poll()

        if event.type == pygame.QUIT:
            viewer.close()
            break
        # if event.type == pygame.KEYDOWN:
        #     reader.write(pygame.key.name(event.key))

        reader.read()
        data.loop(event)
        # viewer.loop(event)
        

      # TODO: If system load is too high, increase this sleep time.
        pygame.time.delay(10)