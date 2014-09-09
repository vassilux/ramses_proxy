/*
 * sessionManager.cpp
 *
 * Created on: Oct 23, 2010
 * Author: vassilux
 */

#include "sessionManager.h"
#include "udpRelayServer.h"

namespace rart
{

SessionManager::SessionManager(boost::asio::io_service& ioservice, UdpRelayServer* pUdpRelayServer) :
	m_ioservice(ioservice),
	m_pUdpRelayServer(pUdpRelayServer)
{
	log_notice(rart_category, "SessionManager::ctor()");
}

SessionManager::~SessionManager()
{
	log_notice(rart_category, "SessionManager::dtor()");
}

void SessionManager::add_session(RouterSessionPtr& spSession)
{
	log_notice(rart_category, "Add new session for the udp peer  [%s].", spSession->get_adr_tuple().to_string().c_str());
	m_udpSessions[spSession->get_adr_tuple()] = spSession;
}

void SessionManager::remove_session(AdrTuple& adrTuple)
{
	log_info(rart_category, "Remove the peer [%s] session.", adrTuple.to_string().c_str());
	UdpSessionsMap::iterator it = m_udpSessions.find(adrTuple);
	if (it != m_udpSessions.end())
	{
		m_udpSessions.erase(it);
		log_info(rart_category, "Session removed for the peer [%s] session.", adrTuple.to_string().c_str());
	}else
	{
		log_error(rart_category, "Can't find and remove the peer [%s] session.", adrTuple.to_string().c_str());
	}
}

void SessionManager::handle_connection_failed(RouterSessionPtr& spSession)
{
	remove_session(spSession->get_adr_tuple());
}

void SessionManager::deliver_to_udp_peer(RouterSessionPtr spSession)
{
	log_debug(rart_category, "Delivring the tcp packet to the udp peer.");
	m_pUdpRelayServer->deliver_to_udp_peer(spSession);
}

void SessionManager::deliver_from_udp_peer(AdrTuple& adrTuple,  const NetworkBuffer& recvBuffer, std::size_t nBytesReceived)
{
	UdpSessionsMap::iterator it;

	for (it = m_udpSessions.begin(); it != m_udpSessions.end(); it++)
	{
		if (it->second->get_adr_tuple() == adrTuple)
		{
				break;
		}
	}
	//prepare udp buffer 
	boost::uint32_t nPacketSize = 0;
	std::stringstream ss;
	ss << adrTuple;
	std::string sPeer = adrTuple.to_string();
	nPacketSize = sPeer.length();
	nPacketSize += nBytesReceived;
	RouterPacket udpPacket;
	//
	bool res = udpPacket.set_peer_data(sPeer.c_str(), sPeer.length());
	if (false == res)
	{
		log_error(rart_category, "Cannot set the peer data for the peer [%s], the udp packet is ignored.", sPeer.c_str());
		return;
	}
	//
	res = udpPacket.set_packet_data(recvBuffer.data(), nBytesReceived);
	if (false == res)
	{
		log_error(rart_category, "Cannot set the packet data for the peer [%s], the udp packet is ignored.", sPeer.c_str());
		return;
	}
	//
	udpPacket.encode_header();
	//end prepare udp buffer 
	if(it == m_udpSessions.end())
	{
		//dont' find the udp peer session
		log_info(rart_category, "Create new session for the peer [%s].", adrTuple.to_string().c_str());
		RouterSessionPtr spSession(new RouterSession(m_ioservice, this));
		spSession->set_adr_tuple(adrTuple);
		add_session(spSession);
		spSession->add_router_packet(udpPacket);
	}
	else
	{
		log_info(rart_category, "Process data from udp the peer [%s].", adrTuple.to_string().c_str());
		it->second->add_router_packet(udpPacket);
	}
}

ParamsManagerWPtr SessionManager::getParamsManager()
{

	return m_pUdpRelayServer->getParamsManager();
}

int SessionManager::add_tcp_packet( RouterPacket& packet )
{
	int nRes = 0;
	bool bWriteInProgress = m_tcpPacketQueue.empty();
	m_tcpPacketQueue.push_back(packet);
	log_info(rart_category, "New packet added to the tcp packet queue, the queue size is [%d].", m_tcpPacketQueue.size());
	if (false == bWriteInProgress)
	{
		log_info(rart_category, "Start the delivering process tho the udp peer.");
		m_pUdpRelayServer->deliver_to_udp_peer();
	}
	return nRes;
}

RouterPacket& SessionManager::peek_tcp_packet()
{
	return m_tcpPacketQueue.front();
}

void SessionManager::pop_tcp_packet()
{
	m_tcpPacketQueue.pop_front();
}

bool SessionManager::tcp_queue_empty()
{
	bool bEmpty = true;
	bEmpty = m_tcpPacketQueue.empty();
	return bEmpty;
}


}
