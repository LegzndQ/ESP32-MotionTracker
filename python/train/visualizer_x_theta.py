import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# ---------- 读取数据 ----------
file_path = "./data/data3.csv"  # 替换为你的文件路径
df = pd.read_csv(file_path, sep=None, engine='python', usecols=range(7), header=None)
df.columns = ['Timestamp', 'Ax', 'Ay', 'Az', 'Gx', 'Gy', 'Gz']

# ---------- 时间处理 ----------
time = df['Timestamp'].values / 1000.0  # 从毫秒转换为秒
dt = np.diff(time, prepend=time[0])  # 计算时间差（首项为0）

# ---------- 提取加速度和角速度 ----------
ax = df['Ax'].values * 9.8
ay = df['Ay'].values * 9.8
az = df['Az'].values * 9.8
# -------- 零点漂移补偿（静态前 100 帧） --------
bias_ax = np.mean(ax[:100])
bias_ay = np.mean(ay[:100])
bias_az = np.mean(az[:100])

# 应用补偿
ax -= bias_ax
ay -= bias_ay
az -= bias_az

gx = df['Gx'].values
gy = df['Gy'].values
gz = df['Gz'].values

# --------- 陀螺仪零偏补偿（单位 deg/s） ---------
bias_gx = np.mean(gx[:100])
bias_gy = np.mean(gy[:100])
bias_gz = np.mean(gz[:100])

gx -= bias_gx
gy -= bias_gy
gz -= bias_gz

# ---------- 一次积分：速度 ----------
vx = np.cumsum(ax * dt)
vy = np.cumsum(ay * dt)
vz = np.cumsum(az * dt)

# ---------- 二次积分：位移 ----------
sx = np.cumsum(vx * dt)
sy = np.cumsum(vy * dt)
sz = np.cumsum(vz * dt)

# ---------- 角速度积分：角度 ----------
theta_x = np.cumsum(gx * dt)
theta_y = np.cumsum(gy * dt)
theta_z = np.cumsum(gz * dt)

# ---------- 可视化 ----------
plt.figure(figsize=(12, 6))

# --- 位移图 ---
plt.subplot(2, 1, 1)
plt.plot(time, sx, label='Sx')
plt.plot(time, sy, label='Sy')
plt.plot(time, sz, label='Sz')
plt.title('x')
plt.ylabel('x [m]')
plt.xlabel('t [s]')
plt.legend()
plt.grid(True)

# --- 角度图 ---
plt.subplot(2, 1, 2)
plt.plot(time, theta_x, label='Angle X')
plt.plot(time, theta_y, label='Angle Y')
plt.plot(time, theta_z, label='Angle Z')
plt.title('theta')
plt.ylabel('theta [°]')
plt.xlabel('t [s]')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()