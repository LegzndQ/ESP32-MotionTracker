# 导入系统库
import time

# 导入PyTorch库
import torch
from torch import nn
from torch.utils.tensorboard import SummaryWriter

# 导入自定义库
from datasets import SRDataset
import models


def main():
    # 数据目录
    data_folder = "./data"

    # 学习参数
    checkpoint = None #'./results/checkpoint.pth'  # 预训练模型路径，如果不存在则为None
    batch_size = 400  # 批大小
    start_epoch = 1  # 轮数起始位置
    epochs = 10000  # 迭代轮数
    lr = 1e-4  # 学习率
    time_window = 10

    # 定义设备运行环境
    device = (
        "cuda"
        if torch.cuda.is_available()
        else "mps"
        if torch.backends.mps.is_available()
        else "cpu"
    )

    writer = SummaryWriter()  # 实时监控     使用命令 tensorboard --logdir runs  进行查看
    # 初始化模型
    model = models.SRModel(time_steps=time_window).to(device)


    # 定义损失函数和优化器
    criterion = nn.MSELoss().to(device)
    # 初始化优化器
    optimizer = torch.optim.Adam(params=filter(lambda p: p.requires_grad, model.parameters()), lr=lr)

    # 加载预训练模型
    if checkpoint is not None:
        checkpoint = torch.load(checkpoint)
        start_epoch = checkpoint['epoch'] + 1
        model.load_state_dict(checkpoint['model'])
        optimizer.load_state_dict(checkpoint['optimizer'])

    train_dataset = SRDataset(data_folder, mode='train', time_window=time_window)
    train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=1,
                                               pin_memory=True)

    # 训练循环
    for epoch in range(start_epoch, epochs + 1):
        model.train()
        epoch_loss = 0
        start_time = time.time()

        for i, (inputs, targets) in enumerate(train_loader):
            inputs, targets = inputs.to(device), targets.to(device)

            # 前向传播
            outputs = model(inputs)
            loss = criterion(outputs, targets)

            # 反向传播与优化
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            epoch_loss += loss.item()

        # 手动释放内存
        del inputs, targets, outputs, loss

        # 监控损失值变化
        avg_loss = epoch_loss / len(train_loader)
        elapsed_time = time.time() - start_time
        writer.add_scalar('MSE_Loss', avg_loss, epoch)

        if epoch % 100 == 0:
            torch.save({
                'epoch': epoch,
                'model': model.state_dict(),
                'optimizer': optimizer.state_dict()
            }, f'results/checkpoint_{epoch}.pth')


        print(f"Epoch {epoch}/{epochs}, Loss: {avg_loss:.4f}, Time: {elapsed_time:.2f}s")
    # 训练结束关闭监控
    writer.close()

if __name__ == '__main__':
    main()
