/*
 * adrTuple.h
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */

#ifndef ADRTUPLE_H_
#define ADRTUPLE_H_
#include "common.h"

namespace rart
{

class AdrTuple
{
public:
	typedef enum
	{
			None, UDP, TCP
	} TransportType;

private:
protected:
	TransportType m_transport;
	boost::asio::ip::address m_adr;
	unsigned int m_nPort;

	void Copy(const AdrTuple& other);
public:

	explicit AdrTuple();
	explicit AdrTuple(TransportType transport, const boost::asio::ip::address& adr, unsigned int nPort);
	AdrTuple(const AdrTuple& other);

	virtual ~AdrTuple();

	AdrTuple& operator=(const AdrTuple& other);

	bool operator==(const AdrTuple& other) const;
	bool operator!=(const AdrTuple& other) const;
	bool operator<(const AdrTuple& other) const;

	TransportType getTransportType() const { return m_transport; }
	void setTransportType(TransportType transport) { m_transport = transport; }

	const asio::ip::address& getAddress() const { return m_adr; }
	void setAddress(const asio::ip::address& adr) { m_adr = adr; }

	unsigned int getPort() const { return m_nPort; }
	void setPort(unsigned int nPort) { m_nPort = nPort; }

	std::string to_string();

	friend std::ostream& operator<<(std::ostream& strm, const AdrTuple& tuple);
};
std::ostream& operator<<(std::ostream& strm, const AdrTuple& tuple);
}

#endif /* ADRTUPLE_H_ */
