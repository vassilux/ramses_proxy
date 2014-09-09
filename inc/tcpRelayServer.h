/*
 * tcpRelayServer.h
 *
 *  Created on: Nov 27, 2010
 *  Author: vassilux
 */

#ifndef TCPRELAYSERVER_H_
#define TCPRELAYSERVER_H_

#include "common.h"
#include "logging.h"
#include "sessionManager.h"
#include "paramsManager.h"
#include "routerSession.h"

namespace rart
{
using boost::asio::ip::tcp;

class TcpRelayServer
{
private:

protected:
	asio::io_service& m_ioservice;
	tcp::acceptor m_acceptor;

	SessionManagerPtr m_spSessionManager;
	ParamsManagerWPtr m_wpParamsManager;

	RouterSessionPtr m_spUdpSession;
public:
	TcpRelayServer(boost::asio::io_service& ioservice,
			ParamsManagerPtr spParamsManaerPtr, SessionManagerPtr spSessionManager);
	virtual ~TcpRelayServer();

	int open();

	int start();
	int stop();

	void handle_accept(const boost::system::error_code& error);
};

}

#endif /* TCPRELAYSERVER_H_ */
