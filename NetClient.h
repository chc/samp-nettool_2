#ifndef _NETCLIENT_H
#define _NETCLIENT_H
#include <main.h>
namespace Net {
	class Client {
	public:
		Client(const struct sockaddr_in *in_addr, int sd);
		Client();
		~Client();
		virtual void think(fd_set *set) = 0; //called each time for recv/send
		virtual void handle_packet(const char *buf, int len) = 0;
		struct sockaddr_in GetSockAddr() {return m_sock_addr; };
		int GetSocket() { return m_sd; };
	protected:
		struct sockaddr_in m_sock_addr;
		int m_sd;
	};
}
#endif //_NETCLIENT_H