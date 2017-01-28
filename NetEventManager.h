#ifndef _NETEVENTMGR_H
#define _NETEVENTMGR_H
#include "NetServer.h"
#include "NetClient.h"
#include "NetProxy.h"
#include <vector>
namespace Net {
	class IEventManager {
		public:
			IEventManager();
			~IEventManager();
			virtual void run() = 0;
			void addNetworkServer(Net::Server *driver);
			void addNetworkClient(Net::Client *driver);
			void addNetworkProxy(Net::Proxy *driver);
		protected:
			std::vector<Net::Server *> m_net_servers;
			std::vector<Net::Client *> m_net_clients;
			std::vector<Net::Proxy *> m_net_proxies;
	};
}
#endif //_NETEVENTMGR_H