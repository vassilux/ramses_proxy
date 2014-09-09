/*
 * server.h
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "common.h"
#include "logging.h"
#include "sessionManager.h"
#include "paramsManager.h"

namespace rart
{
using boost::asio::ip::udp;

/**
 * @bref UDP server
 * <br>Receive udp peers packets and dispatch by SessionManager to TCP server(F1 application server)
 * @see SessionManager
 */
class UdpRelayServer
{
private:
protected:
	 boost::asio::io_service& m_ioservice;
	 udp::socket m_socket;
	 udp::endpoint m_remoteEndpoint;
	 //current udp recevie buffer
	 NetworkBuffer m_recvBuffer;
	 //session manager
	 SessionManagerPtr m_spSessionManager;
	 //params manager
	 ParamsManagerWPtr m_wpParamsManager;
public:
	UdpRelayServer(boost::asio::io_service& ioservice, ParamsManagerPtr spParamsManaerPtr);
	virtual ~UdpRelayServer();

	 void start_receive();
	 void handle_receive(const boost::system::error_code& error, std::size_t nBytesReceived);

	 void deliver_to_udp_peer(RouterSessionPtr& spSession);

	 void handle_deliver_to_udp_peer(const boost::system::error_code& error, size_t nBytesSent);

	 ParamsManagerWPtr getParamsManager() { return m_wpParamsManager; }

	 SessionManagerPtr getSessionManager() { return m_spSessionManager; }

	 void deliver_to_udp_peer();

};

}

#endif /* SERVER_H_ */
