/*
 * paramsManager.cpp
 *
 *  Created on: Oct 23, 2010
 *  Author: vassilux
 */
#include <fstream>

#include "paramsManager.h"
///ERMO/param/

#ifndef WIN32
#define RART_CONFIG_FILE_PATH "raRt.cfg"
#else
#define RART_CONFIG_FILE_PATH "raRt.cfg"
#endif

namespace rart
{

ParamsManager::ParamsManager(const char* pcConfigPath) :
	m_sConfigPath(pcConfigPath)
{
	m_sF1Adr = "127.0.0.1";
	m_nF1Port = 3500;
	m_nUDpPort = 3500;
	//
	m_nSessionTimeout = 30 * 60 * 1000;
	m_nLogLevel = LOG_USER;
	log_debug(rart_category, "ParamsManager::ctor");
	//TRACE(RART_LOG_DEBUG, "ParamsManager::ctor");

}

ParamsManager::~ParamsManager()
{
	//TRACE(RART_LOG_INFO, "ParamsManager::dtor");
	log_debug(rart_category, "ParamsManager::dtor");
}

int ParamsManager::save()
{
	int nRes = -1;
#if 1
	try
	{

		std::stringstream ss;
		ss << "f1serveradr=" << m_sF1Adr << std::endl;
		ss << "f1serverport=" << m_nF1Port << std::endl;
		ss << "udpport=" << m_nUDpPort << std::endl;
		ss << "tcpport=" << m_nTcpPort << std::endl;
		ss << "tcpsessiontimeout=" << m_nSessionTimeout << std::endl;
		ss << "loglevel=" << m_nLogLevel << std::endl;

		std::set<std::string> options;
		options.insert("f1serveradr");
		options.insert("f1serverport");
		options.insert("udpport");
		options.insert("tcpport");
		options.insert("tcpsessiontimeout");
		options.insert("loglevel");

		std::ofstream ofs(RART_CONFIG_FILE_PATH);
		ofs << ss.str();
		nRes = 0;
	} catch (std::exception& e)
	{
		log_error(rart_category, "I got the error [%s]", e.what());
		SYS_LOG(LOG_CRIT, "Exception : " << e.what());
	}
#endif
	return nRes;
}

int ParamsManager::load()
{
	int nRes = -1;

	m_sConfigPath += "/";
	m_sConfigPath += RART_CONFIG_FILE_PATH;
	std::ifstream config(m_sConfigPath.c_str());
	if (!config)
	{
		SYS_LOG(LOG_CRIT, "Can't find raRt.cfg file");
		log_error(rart_category, "Can't find raRt.cfg file [%s]",  m_sConfigPath.c_str());	
	}
	else
	{
		//parameters
		std::set<std::string> options;
		std::map<std::string, std::string> parameters;
		//options.insert("*");
		options.insert("f1serveradr");
		options.insert("f1serverport");
		options.insert("udpport");
		options.insert("tcpport");
		options.insert("tcpsessiontimeout");
		options.insert("loglevel");

		try
		{
			for (pod::config_file_iterator i(config, options), e; i != e; ++i)
			{
				if (i->string_key == "f1serveradr")
				{
					m_sF1Adr = i->value[0];
				}
				else if (i->string_key == "f1serverport")
				{
					m_nF1Port = atoi(i->value[0].c_str());
				}
				else if (i->string_key == "tcpsessiontimeout")
				{
					m_nSessionTimeout = atol(i->value[0].c_str());
				}
				else if (i->string_key == "loglevel")
				{
					m_nLogLevel = atoi(i->value[0].c_str());
				}
				else if (i->string_key == "udpport")
				{
					m_nUDpPort = atoi(i->value[0].c_str());
				}
				else if (i->string_key == "tcpport")
				{
					m_nTcpPort = atoi(i->value[0].c_str());
				}

			}
			nRes = 0;

		} catch (std::exception& e)
		{
			SYS_LOG(LOG_CRIT, "ParamsManager::load exception : " << e.what());
			log_error(rart_category, "ParamsManager::load exception [%s]",  e.what());
		}
	}


	return nRes;
}

}
