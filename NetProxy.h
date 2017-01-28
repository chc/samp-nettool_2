#ifndef _NETPROXY_H
#define _NETPROXY_H
#include "main.h"
#include "NetServer.h"
#include "NetClient.h"
namespace Net {
	class SelectNetEventManager;
	class Proxy {
	public:
		Proxy(Server *server); //localhost:7777
		~Proxy();
		//void SetClientFactory(ClientFactoryFunc func);
		void think(); //called each time for recv/send
		void run(fd_set *set);
		int setup_fdset(fd_set *set); //for select event manager
	private:
		//ClientFactoryFunc mp_client_factory;
		SelectNetEventManager *mp_event_mgr;
		Server *mp_proxy_server;
	};
}
#endif //_NETPROXY_H