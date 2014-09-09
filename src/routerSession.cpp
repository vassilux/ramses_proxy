/*
 * session.cpp
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */
#include "common.h"
#include "paramsManager.h"
#include "routerSession.h"
#include "sessionManager.h"

namespace rart
{

RouterSession::RouterSession(boost::asio::io_service& ioservice,
		SessionManager* pSessionManager) :
	m_ioservice(ioservice), m_resolver(ioservice), m_pSessionManager(
			pSessionManager), m_socket(ioservice), m_bConnecting(false)
{
	log_debug(rart_category, "RouterSession::ctor()");
}

RouterSession::~RouterSession()
{
	log_debug(rart_category, "RouterSession::dtor() for the udp peer [%s]", m_udpTuple.to_string().c_str());
}

/**
 * @bref Start session job
 */
void RouterSession::start()
{
	log_info(rart_category, "Start session for the peer :  [%s]", m_udpTuple.to_string().c_str());
	ip::tcp::resolver resolver(m_ioservice);
	ParamsManagerWPtr wpParamsManager = m_pSessionManager->getParamsManager();
	ParamsManagerPtr spParamsManager(wpParamsManager.lock());
	std::string sServer;

	sServer = spParamsManager->getF1ServerAdr();
	std::stringstream ssPort;
	ssPort << spParamsManager->getF1Port();
	log_debug(rart_category, "Start the connection to the F1 Server on address [%s] for the peer [%s]", sServer.c_str(), m_udpTuple.to_string().c_str());
	ip::tcp::resolver::query query(sServer, ssPort.str());
	m_resolver.async_resolve(query, boost::bind(&RouterSession::handle_resolve,
			shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));

}

/**
 *@bref Add un packet from udp peer
 * @param udpBuffer, raw data
 * @param nSize, size of data
 */
void RouterSession::set_udp_buffer(const NetworkBuffer& udpBuffer,
		boost::uint32_t nSize)
{
	log_debug(rart_category, "The udp packet is preparing for the peer : [%s]", m_udpTuple.to_string().c_str());
	boost::uint32_t nPacketSize = 0;
	std::stringstream ss;
	ss << m_udpTuple;
	std::string sPeer = m_udpTuple.to_string();
	nPacketSize = sPeer.length();
	nPacketSize += nSize;
	RouterPacket udpPacket;

	bool res = udpPacket.set_peer_data(sPeer.c_str(), sPeer.length());
	const char* pcPeerAdrPort = m_udpTuple.to_string().c_str();
	if (false == res)
	{
		log_error(rart_category, "The udp packet preparing failed for the peer : [%s]", pcPeerAdrPort);
		return;
	}
	log_debug(rart_category, "The udp packet set to the peer [%s] queues", pcPeerAdrPort);
	res = udpPacket.set_packet_data(udpBuffer.data(), nSize);
	if (false == res)
	{
		log_error(rart_category, "Error to set the packet to the peer [%s] queues.", pcPeerAdrPort);
		return;
	}
	log_debug(rart_category, "Encode the header of the udp packet for the peer [%s]", pcPeerAdrPort);
	udpPacket.encode_header();
	//get state of packet queue
	bool bWriteInProgress = !m_udpPacketQueue.empty();

	m_udpPacketQueue.push_back(udpPacket);
	if (false == bWriteInProgress)
	{
		log_info(rart_category, "The udp que is empty for the peer [%s] start the delivering to the F1 server", pcPeerAdrPort);
		//it is not very clean but it works, for many threading implementation it is not good solution
		deliver_to_tcp_peer();
	}else{
		log_info(rart_category, "The udp que is not empty for the peer [%s] , I process the next time.", pcPeerAdrPort);
	}
}

void  RouterSession::add_router_packet(RouterPacket& udpPacket)
{
	//get state of packet queue
	bool bWriteInProgress = !m_udpPacketQueue.empty();
	char* pcPacketData = udpPacket.get_packet_data();
	m_udpPacketQueue.push_back(udpPacket);
	log_debug(rart_category, "The udp packet [%s] pushed to the delivering queue, actual queue size [%d]", pcPacketData, m_udpPacketQueue.size());
	if (false == bWriteInProgress)
	{
		log_info(rart_category, "Processe delivering the packet to the F1 server");
		//it is not very clean but it works, for many threading implementation it is not good solution
		deliver_to_tcp_peer();
	}else{
		log_debug(rart_category, "The udp packet will delivred next time", pcPacketData);
	}
}

/**
 * @bref Start tcp conexion to TCP server
 * @param endpoint_iter
 */
void RouterSession::start_tcp_connection(
		ip::tcp::resolver::iterator endpoint_iter)
{
	boost::system::error_code ec;
	std::string adr = endpoint_iter->endpoint().address().to_string(ec);
	if (endpoint_iter != ip::tcp::resolver::iterator())
	{
		log_info(rart_category, "Trying reconnect to the F1 server [%s]", adr.c_str());
		m_socket.async_connect(endpoint_iter->endpoint(), boost::bind(
				&RouterSession::handle_connection, this, _1, endpoint_iter));
	}
	else
	{
		// bye
		log_error(rart_category, "Cannot connect tho the F1 server on address  [%s]. Bye.", adr.c_str());
		close_session();
	}

}

/**
 * @bref Handle to result of TCP server connection
 * @param error
 * @param endpoint_iterator
 */
void RouterSession::handle_connection(const boost::system::error_code& error,
		ip::tcp::resolver::iterator endpoint_iterator)
{
	//
	if (!error && true == m_socket.is_open())
	{
		
		log_info(rart_category, "The F1 server is connected. Start read write process.");
		// 
		deliver_to_tcp_peer();
		// start read header data from TCP Server
		start_read_from_tcp();
		
		

	}
	else if (endpoint_iterator != ip::tcp::resolver::iterator())
	{
		// re try again
		log_info(rart_category, "The F1 server not found I re-try again.");
		m_socket.close();
		ip::tcp::endpoint endpoint = *endpoint_iterator;
		m_socket.async_connect(endpoint, boost::bind(
				&RouterSession::handle_connection, shared_from_this(),
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		//something wrong close session and bye,
		//next session will be create by another UDP packet
		log_info(rart_category, "The F1 server connection failed with error %s.", error.message().c_str());
		close_session();
	}

}

/**
 * @bref Hadle resolver result
 * @param error
 * @param endpoint_iterator
 */
void RouterSession::handle_resolve(const boost::system::error_code& error,
		ip::tcp::resolver::iterator endpoint_iterator)
{
	log_notice(rart_category, "Trying resolve the F1 server address.");
	if (!error)
	{
		ip::tcp::endpoint endpoint = *endpoint_iterator;
		m_socket.async_connect(endpoint, boost::bind(
				&RouterSession::handle_connection, shared_from_this(),
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else if (endpoint_iterator != ip::tcp::resolver::iterator())
	{
		m_socket.close();
		ip::tcp::endpoint endpoint = *endpoint_iterator;
		m_socket.async_connect(endpoint, boost::bind(
				&RouterSession::handle_connection, shared_from_this(),
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		log_error(rart_category, "Trying resolve close the sesson to the F1 Server for the peer [%s]", m_udpTuple.to_string().c_str());
		close_session();
	}

}

/**
 * @bref Closing session
 * <br>
 */
void RouterSession::close_session()
{
	m_ioservice.post(boost::bind(&SessionManager::handle_connection_failed,
			m_pSessionManager, shared_from_this()));
}

/**
 *Start receive header data from server
 *@see RouterPacket
 */
void RouterSession::start_read_from_tcp()
{
	log_debug(rart_category, "Start read from the F1 server tcp connection for the peer [%s]", m_udpTuple.to_string().c_str());
	boost::asio::async_read(m_socket, asio::buffer(m_fromTcpPacket.data(),
			RouterPacket::HEADER_LENGTH), boost::bind(
			&RouterSession::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

}

/**
 * @bref Handle of header data
 * @param error
 * @param nBytesLength,
 */
void RouterSession::handle_read_header(const boost::system::error_code& error,
		size_t nBytesLength)
{
	if (nBytesLength == 0)
	{
		//not data close session
		log_notice(rart_category, "Receive 0 bytes for header from F1 Server for the peer [%s]", m_udpTuple.to_string().c_str());
		close_session();
		return;
	}
	if (!error)
	{
		
		if(true == m_fromTcpPacket.decode_header() )
		{
			log_debug(rart_category, "I get the header packet of the size [%d] from F1 Server for the peer [%s]", m_fromTcpPacket.payload_length(), m_udpTuple.to_string().c_str());
			//TRACE(RART_LOG_TRACE, "Header packet received , wait for payload  length : [" << m_fromTcpPacket.payload_length() << "]");
			asio::async_read(m_socket, asio::buffer(m_fromTcpPacket.payload(),
				m_fromTcpPacket.payload_length()), boost::bind(
				&RouterSession::handle_read_payload, shared_from_this(),
				asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			log_info(rart_category, "Error to decode payload size from F1 Server, I close session for the peer [%s]", m_udpTuple.to_string().c_str());
			close_session();
			return;
		}
		
	}
	else
	{
		close_session();
	}
}

/**
 * @bref
 * @param error
 * @param nBytesLength
 */
void RouterSession::handle_read_payload(const boost::system::error_code& error,
		size_t nBytesLength)
{
	if (nBytesLength == 0)
	{
		log_info(rart_category, "Receive 0 for payload, connection is closed by remote (F1) for the peer [%s]", m_udpTuple.to_string().c_str());
		close_session();
	}
	if (!error)
	{
		if(0 == nBytesLength)
		{
			m_fromTcpPacket.clear();
			return;
		}
		if(true == m_fromTcpPacket.decode_peer())
		{
			log_debug(rart_category, "I get the message [%s]  of the size [%d] from F1 Server for the peer [%s]", 
				m_fromTcpPacket.data(), 
				m_fromTcpPacket.data_length(), 
				m_udpTuple.to_string().c_str());
			m_pSessionManager->add_tcp_packet(m_fromTcpPacket);
			m_fromTcpPacket.clear();
			// got data from tcp endpoint and send to an udp endpoint
			m_pSessionManager->deliver_to_udp_peer(shared_from_this());			
			//
			start_read_from_tcp();
		}
		else
		{
			m_fromTcpPacket.clear();
		}
	}
	else
	{
		close_session();
		return;
	}
}

/**
 * @bref Handle delivring to TCP server (F application)
 * @param error
 */
void RouterSession::handle_deliver_tcp(const boost::system::error_code& error)
{
	if (!error)
	{
		log_info(rart_category, "Pop the buffer from udp packet queue for the peer [%s]", m_udpTuple.to_string().c_str());
		m_udpPacketQueue.pop_front();
		//
		if(false == m_udpPacketQueue.empty())
		{
			deliver_to_tcp_peer();		
		}
	}
	else
	{
		log_error(rart_category, "TCP deliver error [%s] to the F1 server for the peer [%s]", error.message().c_str(), m_udpTuple.to_string().c_str());
		close_session();
	}
}

/**
 * @bref
 */
void RouterSession::deliver_to_tcp_peer()
{
	if (false == m_socket.is_open())
	{
		//first time
		start();
	}
	else
	{
		log_info(rart_category, "Check udp packet queue for the peer [%s]", m_udpTuple.to_string().c_str());
		if(m_udpPacketQueue.empty() == false)
		{
			RouterPacket& packetToTcp = m_udpPacketQueue.front();
			//
			log_debug(rart_category, "Delivring the message [%s] of the size [%d] for the peer [%s] to the F1 server", packetToTcp.data(), packetToTcp.data_length(), m_udpTuple.to_string().c_str());
			//
			boost::asio::async_write(m_socket, boost::asio::buffer(
				packetToTcp.data(), packetToTcp.data_length()), boost::bind(
				&RouterSession::handle_deliver_tcp, shared_from_this(), _1));
		}

	}
}

}
