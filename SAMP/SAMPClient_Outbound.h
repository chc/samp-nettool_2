#ifndef SAMPCLIENT_OUTBOUND_H
#define SAMPCLIENT_OUTBOUND_H
#include "SAMPPacket.h"
namespace SAMP {
	class SAMPOutboundClientHandler;
	typedef struct _SAMPOutboundClientMsgHandlers {
			PacketEnumeration id;
			ESAMPAuthState required_state;
			void (SAMPOutboundClientHandler::*mpHandler)(RakNet::BitStream *, PacketEnumeration);
	} SAMPOutboundClientMsgHandler;
	class SAMPOutboundClientHandler : public SAMPPacketHandler {
	public:
		SAMPOutboundClientHandler(SAMPPacketHandlerSendFunc send_func, SAMPPacketHandlerRecvFunc recv_func, SAMP::Client *client, const struct sockaddr_in *in_addr);
		~SAMPOutboundClientHandler();
		void handle_bitstream(RakNet::BitStream *stream);

		void tick(fd_set *set);
	private:
		void handle_nonrak_packet(RakNet::BitStream *stream);
		void handle_raknet_packet(RakNet::BitStream *stream);
		void process_racket_sequence(RakNetByteSeq &byte_seq);
		//initial connection packets
		void send_cookie_request();
		void send_open_conn_reply();
		
		bool m_raknet_mode;

		bool m_sent_cookie_packet;
		uint16_t m_server_cookie;

		SAMP::Client *mp_client;

		SAMPPacketHandlerSendFunc mp_send_func;
		SAMPPacketHandlerRecvFunc mp_recv_func;

		static SAMPOutboundClientMsgHandler m_msg_handlers[];
		
		void m_handle_conn_req(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_auth_key(RakNet::BitStream *data, PacketEnumeration id);
		void m_new_incoming_conn(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_rpc(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_sync(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_internal_ping(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_detect_lost_connections(RakNet::BitStream *data, PacketEnumeration id);
		void m_handle_disconnect(RakNet::BitStream *data, PacketEnumeration id);

		int m_player_id;
		struct sockaddr_in m_client_addr;
	};
}
#endif //SAMPCLIENT_INBOUND_H