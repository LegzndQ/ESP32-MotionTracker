import torch
import torch.nn as nn

class SRModel(nn.Module):
    def __init__(self, input_size=8, time_steps=10, output_size=2):
        """
        :param input_size: 每帧的输入维度 (IMU 传感器数据大小)
        :param time_steps: 序列长度 (前 10 帧)
        :param output_size: 输出维度 (PWM 控制信号大小)
        """
        super(SRModel, self).__init__()

        self.flatten_size = input_size * time_steps  # 10帧 * 每帧6个参数 = 60维输入

        self.fc = nn.Sequential(
            nn.Linear(self.flatten_size, 1024),
            nn.ReLU(),
            nn.Linear(1024, 64),
            nn.ReLU(),
            nn.Linear(64, output_size)
        )

    def forward(self, x):
        batch_size = x.shape[0]
        x = x.view(batch_size, -1)  # 展平为 (batch_size, 60)
        out = self.fc(x)
        return out

# 测试模型
if __name__ == "__main__":
    model = SRModel()
    dummy_input = torch.randn(32, 10, 6)  # (batch_size=32, time_steps=10, input_size=6)
    output = model(dummy_input)
    print("Output shape:", output.shape)  # 预期输出: (32, 2)
