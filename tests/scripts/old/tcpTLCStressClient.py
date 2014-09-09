import socket

host = "127.0.0.1"
port = 3500

# 
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#
s.connect(host,port)

#
s.send("30303035313130334E14322D313630352C30303035313130332C392C31")
while True:
    resp = s.recv(1024)
    if resp == "": break
    print resp

# 
s.close()
print "\nBye, done"