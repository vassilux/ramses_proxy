/*
 * paramsManager.h
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */

#ifndef PARAMSMANAGER_H_
#define PARAMSMANAGER_H_

#include "common.h"

namespace rart
{


class ParamsManager : public boost::enable_shared_from_this< ParamsManager >
{
protected:
	//
	std::string m_sF1Adr;
	size_t      m_nF1Port;
	//
	size_t		m_nUDpPort;
	//
	size_t		m_nTcpPort;
	long		m_nSessionTimeout;

	//
	int			m_nLogLevel;

	std::string m_sConfigPath;


public:
	ParamsManager(const char* pcConfigPath);
	virtual ~ParamsManager();

	int load();
	int save();


	 void setSessionTimeout(long nSessionTimeout)
	 {
	        m_nSessionTimeout = nSessionTimeout;
	 }

    long getSessionTimeout() const
    {
        return m_nSessionTimeout;
    }

    size_t getF1Port() const
    {
        return m_nF1Port;
    }

    size_t getUDpPort() const
    {
        return m_nUDpPort;
    }

    std::string getF1ServerAdr() const
    {
        return m_sF1Adr;
    }



    void setF1Port(size_t nTCPPort)
    {
        m_nF1Port = nTCPPort;
    }

    void setUDpPort(size_t nUDpPort)
    {
        m_nUDpPort = nUDpPort;
    }

    void setF1ServerAdr(const std::string& sTCPServerAdr)
    {
        m_sF1Adr = sTCPServerAdr;
    }

    int getLogLevel()
    {
    	return m_nLogLevel;
    }

    size_t getTcpPort()
    {
    	return m_nTcpPort;
    }

    void setTcpPort(size_t nTcpPort)
    {
    	m_nTcpPort = nTcpPort;
    }

};


typedef boost::shared_ptr<ParamsManager> ParamsManagerPtr;
typedef boost::weak_ptr<ParamsManager> ParamsManagerWPtr;


}

#endif /* PARAMSMANAGER_H_ */
