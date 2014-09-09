/*
 * util.cpp
 *
 *  Created on: Oct 24, 2010
 *  Author: vassilux
 */
#include <cstdlib>
#include <string>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#ifndef WIN32
#include <syslog.h>

#include <signal.h>
#endif 



std::string makeHex(const char* pcBuffer, size_t nSize)
{
	std::stringstream ss;
	try
	{
		/*ss.setf(std::ios_base::hex, std::ios::basefield);
		ss.setf(std::ios_base::uppercase);
		ss.fill('0');

		for (size_t i = 0; i < nSize; ++i)
		{
			ss << std::hex << std::setw(2)
				<< (unsigned short) (unsigned char) pcBuffer[i];
		}*/
		ss << pcBuffer;
	}
	catch (std::exception& e)
	{
		 
	}
	
	
	
	return ss.str();
}

