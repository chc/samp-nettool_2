#ifndef SAMPCLIENT_INBOUND_H
#define SAMPCLIENT_INBOUND_H
#include "SAMPPacket.h"
#define SAMP_SEND_PING_TIME 15
namespace SAMP {
	class SAMPInboundClientHandler;
	typedef struct _SAMPClientMsgHandlers {
			PacketEnumeration id;
			ESAMPAuthState required_state;
			void (SAMPInboundClientHandler::*mpHandler)(RakNet::BitStream *, PacketEnumeration id);
	} SAMPClientMsgHandler;
	class SAMPInboundClientHandler : public SAMPPacketHandler {
	public:
		SAMPInboundClientHandler(SAMPPacketHandlerSendFunc func, SAMP::Client *client, const struct sockaddr_in *in_addr);
		~SAMPInboundClientHandler();
		void handle_bitstream(RakNet::BitStream *stream);

		void tick(fd_set *set);
	private:
		void send_ping();
		struct timeval m_last_sent_ping;

		void handle_nonrak_packet(RakNet::BitStream *stream);
		void handle_raknet_packet(RakNet::BitStream *stream);
		void process_racket_sequence(RakNetByteSeq &byte_seq);
		//initial connection packets
		void send_cookie_request();
		void send_open_conn_reply();
		
		bool m_raknet_mode;

		bool m_sent_cookie_packet;
		uint16_t m_server_cookie;

		std::string m_password; //pw user sent to try join

		SAMP::Client *mp_client;

		SAMPPacketHandlerSendFunc mp_send_func;

		static SAMPClientMsgHandler m_msg_handlers[];



		void m_handle_conn_req(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_auth_key(RakNet::BitStream *data, PacketEnumeration id);
		void m_new_incoming_conn(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_rpc(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_internal_ping(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_sync(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_stats_update(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_detect_lost_connections(RakNet::BitStream *data, PacketEnumeration id);
		void m_connected_pong(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_disconnect(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_recv_static_data(RakNet::BitStream *data, PacketEnumeration id);
		
	};
}
#endif //SAMPCLIENT_INBOUND_H