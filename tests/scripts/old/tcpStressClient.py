import socket

host = "127.0.0.1"
port = 3500

# 
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#
s.connect(host,port)

#
s.send("  26  15127.0.0.1:50844XXXXXXXXXX")
while True:
    resp = s.recv(1024)
    if resp == "": break
    print resp,

# 
s.close()
print "\nBye, done"