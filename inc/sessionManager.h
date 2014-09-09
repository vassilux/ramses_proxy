/*
 * sessionManager.h
 *
 *  Created on: Oct 17, 2010
 *  Author: vassilux
 */

#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include "common.h"
#include "paramsManager.h"

#include "routerSession.h"

namespace rart
{

typedef std::map<AdrTuple, RouterSessionPtr> UdpSessionsMap;

class UdpRelayServer;

/**
 * @bref Session manager beetwen udp peers and TCP server(F1 server applicaiton)
 */
class SessionManager
{
private:
protected:
	// sessions map, by peer address port
	UdpSessionsMap m_udpSessions;
	boost::asio::io_service& m_ioservice;
	//udp dispatcher
	UdpRelayServer* m_pUdpRelayServer;
	//packet to udp peers
	PacketQueue m_tcpPacketQueue;

public:
	SessionManager(boost::asio::io_service& ioservice, UdpRelayServer* pUdpRelayServer);
	virtual ~SessionManager();

	void add_session(RouterSessionPtr& spSession);
	void remove_session(AdrTuple& adrTuple);
	void handle_connection_failed(RouterSessionPtr& spSession);
	void deliver_to_udp_peer(RouterSessionPtr spSession);
	void deliver_from_udp_peer(AdrTuple& adrTuple,  const NetworkBuffer& m_recvBuffer, std::size_t nBytesReceived);
	ParamsManagerWPtr getParamsManager();
	PacketQueue& get_tcp_packet_queue() { return m_tcpPacketQueue; };
	int add_tcp_packet(RouterPacket& packet);
	RouterPacket& peek_tcp_packet();
	void pop_tcp_packet();
	bool tcp_queue_empty();
};

typedef boost::shared_ptr<SessionManager> SessionManagerPtr;

}

#endif /* SESSIONMANAGER_H_ */
