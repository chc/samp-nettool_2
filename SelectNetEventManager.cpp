#include "NetServer.h"
#include "NetProxy.h"
#include "SelectNetEventManager.h"
#include <stdio.h>
namespace Net {
	SelectNetEventManager::SelectNetEventManager() {

	}
	SelectNetEventManager::~SelectNetEventManager() {

	}
	void SelectNetEventManager::run() {
	
		struct timeval timeout;

		memset(&timeout,0,sizeof(struct timeval));
		timeout.tv_usec = 16000;

		int hsock = setup_fdset();
		if(select(hsock + 1, &m_fdset, NULL, NULL, &timeout) < 0) {
    		return;
		}

		std::vector<Net::Server *>::iterator it = m_net_servers.begin();
		while(it != m_net_servers.end()) {
			Net::Server *driver = *it;
			driver->think(&m_fdset);
			it++;
		}

		std::vector<Net::Proxy *>::iterator it2 = m_net_proxies.begin();
		while(it2 != m_net_proxies.end()) {
			Net::Proxy *proxy = *it2;
			proxy->run(&m_fdset);
			it2++;
		}

		std::vector<Net::Client *>::iterator it3 = m_net_clients.begin();
		while(it3 != m_net_clients.end()) {
			Net::Client *client = *it3;
			client->think(&m_fdset);
			it3++;
		}
	}
	int SelectNetEventManager::setup_fdset() {
		FD_ZERO(&m_fdset);
		int hsock = -1;
		std::vector<Net::Server *>::iterator it = m_net_servers.begin();
		while(it != m_net_servers.end()) {
			Net::Server *server = *it;

			//add listening socket/clients to fd
			int ret = server->setup_fdset(&m_fdset);
			if(ret >= hsock)
				hsock = ret+1;
			it++;
		}
		std::vector<Net::Proxy *>::iterator it2 = m_net_proxies.begin();
		while(it2 != m_net_proxies.end()) {
			Net::Proxy *server = *it2;

			//add listening socket/clients to fd
			int ret = server->setup_fdset(&m_fdset);
			if(ret >= hsock)
				hsock = ret+1;
			it2++;
		}

		std::vector<Net::Client *>::iterator it3 = m_net_clients.begin();
		while(it3 != m_net_clients.end()) {
			Net::Client *client = *it3;

			//add listening socket/clients to fd
			int ret = client->GetSocket();
			FD_SET(ret, &m_fdset);
			if(ret >= hsock)
				hsock = ret+1;
			it3++;
		}
		return hsock;
	}
}