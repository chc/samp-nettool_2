#ifndef _SAMPCLIENT_H
#define _SAMPCLIENT_H
#include <stdint.h>
#include <NetClient.h>
#include <BitStream.h>

#include "SAMPPacket.h"
namespace SAMP {
	class Client : public Net::Client {
	public:
		Client(int sd, const struct sockaddr_in *in_addr, SAMP::Server *server, uint16_t port);
		Client(const char *hostname);
		~Client();
		void think(fd_set *set);
		void handle_packet(const char *buf, int len);
		static void client_send_func(RakNet::BitStream &bs, void *extra, bool encrypt);
		static void client_recv_func(RakNet::BitStream &out, void *extra, bool decrypt);
		SAMP::Server *GetServer() { return mp_server; };

		void SendRPC(int rpcid, RakNet::BitStream *rpc_data);
		void SendMessage(int rpcid, RakNet::BitStream *rpc_data);

		bool GetInbound() { return m_inbound; };
	private:
		SAMP::Server *mp_server;
		uint16_t m_server_port;
		SAMPPacketHandler *mp_packet_handler;

		bool m_inbound;
		//SAMPConnection *mp_connection;
	};
}
#endif //_SAMPCLIENT_H