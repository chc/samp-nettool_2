#include "SAMPServer.h"
#include "SAMPClient.h"
#include <legacy/buffwriter.h>
namespace SAMP {
	Server::Server(const char *hostname) : Net::Server(hostname) {
		uint16_t server_port;
		uint32_t server_ip;
		char extracted_hostname[128];
		get_server_address_port(hostname, (char *)&extracted_hostname, server_port);
		server_ip = resolv(extracted_hostname);
		m_server_addr.sin_family=AF_INET;  
		m_server_addr.sin_addr.s_addr=(server_ip);  
		m_server_addr.sin_port=htons(server_port);
		m_server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		bind(m_server_socket, (struct sockaddr *)&m_server_addr, sizeof m_server_addr);

		QueryDetails.name = "Proxy Server";
		QueryDetails.mode = "nettool 0.2";
		QueryDetails.map = "nettool";
		QueryDetails.max_players = 5;
		QueryDetails.num_players = 0;
		m_server_password = "123321";
	}
	Server::~Server() {
	}
	void Server::think(fd_set *set) {
		char buf[MTUSize];
		if(set) {
			if(FD_ISSET(m_server_socket,set)) {
				struct sockaddr_in from_addr;
				socklen_t from_len = sizeof(from_addr);
				int *magic = (int *)&buf;
				int len = recvfrom(m_server_socket, (char *)&buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &from_len);
				if(*magic == SAMP_MAGIC) {
					handle_samp_query((char *)&buf, len, &from_addr);
				} else {
					Client *client = find_samp_client_or_create((struct sockaddr_in *)&from_addr);
					client->handle_packet((const char *)&buf, len);
				}
			}
			std::vector<SAMP::Client *>::iterator it = m_samp_clients.begin();
			while(it != m_samp_clients.end()) {
				SAMP::Client *client = *it;
				client->think(set);
				it++;
			}
		}
}
	void Server::handle_query_info_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header) {
		char writebuff[256];

		const char *server_name = QueryDetails.name.c_str();
		const char *server_gamemode = QueryDetails.mode.c_str();
		const char *server_mapname = QueryDetails.map.c_str();

		uint32_t out_len = 0;
		uint8_t *p = (uint8_t *)&writebuff;

		BufferWriteData(&p, &out_len, (uint8_t *)header, sizeof(SAMPQueryHeader)-1);
		BufferWriteByte(&p,&out_len, m_server_password.size() > 0); //password
		BufferWriteShort(&p,&out_len, QueryDetails.num_players); //num players
		BufferWriteShort(&p,&out_len, QueryDetails.max_players); //max players

		BufferWriteInt(&p, &out_len, QueryDetails.name.size());
		BufferWriteData(&p, &out_len, (uint8_t *)QueryDetails.name.c_str(), QueryDetails.name.size());

		BufferWriteInt(&p, &out_len, QueryDetails.name.size());
		BufferWriteData(&p, &out_len, (uint8_t *)QueryDetails.name.c_str(), QueryDetails.name.size());

		BufferWriteInt(&p, &out_len, QueryDetails.map.size());
		BufferWriteData(&p, &out_len, (uint8_t *)QueryDetails.map.c_str(), QueryDetails.map.size());

		sendto(m_server_socket, (char *)&writebuff, out_len, 0, (struct sockaddr *)from, sizeof(struct sockaddr_in));
	}
	void Server::handle_query_ping_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header, SAMPPing *ping) {
		char writebuff[256];
		uint32_t out_len = 0;

		uint8_t *p = (uint8_t *)&writebuff;

		BufferWriteData(&p, &out_len, (uint8_t *)header, sizeof(SAMPQueryHeader)-1);
		BufferWriteInt(&p, &out_len, ping->ping);
		int slen = sizeof(struct sockaddr_in);

		sendto(m_server_socket,(char *)&writebuff,out_len,0,(struct sockaddr *)from, slen);
	}
	void Server::handle_query_clients_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header, bool detailed) {
			char writebuff[10000];
			uint32_t olen = 0;

			uint8_t *p = (uint8_t *)&writebuff;
			BufferWriteData(&p, &olen, (uint8_t *)header, sizeof(SAMPQueryHeader)-1);


			BufferWriteShort(&p, &olen, QueryDetails.num_players);


			/*
			std::vector<SAMPRakPeer *>::iterator it = m_connections.begin();
			while(it != m_connections.end()) {
				SAMPRakPeer *user = *it;
				SAMPPlayer *player = user->GetPlayer();
				if(detailed)
					BufferWriteByte(&p, &len, player->GetPlayerID()); //write player id

				BufferWriteByte(&p, &len, strlen(player->GetName()));

				BufferWriteData(&p, &len, (uint8_t*)player->GetName(), strlen(player->GetName()));

				BufferWriteInt(&p, &len, 0);

				if(detailed) {
					BufferWriteInt(&p,&len,user->GetPing());
				}
				it++;
			}*/


			int slen = sizeof(struct sockaddr_in);
			sendto(m_server_socket,(char *)&writebuff,olen,0,(struct sockaddr *)from, slen);
	}
	void Server::handle_query_rules_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header) {
		char writebuff[256];

		uint32_t out_len = 0;

		uint8_t *p = (uint8_t *)&writebuff;

		BufferWriteData(&p, &out_len, (uint8_t *)header, sizeof(SAMPQueryHeader)-1);
	
		BufferWriteShort(&p,&out_len, 0);//rule_list.size());

		/*
		std::vector<struct RuleInfo>::iterator it = rule_list.begin();
		while(it != rule_list.end()) {
			struct RuleInfo rule = *it;
			BufferWriteByte(&p,&len, strlen(rule.name));
			BufferWriteData(&p, &len, (uint8_t*)rule.name, strlen(rule.name));

			BufferWriteByte(&p,&len, strlen(rule.value));
			BufferWriteData(&p, &len, (uint8_t*)rule.value, strlen(rule.value));
			it++;
		}
		*/

		int slen = sizeof(struct sockaddr_in);
		sendto(m_server_socket,(char *)&writebuff,out_len,0,(struct sockaddr *)from, slen);
	}
	void Server::handle_samp_query(char *buf, int len, struct sockaddr_in *from) {
		SAMPQueryHeader *header = (SAMPQueryHeader *)buf;
	

		SAMPPing *ping = (SAMPPing *)(buf+sizeof(SAMPQueryHeader)-1);
		switch(header->opcode) {
			case SAMP_PACKET_INFO:
				handle_query_info_packet(buf, len, from, header);
				break;
			case SAMP_PACKET_PING:		
				handle_query_ping_packet(buf, len, from, header, ping);
				break;
			case SAMP_PACKET_CLIENTS:
				handle_query_clients_packet(buf, len, from, header, false);
				break;
			case SAMP_PACKET_CLIENTS_DETAILED:
				handle_query_clients_packet(buf, len, from, header, true);
				break;
			case SAMP_PACKET_RULES:
				handle_query_rules_packet(buf, len, from, header);
				break;
		}
	}
	SAMP::Client *Server::find_samp_client_or_create(struct sockaddr_in *addr) {

		SAMP::Client *client = NULL;
		std::vector<SAMP::Client *>::iterator it = m_samp_clients.begin();
		while(it != m_samp_clients.end()) {
			client = *it;
			#ifdef _WIN32
			if(client->GetSockAddr().sin_addr.S_un.S_addr == addr->sin_addr.S_un.S_addr && addr->sin_port == client->GetSockAddr().sin_port)
			#else
			if(client->GetSockAddr().sin_addr.s_addr == addr->sin_addr.s_addr && addr->sin_port == client->GetSockAddr().sin_port)
			#endif
			{
				return client;
			}
			it++;
		}
		client = new SAMP::Client(m_server_socket, addr, this, htons(m_server_addr.sin_port));
		m_samp_clients.push_back(client);
		return client;	
	}
}