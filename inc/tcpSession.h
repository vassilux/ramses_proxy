/*
 * tcpSession.h
 *
 *  Created on: Nov 28, 2010
 *      Author: vassilux
 */

#ifndef TCPSESSION_H_
#define TCPSESSION_H_

#include "common.h"
#include "adrTuple.h"
#include "routerSession.h"

namespace rart
{



class TcpSession //: public RouterSession//,
			//public boost::enable_shared_from_this< TcpSession >
{
private:

protected:
	//boost::asio::io_service& m_ioservice;
	//SessionManager* m_pSessionManager;
	RouterPacket m_tcpPacket;

public:
	explicit TcpSession(boost::asio::io_service& ioservice,
			SessionManager* pSessionManager);
	virtual ~TcpSession();

	void start();
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_payload(const boost::system::error_code& error);

};

typedef boost::shared_ptr<TcpSession> TcpSessionPtr;
typedef boost::weak_ptr<TcpSession>   TcpSessionWptr;


};

#endif /* TCPSESSION_H_ */
