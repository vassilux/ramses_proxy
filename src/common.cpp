/*
 * common.cpp
 *
 *  Created on: Oct 24, 2010
 *  Author: vassilux
 */
#include "common.h"


SyslogTemporary SysLog(int iPriority)
{
	return SyslogTemporary(iPriority);
}

void split_addr (char *ip_port, char *ip, char *port)
{
    char *p;
    p = strtok (ip_port, ":");
    strcpy (ip, p);
    p = strtok (NULL, "");
    if (p) strcpy (port, p);
    else   *port = 0;
}

