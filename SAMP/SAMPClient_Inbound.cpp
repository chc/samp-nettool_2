#include "SAMPClient.h"
#include "SAMPClient_Inbound.h"

#include "rak_minimal/GetTime.h"

namespace SAMP {
	SAMPClientMsgHandler SAMPInboundClientHandler::m_msg_handlers[] = {
		{ID_CONNECTION_REQUEST, ESAMPAuthState_WaitConnReq, &SAMPInboundClientHandler::m_handle_conn_req},
		{ID_AUTH_KEY, ESAMPAuthState_SentAuthKey, &SAMPInboundClientHandler::m_handle_auth_key},
		{ID_NEW_INCOMING_CONNECTION, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_new_incoming_conn},
		{ID_RPC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_rpc},
		{ID_INTERNAL_PING, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_internal_ping},
		{ID_PLAYER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_VEHICLE_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_PASSENGER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		//{ID_MARKERS_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_UNOCCUPIED_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_AIM_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		//{ID_TRAILER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		//{ID_PASSENGER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_BULLET_SYNC, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_sync},
		{ID_STATS_UPDATE, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_stats_update},
		{ID_DETECT_LOST_CONNECTIONS, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_detect_lost_connections},
		{ID_CONNECTED_PONG, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_connected_pong},
		{ID_DISCONNECTION_NOTIFICATION, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_disconnect},
		{ID_RECEIVED_STATIC_DATA, ESAMPAuthState_ConnAccepted, &SAMPInboundClientHandler::m_handle_recv_static_data},
	};
	SAMPInboundClientHandler::SAMPInboundClientHandler(SAMPPacketHandlerSendFunc func, SAMP::Client *client, const struct sockaddr_in *in_addr) : SAMPPacketHandler(in_addr) {
		m_raknet_mode = false;
		m_server_cookie = 0x6969;
		m_sent_cookie_packet = false;
		mp_send_func = NULL;

		mp_send_func = func;
		mp_client = client;

		m_transtate_out.m_out_seq = 0;

		m_in_addr = *in_addr;

		m_transtate_out.m_ordering_channel = 1;

		gettimeofday(&m_last_sent_ping, NULL);
	}
	SAMPInboundClientHandler::~SAMPInboundClientHandler() {
	}
	void SAMPInboundClientHandler::handle_bitstream(RakNet::BitStream *stream) {
		if(!m_raknet_mode) {
			handle_nonrak_packet(stream);
		} else {
			handle_raknet_packet(stream);
		}
	}
	void SAMPInboundClientHandler::handle_raknet_packet(RakNet::BitStream *stream) {
		//void readRaknetPacket(RakNetPacketHead &head, RakNet::BitStream *input);
		RakNetPacketHead packet;
		//std::vector<RakNetByteSeq> byte_seqs;
		readRaknetPacket(packet, stream);
		std::vector<RakNetByteSeq>::iterator it = packet.byte_seqs.begin();
		while(it != packet.byte_seqs.end()) {
			RakNetByteSeq byte_seq = *it;
			if(byte_seq.reliability == RELIABLE || byte_seq.reliability == RELIABLE_SEQUENCED || byte_seq.reliability == RELIABLE_ORDERED) {
				m_transtate_out.m_send_acks.push_back(byte_seq.seqid);
			}
			process_racket_sequence(byte_seq);
			it++;
		}
		if(m_transtate_out.m_send_acks.size() > 0)
			sendByteSeqs(m_transtate_out, m_send_queue, mp_send_func, mp_client, false);
		//freeRaknetPacket(&packet);
	}
	void SAMPInboundClientHandler::handle_nonrak_packet(RakNet::BitStream *stream) {
		uint8_t msgid;
		stream->Read(msgid);
		//printf("Nonraknet got: %d\n",msgid);
		switch(msgid) {
			case ID_OPEN_CONNECTION_REQUEST:
				send_cookie_request();
				break;
			default:
				break;
		}
	}
	void SAMPInboundClientHandler::send_cookie_request() {
		RakNet::BitStream bs;
		//bs.Write((uint8_t)ID_OPEN_CONNECTION_COOKIE);
		//bs.Write((uint16_t)m_server_cookie);

		m_sent_cookie_packet = true;

		bs.Write((uint8_t)ID_OPEN_CONNECTION_REPLY);
		bs.Write((uint8_t)0);
		m_raknet_mode = true;

		mp_send_func(bs, mp_client, false);

	}
	void SAMPInboundClientHandler::send_open_conn_reply() {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_OPEN_CONNECTION_REPLY);
		mp_send_func(bs, mp_client, false);
	}
	void SAMPInboundClientHandler::m_new_incoming_conn(RakNet::BitStream *data, PacketEnumeration id) {
	}
	void SAMPInboundClientHandler::tick(fd_set *set) {
		mp_mutex->lock();
		if(m_send_queue.size() > 0 || m_transtate_out.m_send_acks.size() > 0) {
			sendByteSeqs(m_transtate_out, m_send_queue, mp_send_func, mp_client, false);
			/*
			for(std::vector<RakNetByteSeq>::iterator it = m_send_queue.begin();it != m_send_queue.end();it++) {
				RakNetByteSeq seq = *it;
				delete seq.data;
			}
			*/
			m_send_queue.clear();
		}
		mp_mutex->unlock();
		struct timeval current_time;
		gettimeofday(&current_time, NULL);
		
		if(current_time.tv_sec - m_last_sent_ping.tv_sec > SAMP_SEND_PING_TIME) {
			gettimeofday(&m_last_sent_ping, NULL);
			send_ping();
		}
	}

