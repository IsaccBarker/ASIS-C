string = input("> ")

print("Push: A " + hex(len(string)+2).replace("0x", ""), end = " ")

for i, v in enumerate(string):
    print(hex(ord(v)).replace("0x", ""), end = " ")

print("0 0")

print("Print: C " + hex(len(string)).replace("0x", ""))
print("Pop: 9 " + hex(len(string)).replace("0x", ""))
