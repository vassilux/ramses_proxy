#include "common.h"
#include "udpRelayServer.h"
#include "util.h"


namespace rart
{

UdpRelayServer::UdpRelayServer(boost::asio::io_service& ioservice, ParamsManagerPtr spParamsManaerPtr) :
		m_wpParamsManager(spParamsManaerPtr),
	m_ioservice(ioservice), m_socket(ioservice, udp::endpoint(udp::v4(), spParamsManaerPtr->getUDpPort())),
	m_spSessionManager(new SessionManager(ioservice, this))

{
	log_debug(rart_category, "UdpRelayServer:: %s.", "ctor");
}

UdpRelayServer::~UdpRelayServer()
{
	log_debug(rart_category, "UdpRelayServer::%d.", "dtor");
}

/**
 * @bref Start serer job
 */
void UdpRelayServer::start_receive()
{
	log_debug(rart_category, "UdpRelayServer::start_receive.", "dtor");
	//TRACE(RART_LOG_TRACE, "UdpRelayServer::start_receive");
	m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer),
			m_remoteEndpoint, boost::bind(&UdpRelayServer::handle_receive,
					this, boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

}

/**
 * @bref handle of receive data
 * @param error
 * @param nBytesReceived
 */
void UdpRelayServer::handle_receive(const boost::system::error_code& error,
		std::size_t nBytesReceived)
{
	//initialize peer address
	AdrTuple adrTuple(AdrTuple::UDP, m_remoteEndpoint.address(), m_remoteEndpoint.port());
	//TRACE(RART_LOG_DEBUG, "Packet from UDP peer " << adrTuple << " " << makeHex(m_recvBuffer.data(), nBytesReceived));
	log_debug(rart_category, "Packet from UDP peer [%s] -> [%s] size %d", adrTuple.to_string().c_str(), m_recvBuffer.data(), nBytesReceived);

	if (!error || nBytesReceived > 0 )
	{
		//dispatch data to target session
		m_spSessionManager->deliver_from_udp_peer(adrTuple, m_recvBuffer, nBytesReceived);
		//clear
		m_recvBuffer.clear();
		//memset(m_recvBuffer.c_array(), 0, NETWOR_BUFFER_SIZE);
	}
	else
	{
		m_spSessionManager->remove_session(adrTuple);
	}
	start_receive();
}

/**
 * @bref Depricated
 * @param spSession
 *
 */
void UdpRelayServer::deliver_to_udp_peer(RouterSessionPtr& spSession)
{
   if(true == m_spSessionManager->tcp_queue_empty())
   {
   		log_info(rart_category, "tcp packet queue is empty ");
		return;
   }
   RouterPacket& packet = m_spSessionManager->peek_tcp_packet();
   char szcPeerData[128] = "";
   char szcPeerAdr[32] = "";
   char szcPeerPort[8] = "";
   bool res = packet.get_peer_data(szcPeerData, 128);
   if(false == res)
   {
   		log_warn(rart_category, "Packet udp peer address error, erase pop element in the queue.");
	   //TRACE(RART_LOG_ERROR, "Packet udp peer address error, erase pop element in the queue");
	   m_spSessionManager->pop_tcp_packet();
	   return;
   }
   split_addr (szcPeerData, szcPeerAdr, szcPeerPort);
   udp::endpoint udpPeer;
   udpPeer.address(ip::address::from_string(szcPeerAdr));
   udpPeer.port(atoi(szcPeerPort));

   char* pcPacketData = packet.get_packet_data();
   boost::uint32_t nDataSize = packet.packet_data_length();
   //TRACE(RART_LOG_DEBUG, "Message to UDP peer : [" << szcPeerAdr << ":" << szcPeerPort << " " << makeHex(pcPacketData, nDataSize) << "]");
   log_info(rart_category, "Packet udp peer address error, erase pop element in the queue.");

   m_socket.async_send_to(
             boost::asio::buffer(pcPacketData, nDataSize), udpPeer,
             boost::bind(&UdpRelayServer::handle_deliver_to_udp_peer, this,
                           boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));

}

void UdpRelayServer::handle_deliver_to_udp_peer(const boost::system::error_code& error, size_t nBytesSent)
{
	if(error)
	{
		log_error(rart_category, "I get the error :  %s", error.message().c_str());
		SYS_LOG(LOG_WARNING,  " " << error.message() << ".");

	}
	else
	{
		
		if( false == m_spSessionManager->tcp_queue_empty())
		{
			RouterPacket& packet = m_spSessionManager->peek_tcp_packet();
			char* pcPacketData = packet.get_packet_data();
			log_info(rart_category, "The tcp packet [%s] delivred and bytes wroten %d.", pcPacketData, nBytesSent);
			m_spSessionManager->pop_tcp_packet();
			//send the next message 
			deliver_to_udp_peer();
		}else{
			log_info(rart_category, "The tcp packet delivred and bytes wroten %d and the packets queue is empty.", nBytesSent);
		}
		

	}
}

/**
 * @bref Deliver pop packet from tcp queue to UDP peer
 */
void UdpRelayServer::deliver_to_udp_peer()
{
	if(true == m_spSessionManager->tcp_queue_empty())
	   {
		   return;
	   }
	   RouterPacket& packet = m_spSessionManager->peek_tcp_packet();
	   char szcPeerData[128] = "";
	   char szcPeerAdr[32] = "";
	   char szcPeerPort[8] = "";
	   bool res = packet.get_peer_data(szcPeerData, 128);
	   if(false == res)
	   {
  	 		log_error(rart_category, "Packet udp peer address error, erase pop element in the queue.");
			m_spSessionManager->pop_tcp_packet();
			return;
	   }
	   split_addr (szcPeerData, szcPeerAdr, szcPeerPort);
	   udp::endpoint udpPeer;
	   udpPeer.address(ip::address::from_string(szcPeerAdr));
	   udpPeer.port(atoi(szcPeerPort));

	   char* pcPacketData = packet.get_packet_data();
	   boost::uint32_t nDataSize = packet.packet_data_length();
	   log_info(rart_category, "Message to the UDP peer [%s:%s] : %s", szcPeerAdr, szcPeerPort, pcPacketData);
	   //TRACE(RART_LOG_DEBUG, "Message to UDP peer : [" << szcPeerAdr << ":" << szcPeerPort << " " << makeHex(pcPacketData, nDataSize) << "]");

	   m_socket.async_send_to(
	             boost::asio::buffer(pcPacketData, nDataSize), udpPeer,
	             boost::bind(&UdpRelayServer::handle_deliver_to_udp_peer, this,
	             boost::asio::placeholders::error,
	               boost::asio::placeholders::bytes_transferred));
}


}
