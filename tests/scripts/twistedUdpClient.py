#
# Emulator for proxy laod test
#
#
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
from twisted.python import log
from twisted.internet.task import LoopingCall
import sys
from datetime import datetime
import os
import string
import random

host = "192.168.1.3"
port = 2036
my_id="1"

def id_generator(size=7, chars=string.ascii_uppercase + string.digits):
	callref = '@'
	generated = ''.join(random.choice(chars) for x in range(size))
	callref += generated
	return callref

class UdpClient(DatagramProtocol):
	def startProtocol(self):
		self.transport.connect(host, port)

	def doPolling(self):
		polling = "polling from %s at %s\n" %(my_id, datetime.now())
		log.msg("Send polling  : [%s]" % (polling))
		self.transport.write(polling)

	def doAlarm(self):
		alarm = "alarm from %s at %s\n" %(my_id,datetime.now())
		log.msg("Send alarm  : [%s]" % (alarm))
		self.transport.write(alarm)

	def datagramReceived(self, data, (host, port)):
		log.msg("Receive data from the server : [%r]." % (data))

if __name__ == "__main__":
    log.startLogging(sys.stdout)
    my_id=id_generator()
    udpclient = UdpClient()
    reactor.listenUDP(0, udpclient)
    #reactor.callWhenRunning(udpclient.doPolling)
    lcPolling = LoopingCall(udpclient.doPolling)
    lcPolling.start(1.0)
    #
    lcAlarm = LoopingCall(udpclient.doAlarm)
    lcAlarm.start(5.0)
    reactor.run()

