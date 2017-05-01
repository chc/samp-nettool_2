#include "main.h"
#include "SAMPAuth.h"
#include "SAMPClient.h"
#include "SAMPClient_Outbound.h"
#include "GetTime.h"
namespace SAMP {
	SAMPOutboundClientMsgHandler SAMPOutboundClientHandler::m_msg_handlers[] = {
		{ID_CONNECTION_REQUEST_ACCEPTED , ESAMPAuthState_WaitConnReq, &SAMPOutboundClientHandler::m_handle_conn_req},
		{ID_AUTH_KEY, ESAMPAuthState_SentAuthKey, &SAMPOutboundClientHandler::m_handle_auth_key},
		{ID_RPC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_rpc},
		{ID_INTERNAL_PING, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_internal_ping},
		{ID_PING, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_internal_ping},
		{ID_DETECT_LOST_CONNECTIONS, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_detect_lost_connections},
		{ID_PLAYER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_VEHICLE_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_AIM_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_MARKERS_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_SPECTATOR_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_PASSENGER_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_UNOCCUPIED_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_BULLET_SYNC, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_sync},
		{ID_DISCONNECTION_NOTIFICATION, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_disconnect},
		{ID_RECEIVED_STATIC_DATA, ESAMPAuthState_ConnAccepted, &SAMPOutboundClientHandler::m_handle_recv_static_data},
	};
	SAMPOutboundClientHandler::SAMPOutboundClientHandler(SAMPPacketHandlerSendFunc send_func, SAMPPacketHandlerRecvFunc recv_func, SAMP::Client *client, const struct sockaddr_in *in_addr) : SAMPPacketHandler(in_addr) {
		m_raknet_mode = false;
		m_server_cookie = 0x6969;
		m_sent_cookie_packet = false;
		mp_send_func = NULL;

		mp_send_func = send_func;
		mp_recv_func = recv_func;
		mp_client = client;

		m_transtate_out.m_out_seq = 0;

		m_in_addr = *in_addr;

		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_OPEN_CONNECTION_REQUEST);
		bs.Write((uint16_t)SAMP_COOKIE);

		m_transtate_out.m_ordering_channel = 2;