	void SAMPInboundClientHandler::send_ping() {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_INTERNAL_PING);
		bs.Write((uint32_t)RakNet::GetTime());

		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}

	void SAMPInboundClientHandler::process_racket_sequence(RakNetByteSeq &byte_seq) {
		void (SAMPInboundClientHandler::*func)(RakNet::BitStream *, PacketEnumeration id);
		uint8_t msgid;
		RakNet::BitStream *data = byte_seq.data;
		data->Read(msgid);
		for(int i=0;i<sizeof(m_msg_handlers)/sizeof(_SAMPClientMsgHandlers);i++) {
			if(m_msg_handlers[i].id == msgid) {
				//printf("C->S Got msg %d - %d(%d) - seq %d\n", msgid, data->GetNumberOfBitsUsed(),data->GetNumberOfBytesUsed(),byte_seq.seqid);
				func = m_msg_handlers[i].mpHandler;
				(*this.*func)(data, (PacketEnumeration)msgid);
				return;
			}
		}
		//printf("C->S Couldn't find msg handler for 0x%02X - %d\n",msgid,msgid);
	}
	void dump_raknet_bitstream(RakNet::BitStream *stream, const char *fmt, ...);
	void SAMPInboundClientHandler::m_handle_rpc(RakNet::BitStream *data, PacketEnumeration id) {
		uint8_t rpc_id;

		uint32_t bits = 0;
		data->Read(rpc_id);
		data->ReadCompressed(bits);

		RakNet::BitStream bs;
		#define MAX_SYNC_SIZE 1024
		unsigned char sync_data[MAX_SYNC_SIZE];
		int unread_bits = data->GetNumberOfUnreadBits();
		data->ReadBits((unsigned char *)&sync_data, unread_bits);
		bs.WriteBits(sync_data, unread_bits);

		//dump_raknet_bitstream(&bs, "C_rpc_%d.bin", rpc_id);
		static int i = 0;
		dump_raknet_bitstream(&bs, "C_rpc_%d_%d.bin", rpc_id,i++);
		bs.ResetReadPointer();
		//printf("C->S got rpc %d - %d(%d)\n",rpc_id,bits,BITS_TO_BYTES(bits));
		Py::OnGotRPC(mp_client, rpc_id, &bs, true);
	}
	void SAMPInboundClientHandler::m_handle_conn_req(RakNet::BitStream *data, PacketEnumeration id) {
		char pass[256];
		uint8_t len = 0;
		pass[0] = 0;

		data->Read(pass, data->GetNumberOfBytesUsed()-1);

		if(pass[0] != 0) {
			m_password = pass;
		}
		printf("Got conn req (%s)\n", pass);
		//send_samp_rakauth("277C2AD934406F33");
		const char *key = "277C2AD934406F33";
		len = strlen(key);

		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_AUTH_KEY);
		
		bs.Write(len);
		bs.Write(key,len);
		bs.Write((char)0);
		
		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPInboundClientHandler::m_handle_auth_key(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		EConnRejectReason response = Py::OnNewConnection(mp_client->GetServer(), mp_client, m_password);
		switch(response) {
			case EConnRejectReason_Accepted:
				bs.Write((uint8_t)ID_CONNECTION_REQUEST_ACCEPTED);
				#ifdef _WIN32
					bs.Write((uint32_t)m_in_addr.sin_addr.S_un.S_addr);
				#else
					bs.Write((uint32_t)m_in_addr.sin_addr.s_addr);
				#endif
				bs.Write((uint16_t)m_in_addr.sin_port);
				bs.Write((uint16_t)16);
				bs.Write((uint32_t)0xFFFFFFFF);
			break;
			case EConnRejectReason_InvalidPass:
				bs.Write((uint8_t)ID_INVALID_PASS);
			break;
			case EConnRejectReason_Banned:
				bs.Write((uint8_t)ID_CONNECTION_BANNED);
			break;
			case EConnRejectReason_ServerFull:
				bs.Write((uint8_t)ID_NO_FREE_INCOMING_CONNECTIONS);
			break;
			case EConnRejectReason_ConnFailed:
				bs.Write((uint8_t)ID_CONNECTION_ATTEMPT_FAILED);
			break;
			case EConnRejectReason_Disconnected:
				bs.Write((uint8_t)ID_DISCONNECTION_NOTIFICATION);
			break;
			case EConnRejectReason_FailedEncryption:
				bs.Write((uint8_t)ID_FAILED_INIT_ENCRYPTION);
			break;
		}

		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPInboundClientHandler::m_handle_internal_ping(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)id);
		bs.Write(data);
		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPInboundClientHandler::m_handle_sync(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		#define MAX_SYNC_SIZE 1024
		unsigned char sync_data[MAX_SYNC_SIZE];
		int unread_bits = data->GetNumberOfUnreadBits();
		data->ReadBits((unsigned char *)&sync_data, unread_bits);
		bs.WriteBits(sync_data, unread_bits);
		bs.ResetReadPointer();
		//AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
		Py::OnGotSync(mp_client, id, &bs, true);
	}
	void SAMPInboundClientHandler::m_handle_stats_update(RakNet::BitStream *data, PacketEnumeration id) {
		int32_t money, drunk;

		data->Read(money);
		data->Read(drunk);

		Py::OnGotStatsUpdate(mp_client, money, drunk);
	}
	void SAMPInboundClientHandler::m_handle_detect_lost_connections(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)id);
		bs.Write(data);
		AddToOutputStream(&bs, RELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPInboundClientHandler::m_connected_pong(RakNet::BitStream *data, PacketEnumeration id) {
	}
	void SAMPInboundClientHandler::m_handle_disconnect(RakNet::BitStream *data, PacketEnumeration id) {
		printf("C->S D/C Packet len: %d %d\n",data->GetNumberOfBitsUsed(), data->GetNumberOfBytesUsed());
		uint8_t type;
		data->Read(type);
		printf("C->S DC Type: %d\n", type);
	}
	void SAMPInboundClientHandler::m_handle_recv_static_data(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_RECEIVED_STATIC_DATA);
		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
	

}