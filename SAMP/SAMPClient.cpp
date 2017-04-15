#include "SAMPClient.h"
#include "SAMPServer.h"
#include "SAMPClient_Inbound.h"
#include "SAMPClient_Outbound.h"
#include "encryption.h"
#include "BitStream.h"

#include "python\PySAMPRPC.h"
namespace SAMP {

	Client::Client(int sd, const struct sockaddr_in *in_addr, SAMP::Server *server, uint16_t port) : Net::Client(in_addr, sd) {
		mp_server = server;
		m_server_port = port;
		m_sd = sd;
		mp_packet_handler = new SAMPInboundClientHandler(client_send_func, this, in_addr);

		m_inbound = true;
	}
	Client::Client(const char *hostname) :  Net::Client() {
		char address[256];
		uint16_t port;
		get_server_address_port(hostname, (char *)&address, port);

		memset(&m_sock_addr,0,sizeof(m_sock_addr));
		m_sock_addr.sin_family=AF_INET;  
		m_sock_addr.sin_addr.s_addr=resolv(address);  
		m_sock_addr.sin_port=htons(port);

		m_inbound = false;

		m_sd = socket(AF_INET, SOCK_DGRAM, 0);

		mp_packet_handler = new SAMPOutboundClientHandler(client_send_func, client_recv_func, this, &m_sock_addr);
	}
	Client::~Client() {
		if(!m_inbound) {
			closesocket(m_sd);
		}
	}
	void Client::think(fd_set *set) {
		//send msg queue
		mp_packet_handler->tick(set);
	}
	void Client::client_send_func(RakNet::BitStream &bs, void *extra, bool encrypt) {
		
		Client *client = (Client *)extra;
		assert(bs.GetNumberOfBytesUsed() < MTUSize);
		char sendbuf[MTUSize];
		int buflen = bs.GetNumberOfBytesUsed();
		struct sockaddr_in addr = client->GetSockAddr();

		assert(bs.GetNumberOfBytesUsed() < MTUSize);

		bs.ResetReadPointer();
		bs.ReadBits((unsigned char *)&sendbuf, bs.GetNumberOfBitsUsed());

		if(encrypt) {
			sampEncrypt((uint8_t *)&sendbuf, buflen, htons(addr.sin_port), 0);
			buflen++;
		}

		
		int r = sendto(client->m_sd,(char *)&sendbuf,buflen,0,(struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		assert(r > 0);
	}
	void Client::client_recv_func(RakNet::BitStream &out, void *extra, bool decrypt) {
		Client *client = (Client *)extra;
		char recvbuf[MTUSize];
		const struct sockaddr_in addr = client->GetSockAddr();
		int from_len = sizeof(struct sockaddr_in);
		int len = recvfrom(client->GetSocket(), (char *)&recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&addr, &from_len);
		assert(len > 0);
		if(decrypt) {
			sampDecrypt((uint8_t*)recvbuf, len, htons(addr.sin_port), 0);
			len--;
		}
		out.Write(recvbuf, len);
	}
	void Client::handle_packet(const char *buf, int len) {
		if(len <= 0) {
			//should flag delete
			return;
		}
		if(m_inbound) {
			sampDecrypt((uint8_t*)buf, len, m_server_port, 0);
			len--;
		}
		RakNet::BitStream is((unsigned char *)buf, len, false);
		mp_packet_handler->handle_bitstream(&is);
	}
	void Client::SendRPC(int rpcid, RakNet::BitStream *rpc_data) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_RPC);
		bs.Write((uint8_t)rpcid);

		bs.WriteCompressed((uint32_t)rpc_data->GetNumberOfBitsUsed());
		bs.Write(rpc_data);

		/*
		const char *name = GetRPCNameByID(rpcid);
		if(this->GetInbound()) {
			printf("Send RPC %d(%s) inbound (%d|%d)\n",rpcid, name, rpc_data->GetNumberOfBitsUsed(),rpc_data->GetNumberOfBytesUsed());
		} else {
			printf("Send RPC %d(%s) outbound (%d|%d)\n",rpcid, name, rpc_data->GetNumberOfBitsUsed(),rpc_data->GetNumberOfBytesUsed());
		}
		*/
		
		
		mp_packet_handler->AddToOutputStream(&bs, UNRELIABLE_SEQUENCED, SAMP::MEDIUM_PRIORITY);
	}
	void Client::SendMessage(int msgid, RakNet::BitStream *rpc_data) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)msgid);
		bs.Write(rpc_data);
		mp_packet_handler->AddToOutputStream(&bs, UNRELIABLE_SEQUENCED, SAMP::MEDIUM_PRIORITY);
		
	}
}