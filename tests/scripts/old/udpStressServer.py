from socket import *
import time

# we want to bind on all possible IP addresses
host = "0.0.0.0"

#if you change the port, change it in the client program as well
port = 3501
buffer = 102400

# Create socket and bind to address
UDPSock = socket(AF_INET,SOCK_DGRAM)
UDPSock.bind((host,port))

time.time()
print "\n"
print "-" * 40
print "udp stress server "
print "Starting UDP receive server...  control-break to exit."
print "-" * 40
print "\nWaiting for data..."

# total bytes recieved since last 'reset'
totalbytes = 0

# -1 is a deliberately invalid timestamp
timestamp = -1

# the total number of bursts that have come in
totalrcvs = 0

while 1:
    data,addr = UDPSock.recvfrom(buffer)
    
    if not data:
        print "No data."
        break
    else:
        donestamp = time.time()
        
        data = len(data)
        totalbytes += data
        totalrcvs += 1

        rate = totalbytes/(donestamp - timestamp) * 8 / 1000
        print "\nRcvd: %s bytes, %s total in %s s at %s kbps" % (data, totalbytes, donestamp - timestamp, rate)

        if data == 1:
                    # this is the reset, send one byte to trigger
                    totalbytes = 0
                    timestamp = time.time()
                    totalrcvs = 0
                    print "Reset recieved, clearing statistics."

UDPSock.close()
