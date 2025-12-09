with open("iopress.key", 'rb') as f:
    lines = f.readlines()
for i, line in enumerate(lines):
    lines[i]=line.decode().replace('\n','\n"'.encode("unicode_escape").decode()+"\n")
command="u_char key[]="
command = command +'"'+ '"'.join(lines)
command=command+";"
print(command)

with open("iopress.crt", 'rb') as f:
    lines = f.readlines()
for i, line in enumerate(lines):
    lines[i]=line.decode().replace('\n','\n"'.encode("unicode_escape").decode()+"\n")
command="u_char cert[]="
command = command +'"'+ '"'.join(lines)
command=command+ ";"
print(command)