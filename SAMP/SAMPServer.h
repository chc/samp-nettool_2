#ifndef _SAMPSERVER_H
#define _SAMPSERVER_H
#include <main.h>
#include <NetServer.h>

#include <string>
#include <vector>
#include <map>

#define MTUSize 1092
#define SAMP_MAGIC 0x504d4153
namespace SAMP {
	typedef struct {
		uint32_t magic;
		uint32_t ip;
		uint16_t port;
		uint8_t opcode;
	} SAMPQueryHeader;

	typedef struct {
		uint32_t ping;
	} SAMPPing;
	
	enum {
		SAMP_PACKET_INFO = 'i',
		SAMP_PACKET_RULES = 'r',
		SAMP_PACKET_CLIENTS = 'c',
		SAMP_PACKET_CLIENTS_DETAILED = 'd',
		SAMP_PACKET_RCON = 'x',
		SAMP_PACKET_PING = 'p',
	};

	class Client;

	class Server : public Net::Server {
	public:
		Server(const char *hostname);
		~Server();
		void think(fd_set *set); //called each time for recv/send
	private:
		void handle_samp_query(char *buf, int len, struct sockaddr_in *from);
		void handle_query_info_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header);
		void handle_query_ping_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header, SAMPPing *ping);
		void handle_query_clients_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header, bool detailed);
		void handle_query_rules_packet(char *buf, int len, struct sockaddr_in *from, SAMPQueryHeader *header);
		//typedef Client *(*ClientFactoryFunc)(const char *hostname);
		struct _QueryDetails {
			std::string name;
			std::string mode;
			std::string map;
			int num_players;
			int max_players;
			std::map<std::string, std::string> server_rules;
		} QueryDetails;
		std::string m_server_password;

		Client *find_samp_client_or_create(struct sockaddr_in *from);
		std::vector<Client *> m_samp_clients;
	};
}
#endif //_SAMPSERVER_H