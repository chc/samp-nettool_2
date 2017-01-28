#ifndef _MAIN_H
#define _MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
	#include <WinSock2.h>
#endif

#include "SAMP/python/Python.h"

namespace SAMP {
	class Server;
};

void addSAMPServer(SAMP::Server *server);

uint32_t resolv(const char *host);
void get_server_address_port(const char *input, char *address, uint16_t &port);
int gettimeofday(struct timeval * tp, struct timezone * tzp);
#endif //_MAIN_H