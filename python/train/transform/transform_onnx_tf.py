import onnx
from onnx_tf.backend import prepare
import tensorflow as tf

# **3. ONNX 转 TensorFlow**
onnx_model = onnx.load("model.onnx")
tf_model = prepare(onnx_model)
tf_model.export_graph("tf_model")

# **4. TensorFlow 转 TensorFlow Lite**
converter = tf.lite.TFLiteConverter.from_saved_model("tf_model")
converter.optimizations = [tf.lite.Optimize.DEFAULT]  # 量化优化
tflite_model = converter.convert()

with open("model.tflite", "wb") as f:
    f.write(tflite_model)

print("✅ 模型已转换为 model.tflite，准备烧录到小车")
