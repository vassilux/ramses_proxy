from socket import *
import string
import time

host = "192.168.1.1"

port = 2036

ADDR = (host, port)

UDPSock = socket(AF_INET,SOCK_DGRAM)

print "\n"

print "\nStarting client end.  Control-C to quit."

print "\nOur target:"
print "udp_stress_client.py running on %s port %s" % (host, port)

print "\n\nEnter number of bytes to send and the number of times to send them:\n(for instance '100 10' to send 10 bursts of 100 bytes each)";

while (1):
    data = raw_input('% ')
    args = string.split(data)

    try:
                if args[0] == "reset":
                    data = "X"
                    numtimes = 1
                else:            
                    data = "X" * int(args[0]) + '\n'
                    numtimes = int(args[1])
    except:
                data = None
                numtimes = None
                print "Error, you need to specify two numbers.  First the number of bytes to send, second the number of times to send them."
                
    if not data:
        pass
    else:
            try:
                #the resetter...
                #UDPSock.sendto("X", (host,port))
                
                for x in range(numtimes):
                    line = "packet %04d:"%x
                    line += data
                    if(UDPSock.sendto(line,ADDR)):
                        print "Send packed [%s]" %(line)
                        buffeSize = len(line)
                        response, ADDR = UDPSock.recvfrom(buffeSize)
                        if not response:
                            print "Can't get data"
                        else:
                            print "Response packed [%s]" %(line)
                    else:
                        print "."
                    print ""
                    print ""
                    time.sleep(0.200)
                                   
                UDPSock.sendto("Bye\n",(host,port))
                print "Done."
                

            except:
                print "Send failed"


UDPSock.close()
