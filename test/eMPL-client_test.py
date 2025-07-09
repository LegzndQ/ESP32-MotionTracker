# 基于 MPU6050 的 motion_driver_6.12 中的 python 上位机改写
# 通过 udp 协议接收数据，并可显示传感器姿态

import socket
import pygame
from ponycube import *
from euclid import Quaternion
import threading

class eMPLPacketReader:
    def __init__(self, ip, port, quat_delegate=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((ip, port))
        # self.sock.settimeout(0.1)

        self.quat_delegate = quat_delegate if quat_delegate else EmptyPacketDelegate()

    def read(self):
        buffer = ""  # 用于存储未处理的接收数据
        try:
            data, _ = self.sock.recvfrom(1024)
            buffer += data.decode("utf-8")
            
            # 按换行符分割数据
            lines = buffer.split("\n")
            buffer = lines[-1]  # 保留最后一个未完成的数据
            
            for line in lines[:-1]:
                decoded_data = line.strip().split(",")
                print("收到数据:", decoded_data)

                if len(decoded_data) == 11:
                    q_w, q_x, q_y, q_z = map(float, decoded_data[7:11]) # 左闭右开
                    packet = Quaternion(q_w, q_x, q_y, q_z)
                    self.quat_delegate.dispatch(packet)
                else :
                    print(f"数据格式错误: {decoded_data}")
                    continue

        except socket.timeout:
            pass

    def close(self):
        self.sock.close()


class PacketDelegate:
    def loop(self, event):
        pass

    def dispatch(self, packet):
        pass


class EmptyPacketDelegate(PacketDelegate):
    pass


class CubePacketViewer(PacketDelegate):
    def __init__(self):
        self.screen = Screen(480, 400, scale=1.5)
        self.cube = Cube(30, 60, 10)
        self.q = Quaternion(1, 0, 0, 0)
        self.latest = None

    def loop(self, event):
        if self.latest:
            self.cube.erase(self.screen)
            self.cube.draw(self.screen, self.latest)
            pygame.display.flip()
            self.latest = None

    def dispatch(self, packet):
        self.latest = packet


if __name__ == "__main__":
    pygame.init()
    viewer = CubePacketViewer()

    reader = eMPLPacketReader("0.0.0.0", 12345, quat_delegate=viewer)

    running = True
    while running:
        event = pygame.event.poll()
        if event.type == pygame.QUIT:
            running = False
        reader.read()
        viewer.loop(event)

    reader.close()
    pygame.quit()