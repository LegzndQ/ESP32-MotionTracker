import tensorflow as tf
import numpy as np

interpreter = tf.lite.Interpreter(model_path="./transform/model.tflite")
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# 80 个 float 输入
input_data = np.array([
    1.15,-0.45,0.67,1.15,-0.45,0.67,226.42,226.42,
    0.99,-0.25,0.46,0.99,-0.25,0.46,571.43,571.43,
    1.76,-0.57,0.31,1.76,-0.57,0.31,808.63,808.63,
    1.64,-0.66,0.24,1.64,-0.66,0.24,938.01,938.01,
    1.96,-0.87,0.86,1.96,-0.87,0.86,1015.87,1015.87,
    3.3,-1.9,3.06,3.3,-1.9,3.06,1090.91,1059.74,
    3.79,-2.02,5.18,3.79,-2.02,5.18,1120.88,1120.88,
    2.08,-1.75,5.29,2.08,-1.75,5.29,1164.42,1132.08,
    1.55,-1.65,4.98,1.55,-1.65,4.98,1164.42,1132.08,
    1.68,-1.29,4.51,1.68,-1.29,4.51,1164.42,1132.08
], dtype=np.float32).reshape(input_details[0]["shape"])

interpreter.set_tensor(input_details[0]["index"], input_data)
interpreter.invoke()

output = interpreter.get_tensor(output_details[0]["index"])
print("输出:", output)