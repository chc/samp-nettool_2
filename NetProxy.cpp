#include "NetProxy.h"
#include "SelectNetEventManager.h"
namespace Net {
	Proxy::Proxy(Server *server) {
		mp_proxy_server = server;
		mp_event_mgr = new SelectNetEventManager();
		mp_event_mgr->addNetworkProxy(this);
	}
	Proxy::~Proxy() {
	}
	int Proxy::setup_fdset(fd_set *set) {
		return mp_proxy_server->setup_fdset(set);
	}
	void Proxy::think() {
		mp_event_mgr->run();
	}
	void Proxy::run(fd_set *set) {
		mp_proxy_server->think(set);
	}
}