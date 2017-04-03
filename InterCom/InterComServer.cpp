#include "InterComServer.h"
namespace InterCom {
		Server::Server(const char *hostname) : Net::Server(hostname) {
			uint16_t server_port;
			uint32_t server_ip;
			char extracted_hostname[128];
			get_server_address_port(hostname, (char *)&extracted_hostname, server_port);
			server_ip = resolv(extracted_hostname);
			m_server_addr.sin_family=AF_INET;  
			m_server_addr.sin_addr.s_addr=(server_ip);  
			m_server_addr.sin_port=htons(server_port);
			m_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			bind(m_server_socket, (struct sockaddr *)&m_server_addr, sizeof m_server_addr);

			listen(m_server_addr, SOMAXCONN);
		}
		Server::~Server() {
			
		}
		void Server::think(fd_set *set) {
			
		}
}