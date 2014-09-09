#
# F1 style mock server
# used for emulate the F1 server for raRt test, it doesn't use for the applicaton logic jsut for route logic 
#
#
import os
import sys
import re
import time
import logging
import optparse
import signal
import sys, warnings
import struct
import string

host = "192.168.1.1"
port = 3500
g_timeout = 30

try:
    from twisted.python import log
    from twisted.internet import reactor, protocol
    from twisted.internet.protocol import ServerFactory
    from twisted.protocols.basic import _PauseableMixin
    from twisted.python import failure
    from twisted.internet import reactor, task, defer
    from twisted.internet import error as tw_error
    from twisted.application import internet
    from twisted.application import service
    from twisted.internet.defer import inlineCallbacks

except ImportError:
    print "You need twisted matrix 10.1+ to run AMIWORKER. Get it from http://twistedmatrix.com/"
    sys.exit(1)


class F1ComProtocol(protocol.Protocol, _PauseableMixin):
    """This is just about the simplest possible protocol"""
    __msg_buffer = ""

    #def __init__(self, factory):
    #    self.factory = factory;

    def connectionMade(self):
        d = self.transport.getPeer()
        log.msg("F1ComProtocol : New connection from [%s:%s]." % (d.host, d.port))
        self._timeout = reactor.callLater(g_timeout, self.doTimeout)

    def doTimeout(self):
    	log.msg("F1ComProtocol : Timeout of the client connexion not received data.")
    	self.transport.loseConnection()

    def dataReceived(self, data):
    	self._timeout.reset(g_timeout)
        log.msg("F1ComProtocol : New data [ %s ] of length %i " %(data.encode('hex'), len(data)))
        self.__msg_buffer = self.__msg_buffer + data
        try:
            #check the length of the data
            while len(self.__msg_buffer) > 8 and not self.paused:
                #
                (length) = string.atoi(self.__msg_buffer[0:4])
                (lengthAdr) = string.atoi(self.__msg_buffer[4:8])
                if len(self.__msg_buffer) >= length:
                	#can process
                	log.msg("longeur is ok  ")
                	response = self.__msg_buffer[0:length + 4]
                	log.msg("response [ %s ]  "%(response))
                	self.transport.write(response)
                	self.__msg_buffer = self.__msg_buffer[length + 4:]
                	log.msg("__msg_buffer [ %s ]  "%(self.__msg_buffer))
                else:
                	break
                #
                
            #end while
        except Exception,e:
            log.msg("F1ComProtocol : Get an exception [%s]. Must close the connection " % (e))
            self.transport.loseConnection()

    def connectionLost(self, reason):
        d = self.transport.getPeer()
        log.msg("F1ComProtocol : Connection lost , I remove myself [%s:%s] from factory." % (d.host, d.port))



if __name__ == "__main__":
    log.startLogging(sys.stdout)
    factory = protocol.Factory()
    factory.protocol = F1ComProtocol
    reactor.listenTCP(port, factory, interface=host)
    reactor.run()
