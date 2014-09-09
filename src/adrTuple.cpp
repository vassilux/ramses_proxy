/*
 * adrTuple.cpp
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */

#include "../inc/adrTuple.h"

namespace rart
{

AdrTuple::AdrTuple() :
	m_transport(None), m_nPort(0)
{

}

AdrTuple::~AdrTuple()
{

}

AdrTuple::AdrTuple(TransportType transport, const asio::ip::address& adr,
		unsigned int nPort) :
	m_transport(transport), m_adr(adr), m_nPort(nPort)
{

}

AdrTuple::AdrTuple(const AdrTuple& other)
{
	Copy(other);
}

AdrTuple& AdrTuple::operator=(const AdrTuple& other)
{
	Copy(other);
	return (*this);
}

void AdrTuple::Copy(const AdrTuple& other)
{
	m_transport = other.m_transport;
	m_adr = other.m_adr;
	m_nPort = other.m_nPort;
}

bool AdrTuple::operator==(const AdrTuple& other) const
{
	return m_transport == other.m_transport && m_adr == other.m_adr && m_nPort
			== other.m_nPort;
}

bool AdrTuple::operator!=(const AdrTuple& other) const
{
	return m_transport != other.m_transport || m_adr != other.m_adr || m_nPort
			!= other.m_nPort;
}

bool AdrTuple::operator<(const AdrTuple& other) const
{
	if (m_transport < other.m_transport)
	{
		return true;
	}
	if (m_transport > other.m_transport)
	{
		return false;
	}
	if (m_adr < other.m_adr)
	{
		return true;
	}
	if (m_adr == other.m_adr)
	{
		return m_nPort < other.m_nPort;
	}
	return false;
}

std::string AdrTuple::to_string()
{
	std::stringstream ss;
	ss << this->getAddress() << ":" << this->getPort();
	return ss.str();
}

std::ostream& operator<<(std::ostream& strm, const AdrTuple& tuple)
{
	switch (tuple.m_transport)
	{
	case AdrTuple::None:
		strm << "[None ";
		break;
	case AdrTuple::UDP:
		strm << "[UDP ";
		break;
	case AdrTuple::TCP:
		strm << "[TCP ";
		break;
	default :
		strm << "[UNKNOW ";
		break;
	}
	strm << tuple.getAddress() << ":" << tuple.getPort() << "]";
	return strm;
}

}