		mp_send_func(bs, mp_client, true);
	}
	SAMPOutboundClientHandler::~SAMPOutboundClientHandler() {
	}
	void SAMPOutboundClientHandler::handle_bitstream(RakNet::BitStream *stream) {
		if(!m_raknet_mode) {
			handle_nonrak_packet(stream);
		} else {
			handle_raknet_packet(stream);
		}
	}
	void SAMPOutboundClientHandler::handle_raknet_packet(RakNet::BitStream *stream) {
		RakNetPacketHead packet;
		readRaknetPacket(packet, stream);
		
		std::vector<RakNetByteSeq>::iterator it = packet.byte_seqs.begin();
		while(it != packet.byte_seqs.end()) {
			RakNetByteSeq byte_seq = *it;
			if(byte_seq.reliability == RELIABLE || byte_seq.reliability == RELIABLE_SEQUENCED || byte_seq.reliability == RELIABLE_ORDERED) {
				m_transtate_out.m_send_acks.push_back(byte_seq.seqid);
			}
			//handle non-split packets instantly, split packets are processed later
			if(!byte_seq.has_split_packet) {
				process_racket_sequence(byte_seq);
			} else {
				//store split packet for later
				m_split_data[byte_seq.split_packet_id].m_sequences[byte_seq.split_packet_index] = byte_seq;
				m_split_data[byte_seq.split_packet_id].m_count = byte_seq.split_packet_count;
			}
			it++;
		}
		if(m_transtate_out.m_send_acks.size() > 0) {
			sendByteSeqs(m_transtate_out, m_send_queue, mp_send_func, mp_client, true);
			m_send_queue.clear();
		}
		tryProcessSplitPackets();
	}
	void SAMPOutboundClientHandler::handle_nonrak_packet(RakNet::BitStream *stream) {
		RakNet::BitStream bs;
		uint8_t msgid;
		stream->Read(msgid);
		//printf("Nonraknet got: %d\n",msgid);
		uint16_t cookie;
		switch(msgid) {
			case ID_OPEN_CONNECTION_COOKIE:
				stream->Read(cookie);
				bs.Write((uint8_t)ID_OPEN_CONNECTION_REQUEST);
				bs.Write((uint16_t)(cookie ^ SAMP_COOKIE));
				mp_send_func(bs, mp_client, true);
				break;
			case ID_OPEN_CONNECTION_REPLY:
				m_raknet_mode = true;
				bs.Write((uint8_t)ID_CONNECTION_REQUEST);
				AddToOutputStream(&bs, RELIABLE, SAMP::HIGH_PRIORITY);
				
				break;
			default:
				break;
		}
	}
	void SAMPOutboundClientHandler::send_cookie_request() {
		RakNet::BitStream bs;
		//bs.Write((uint8_t)ID_OPEN_CONNECTION_COOKIE);
		//bs.Write((uint16_t)m_server_cookie);

		m_sent_cookie_packet = true;

		bs.Write((uint8_t)ID_OPEN_CONNECTION_REPLY);
		bs.Write((uint8_t)0);
		m_raknet_mode = true;

		mp_send_func(bs, mp_client, true);

	}
	void SAMPOutboundClientHandler::send_open_conn_reply() {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_OPEN_CONNECTION_REPLY);
		mp_send_func(bs, mp_client, true);
	}
	void SAMPOutboundClientHandler::tick(fd_set *set) {
		mp_mutex->lock();
		if(m_send_queue.size() > 0 || m_transtate_out.m_send_acks.size() > 0) {
			sendByteSeqs(m_transtate_out, m_send_queue, mp_send_func, mp_client, true);
			/*
			for(std::vector<RakNetByteSeq>::iterator it = m_send_queue.begin();it != m_send_queue.end();it++) {
				RakNetByteSeq seq = *it;
				delete seq.data;
			}
			*/
			m_transtate_out.m_send_acks.clear();
			m_send_queue.clear();
		}
		if(set) {
			if(FD_ISSET(mp_client->GetSocket(), set)) {
				RakNet::BitStream bs;
				mp_recv_func(bs, mp_client, false);
				handle_bitstream(&bs);
			}

		}
		mp_mutex->unlock();
	}

	void SAMPOutboundClientHandler::process_racket_sequence(RakNetByteSeq &byte_seq) {
		void (SAMPOutboundClientHandler::*func)(RakNet::BitStream *, PacketEnumeration);
		uint8_t msgid;
		RakNet::BitStream *data = byte_seq.data;
		data->Read(msgid);
		for(int i=0;i<sizeof(m_msg_handlers)/sizeof(_SAMPOutboundClientMsgHandlers);i++) {
			if(m_msg_handlers[i].id == msgid) {
				func = m_msg_handlers[i].mpHandler;
				(*this.*func)(data, (PacketEnumeration)msgid);
				return;
			}
		}
		printf("S->C Couldn't find msg handler for 0x%02X - %d\n",msgid,msgid);
	}
	void dump_raknet_bitstream(RakNet::BitStream *stream, const char *fmt, ...) {
		int offset = stream->GetReadOffset();

		char name[256];
		va_list args;
		va_start (args, fmt);
		vsprintf (name,fmt, args);
		va_end (args);

		stream->ResetReadPointer();

		FILE *fd = fopen(name, "wb");
		char *data = (char *)malloc(stream->GetNumberOfBytesUsed());
		stream->ReadBits((unsigned char *)data, stream->GetNumberOfBitsUsed());
		fwrite(data, stream->GetNumberOfBytesUsed(), 1, fd);
		free(data);
		fclose(fd);

		stream->SetReadOffset(offset);
	}
	void SAMPOutboundClientHandler::m_handle_rpc(RakNet::BitStream *data, PacketEnumeration id) {
		uint8_t rpc_id;

		uint32_t bits = 0;
		data->Read(rpc_id);
		data->ReadCompressed(bits);

		RakNet::BitStream bs;
		//#define MAX_SYNC_SIZE 1024
		//unsigned char sync_data[MAX_SYNC_SIZE];
		//int unread_bits = data->GetNumberOfUnreadBits();
		//data->ReadBits((unsigned char *)&sync_data, unread_bits);
		//bs.WriteBits(sync_data, unread_bits);
		bs.Write(data);

		//printf("S->C got rpc %d - %d(%d)\n",rpc_id,bits,BITS_TO_BYTES(bits));
		//static int i = 0;
		//dump_raknet_bitstream(&bs, "S_rpc_%d_%d.bin", rpc_id,i++);
		//dump_raknet_bitstream(&bs, "S_rpc_%d.bin", rpc_id);
		bs.ResetReadPointer();
		Py::OnGotRPC(mp_client, rpc_id, &bs, false);
	}
	void SAMPOutboundClientHandler::m_handle_sync(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		#define MAX_SYNC_SIZE 1024
		unsigned char sync_data[MAX_SYNC_SIZE];
		int unread_bits = data->GetNumberOfUnreadBits();
		data->ReadBits((unsigned char *)&sync_data, unread_bits);
		bs.WriteBits(sync_data, unread_bits);
		bs.ResetReadPointer();
		//AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
		Py::OnGotSync(mp_client, id, &bs, false);
	}
	void SAMPOutboundClientHandler::m_handle_conn_req(RakNet::BitStream *data, PacketEnumeration id) {
		uint32_t ip;
		uint16_t port;
		uint16_t player_id;
		uint32_t challenge;
		data->Read(ip);
		data->Read(port);
		data->Read(player_id);
		data->Read(challenge);

		m_client_addr.sin_addr.s_addr = ip;
		m_client_addr.sin_port = port;
		m_player_id = player_id;

		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_NEW_INCOMING_CONNECTION);
		#ifdef _WIN32
			bs.Write((uint32_t)m_client_addr.sin_addr.S_un.S_addr);
		#else
			bs.Write((uint32_t)m_client_addr.sin_addr.s_addr);
		#endif
		bs.Write((uint16_t)m_client_addr.sin_port);
		AddToOutputStream(&bs, RELIABLE, SAMP::HIGH_PRIORITY);

		Py::OnConnectionAccepted(mp_client, m_player_id, challenge);
	}
	void SAMPOutboundClientHandler::m_handle_auth_key(RakNet::BitStream *data, PacketEnumeration id) {
		char key[256];
		uint8_t len;
		
		data->Read(len);
		data->Read(key,len);
		key[len]=0;


		const char *auth_key = GetAuthKey(key);

		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_AUTH_KEY);
		len = strlen(auth_key);
		bs.Write(len);
		bs.Write(auth_key,len);

		AddToOutputStream(&bs, RELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPOutboundClientHandler::m_handle_internal_ping(RakNet::BitStream *data, PacketEnumeration id) {
		uint8_t msgid;
		uint32_t times[2];
		RakNet::BitStream bs;
		//data->Read(msgid);
		data->Read(times[0]);
		bs.Write((uint8_t)ID_CONNECTED_PONG);
		times[1] = (times[0] + (RakNet::GetTime() - times[0]));
		bs.Write(times[0]);
		bs.Write(times[1]);
		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPOutboundClientHandler::m_handle_detect_lost_connections(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)id);
		bs.Write(data);
		AddToOutputStream(&bs, RELIABLE, SAMP::HIGH_PRIORITY);
	}
	void SAMPOutboundClientHandler::m_handle_disconnect(RakNet::BitStream *data, PacketEnumeration id) {
		printf("D/C Packet len: %d %d\n",data->GetNumberOfBitsUsed(), data->GetNumberOfBytesUsed());
		uint8_t type;
		data->Read(type); //always uninitalized
		printf("DC Type: %d\n", type);
	}
	void SAMPOutboundClientHandler::m_handle_recv_static_data(RakNet::BitStream *data, PacketEnumeration id) {
		RakNet::BitStream bs;
		bs.Write((uint8_t)ID_RECEIVED_STATIC_DATA);
		AddToOutputStream(&bs, UNRELIABLE, SAMP::HIGH_PRIORITY);
	}
}