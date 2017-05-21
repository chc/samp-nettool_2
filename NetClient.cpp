#include "NetClient.h"
namespace Net {
	Client::Client(const struct sockaddr_in *in_addr, int socket) {
		m_sock_addr = *in_addr;
		m_sd = socket;
		m_delete_flag = false;
	}
	Client::Client() {
		m_delete_flag = false;
	}
	Client::~Client() {
	}
}