with open("mosquitto.org.crt", 'rb') as f:
    lines = f.readlines()
for i, line in enumerate(lines):
    lines[i]=line.decode().replace('\n','\n"'.encode("unicode_escape").decode()+"\n")
command="u_char serverCert[]="
command = command +'"'+ '"'.join(lines)
command=command+";"
print(command)

