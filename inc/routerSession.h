/*
 * session.h
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "common.h"
#include "adrTuple.h"

namespace rart
{

class SessionManager;

/**
 * Router packet
 * Format the packet
 * <br>4 bytes : size of payload ( udp peer address length + 4 bytes (length of udp  peer address) + size of raw data
 * <br>4 bytes : size of addresse
 * <br>Follow by applicaiton data
 */
class RouterPacket
{
public:
	enum
	{
		HEADER_LENGTH = 4, PEER_LENGTH = 4
	};
	enum
	{
		MAX_PAYLOAD_SIZE = 1024
	};
	boost::uint32_t m_payloadLength;
	boost::uint32_t m_peerLength;
	char m_payload[HEADER_LENGTH + PEER_LENGTH + MAX_PAYLOAD_SIZE];

	RouterPacket() :
		m_payloadLength(0), m_peerLength(0)
	{
		clear();

	}

	RouterPacket(const RouterPacket& other)
	{
		copy(other);
	}

	RouterPacket& operator=(const RouterPacket& other)
	{
		copy(other);

	}
	void copy(const RouterPacket& other)
	{
		m_payloadLength = other.m_payloadLength;
		m_peerLength = other.m_peerLength;
		if (m_payloadLength > 0)
		{
			memcpy(m_payload, other.m_payload, HEADER_LENGTH + PEER_LENGTH
					+ m_payloadLength);
		}

	}
	const char* data() const
	{
		return m_payload;
	}

	char* data()
	{
		return m_payload;
	}

	boost::uint32_t data_length()
	{
		return HEADER_LENGTH + m_payloadLength;
	}

	char* payload()
	{
		return m_payload + HEADER_LENGTH;
	}

	void payload_length(boost::uint32_t payLoadLength)
	{
		m_payloadLength = payLoadLength + PEER_LENGTH;
	}

	boost::uint32_t payload_length()
	{
		return m_payloadLength;
	}

	void peer_length(boost::uint32_t peerLength)
	{
		m_peerLength = peerLength;
	}

	boost::uint32_t peer_length()
	{
		return m_peerLength;
	}

	boost::uint32_t packet_data_length()
	{
		return m_payloadLength - m_peerLength - PEER_LENGTH;
	}

	bool decode_header()
	{
		using namespace std;
		char header[HEADER_LENGTH + 1] = "";
		strncat(header, m_payload, HEADER_LENGTH);
		m_payloadLength = atoi(header);
		if (m_payloadLength > MAX_PAYLOAD_SIZE)
		{
			m_payloadLength = 0;
			return false;
		}

		return true;
	}

	bool decode_peer()
	{
		char peer[PEER_LENGTH + 1] = "";
		strncat(peer, m_payload + HEADER_LENGTH, PEER_LENGTH);
		m_peerLength = atoi(peer);
		if (m_peerLength > m_payloadLength)
		{
			m_peerLength = 0;
			return false;
		}
		return true;
	}

	void encode_header()
	{
		using namespace std;
		char header[HEADER_LENGTH + 1] = "";
		sprintf(header, "%4d", m_payloadLength);
		memcpy(m_payload, header, HEADER_LENGTH);
		char peer[PEER_LENGTH + 1] = "";
		sprintf(peer, "%4d", m_peerLength);
		memcpy(m_payload + HEADER_LENGTH, peer, PEER_LENGTH);
	}

	void clear()
	{
		m_payloadLength = 0;
		m_peerLength = 0;
		memset(m_payload, 0, (HEADER_LENGTH + PEER_LENGTH + MAX_PAYLOAD_SIZE)
				* sizeof(char));
	}

	bool get_peer_data(char* peer, boost::uint32_t size)
	{
		if (m_peerLength > 0 && size > m_peerLength)
		{
			strncat(peer, m_payload + HEADER_LENGTH + PEER_LENGTH, m_peerLength);
			return true;
		}

		return false;
	}

	bool set_peer_data(const char* peer, boost::uint32_t size)
	{
		m_peerLength = size;
		memcpy(m_payload + HEADER_LENGTH + PEER_LENGTH, peer, m_peerLength);
		return true;
	}

	bool get_packet_data(char* packet, boost::uint32_t size)
	{
		m_payloadLength = size + m_peerLength;
		memcpy(packet, m_payload + HEADER_LENGTH + PEER_LENGTH + m_peerLength,
				m_payloadLength - m_peerLength);
		return true;

	}

	char* get_packet_data()
	{
		return m_payload + HEADER_LENGTH + PEER_LENGTH + m_peerLength;
	}

	bool set_packet_data(const char* packet, boost::uint32_t size)
	{
		//if ((m_payloadLength - m_peerLength) <= size)
		//{
		m_payloadLength = size + m_peerLength + PEER_LENGTH;
		memcpy(m_payload + HEADER_LENGTH + PEER_LENGTH + m_peerLength, packet,
				size);
		return true;
		//}
		//return false;
	}

};

typedef std::deque<RouterPacket> PacketQueue;

/**
 * Router session
 * <br>Between an udp peer (synchronic transmitter) and TCP server(F1 server application
 */
class RouterSession: public boost::enable_shared_from_this<RouterSession>
{
protected:
	boost::asio::io_service& m_ioservice;
	ip::tcp::resolver m_resolver;
	//bridge beetwen UDP server( recever ) and tcp session management
	SessionManager* m_pSessionManager;
	// client socket to TCP server
	ip::tcp::socket m_socket;
	//addresse of udp peer
	AdrTuple m_udpTuple;
	//temp packets
	RouterPacket m_fromTcpPacket;
	//packets queues for TCP peer (F1 server applicaiton)
	PacketQueue m_udpPacketQueue;

	bool m_bConnecting;
	//bool m_readProcess;
protected:
	void start_tcp_connection(ip::tcp::resolver::iterator endpoint_iter);
	void close_session();
public:
	explicit RouterSession(boost::asio::io_service& ioservice,
			SessionManager* pSessionManager);
	virtual ~RouterSession();
	void start();
	void handle_connection(const boost::system::error_code& error,
			ip::tcp::resolver::iterator endpoint_iterator);
#if 0
	void handle_recv_tcp(const boost::system::error_code& error,
			size_t last_length);
#endif

	void handle_deliver_tcp(const boost::system::error_code& error);

	void handle_resolve(const boost::system::error_code& error,
			ip::tcp::resolver::iterator endpoint_iterator);

	AdrTuple& get_adr_tuple()
	{
		return m_udpTuple;
	}
	void set_adr_tuple(const AdrTuple& adrTuple)
	{
		m_udpTuple = adrTuple;
	}

	void add_router_packet(RouterPacket& packet);

	void set_udp_buffer(const NetworkBuffer& udpBuffer, boost::uint32_t nSize);

	void set_udp_message(const NetworkBuffer& udpBuffer, boost::uint32_t nSize);

	void deliver_to_tcp_peer();

	//tcp side
	ip::tcp::socket& tcp_socket()
	{
		return m_socket;
	}

	void start_read_from_tcp();
	void handle_read_header(const boost::system::error_code& error,
			size_t nBytesLength);
	void handle_read_payload(const boost::system::error_code& error,
			size_t nBytesLength);

	PacketQueue& get_udp_queue()
	{
		return m_udpPacketQueue;
	}

	

};

typedef boost::shared_ptr<RouterSession> RouterSessionPtr;
typedef boost::weak_ptr<RouterSession> RouterSessionWptr;

}

#endif /* SESSION_H_ */
