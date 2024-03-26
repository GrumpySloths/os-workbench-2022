import sys

# 从C程序中读取数据
data_from_c = sys.stdin.read()

# 打印接收到的数据
print("Data received from C program:")
print(data_from_c)
