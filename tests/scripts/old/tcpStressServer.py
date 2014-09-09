import SocketServer
import sys
import string
import time

host = "192.168.1.1"

port = 3500
print "-" * 40
print "TCP Echo server running on %s port %s.  Control-C to quit." % (host, port)
print "-" * 40

class DummyEchoHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        print "Session opened with :", self.client_address
        f = self.request.makefile()
        for line in f:
            print "I got data %s " % line
            print "slepp "
            time.sleep(.10)
            if line.find("Bye") > 0:
                break           
            #senddata = "0025192.168.001.002:02011data"
            print " Data to client %s " % line
            self.request.send(line)
        
        print "Session closed with client", self.client_address
        f.close()


myServer = SocketServer.ThreadingTCPServer((host, port),
                DummyEchoHandler)
myServer.serve_forever()
