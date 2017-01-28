#ifndef _NETSERVER_H
#define _NETSERVER_H
#include "main.h"
#include "NetClient.h"
namespace Net {
	class Server {
	public:
		Server(const char *bind_hostname); //localhost:7777
		~Server();
		virtual void think(fd_set *set) = 0; //called each time for recv/send
		int getListenerSocket();
		int setup_fdset(fd_set *set); //for select event manager
	protected:
		struct sockaddr_in m_server_addr;
		int m_server_socket;
	};
}
#endif //_NETSERVER_H