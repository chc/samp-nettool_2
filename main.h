#ifndef _MAIN_H
#define _MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
	#include <WinSock2.h>
	typedef int socklen_t;
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <signal.h>
	#include <netdb.h> 
	#include <sys/time.h>
#endif

namespace SAMP {
	class Server;
};

void addSAMPServer(SAMP::Server *server);

uint32_t resolv(const char *host);
void get_server_address_port(const char *input, char *address, uint16_t &port);
#ifdef _WIN32
	int gettimeofday(struct timeval * tp, struct timezone * tzp);
#endif

#endif //_MAIN_H