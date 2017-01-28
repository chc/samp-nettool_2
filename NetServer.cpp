#include "NetServer.h"
namespace Net {
	Server::Server(const char *bind_hostname) {
	}
	Server::~Server() {
	}
	int Server::getListenerSocket() {
		return m_server_socket;
	}
	int Server::setup_fdset(fd_set *set) {
		FD_SET(m_server_socket, set);

		return m_server_socket+1; 
	}
}
