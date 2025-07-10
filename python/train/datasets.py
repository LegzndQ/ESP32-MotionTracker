import os
import pandas as pd
import numpy as np
import torch
from torch.utils.data import Dataset

class SRDataset(Dataset):
    def __init__(self, data_folder, mode='train', time_window=10):
        """
        :param data_folder: 数据文件所在文件夹根路径 (train_data.csv 和 val_data.csv 的路径)
        :param mode: 'train' 或者 'val'
        :param time_window: 滑动窗口大小 (用于时间序列建模)
        """
        self.data_folder = data_folder
        self.mode = mode.lower()
        assert self.mode in {'train', 'val'}

        # 选择训练集或验证集
        file_path = os.path.join(data_folder, 'train_data.csv' if self.mode == 'train' else 'val_data.csv')
        self.data = pd.read_csv(file_path)

        self.time_window = time_window
        self.samples = []

        for i in range(len(self.data) - time_window):
            self.samples.append(self.data.iloc[i:i + time_window, 1:].values)

        self.samples = np.array(self.samples)

    def __getitem__(self, index):
        sample = self.samples[index]
        inputs = sample[:, 1:9].flatten()  # 展平后的 IMU 数据，shape = (time_window * imu_feature_dim,)
        targets = sample[-1, -2:]  # 取最后一帧的 PWM 值

        return torch.tensor(inputs, dtype=torch.float32), torch.tensor(targets, dtype=torch.float32)

    def __len__(self):
        return len(self.samples)

# 测试代码
if __name__ == "__main__":
    dataset = SRDataset('./data')
    print(f"数据集大小: {len(dataset)}")
    sample_input, sample_target = dataset[0]
    print(f"输入形状: {sample_input.shape}{sample_input}")  # 预期: (time_window * imu_feature_dim,)
    print(f"目标形状: {sample_target.shape}{sample_target}")  # 预期: (2,)
