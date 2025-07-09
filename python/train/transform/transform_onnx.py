import torch.onnx
from models import SRModel  # 你的模型定义

# **1. 加载 PyTorch 训练好的模型**
model = SRModel()
checkpoint = torch.load("../results/checkpoint_1000.pth", map_location=torch.device('cpu'))
model.load_state_dict(checkpoint['model'])
model.eval()

# **2. 转换为 ONNX**
dummy_input = torch.randn(1, 10 * 8)  # 假设输入是 10 帧 IMU 数据 (10x6=60)
torch.onnx.export(model, dummy_input, "model.onnx", export_params=True, opset_version=11, input_names=["input"], output_names=["output"])

