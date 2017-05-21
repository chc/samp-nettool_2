#ifndef _NETCLIENT_H
#define _NETCLIENT_H
#include <main.h>
namespace Net {
	class Client {
	public:
		Client(const struct sockaddr_in *in_addr, int sd);
		Client();
		virtual ~Client();
		virtual void think(fd_set *set) = 0; //called each time for recv/send
		virtual void handle_packet(const char *buf, int len) = 0;
		struct sockaddr_in GetSockAddr() {return m_sock_addr; };
		int GetSocket() { return m_sd; };
		bool ShouldDelete() { return m_delete_flag; };
		void SetDelete(bool to_delete) { m_delete_flag = to_delete; };
	protected:
		struct sockaddr_in m_sock_addr;
		int m_sd;
		bool m_delete_flag;
	};
}
#endif //_NETCLIENT_H