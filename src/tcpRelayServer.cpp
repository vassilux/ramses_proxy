/*
 * tcpRelayServer.cpp
 *
 *  Created on: Nov 27, 2010
 *  Author: vassilux
 */

#include "tcpRelayServer.h"

namespace rart
{

TcpRelayServer::TcpRelayServer(boost::asio::io_service& ioservice,
		ParamsManagerPtr spParamsManaerPtr, SessionManagerPtr spSessionManager) :
	m_ioservice(ioservice),
	m_wpParamsManager(spParamsManaerPtr), m_acceptor(
			ioservice),
	m_spSessionManager(spSessionManager),
			m_spUdpSession(new RouterSession(m_ioservice,
			m_spSessionManager.get()))
{

}

TcpRelayServer::~TcpRelayServer()
{

}

int TcpRelayServer::open()
{
	int nRes = -1;
	try
	{
		boost::asio::ip::tcp::resolver resolver(m_ioservice);
		ParamsManagerPtr spParamsManagerPtr(m_wpParamsManager);

		std::string address("0.0.0.0");
		//boost::asio::ip::tcp::resolver::query query(address,
		//spParamsManagerPtr->getTcpPort());
		//boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		 tcp::endpoint endpoint(tcp::v4(), spParamsManagerPtr->getTcpPort());
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(
				true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();
		nRes = 0;
		
	} catch (std::exception& e)
	{

	}

	return nRes;
}

int TcpRelayServer::start()
{
	int nRes = -1;
	try
	{
		nRes = open();
		if (0 == nRes)
		{
			m_acceptor.async_accept(m_spUdpSession->tcp_socket(), boost::bind(
					&TcpRelayServer::handle_accept, this,
					boost::asio::placeholders::error));
			nRes = 0;
		}

	} catch (std::exception& e)
	{

	}
	return nRes;
}

void TcpRelayServer::handle_accept(const boost::system::error_code& error)
{
	int nRes = -1;
	try
	{
		m_spUdpSession->start_read_from_tcp();

		m_spUdpSession.reset(new RouterSession(m_ioservice,
				m_spSessionManager.get())); // oups

		m_acceptor.async_accept(m_spUdpSession->tcp_socket(), boost::bind(
				&TcpRelayServer::handle_accept, this,
				boost::asio::placeholders::error));

		nRes = 0;
	} catch (std::exception& e)
	{

	}
}

int TcpRelayServer::stop()
{
	int nRes = -1;

	return nRes;
}

}
