#include "NetEventManager.h"
namespace Net {
	IEventManager::IEventManager() {

	}
	IEventManager::~IEventManager() {

	}
	void IEventManager::addNetworkServer(Net::Server *driver) {
		m_net_servers.push_back(driver);
	}
	void IEventManager::addNetworkProxy(Net::Proxy *driver) {
		m_net_proxies.push_back(driver);
	}
	void IEventManager::addNetworkClient(Net::Client *driver) {
		m_net_clients.push_back(driver);
	}
	
}