#include "SAMPPacket.h"
#include "SAMPClient.h"
#include "python\Python.h"
namespace SAMP {
	void SAMPPacketHandler::AddToOutputStream(RakNet::BitStream *bs, PacketReliability reliability, PacketPriority priority) {
		
		RakNetByteSeq seq;
		seq.seqid = m_transtate_out.m_out_seq++;
		seq.reliability = reliability;
		seq.has_split_packet = false;
		seq.data = new RakNet::BitStream();
		bs->ResetReadPointer();
		seq.data->Write(bs);
		seq.data->ResetReadPointer();
		mp_mutex->lock();
		m_send_queue.push_back(seq);
		mp_mutex->unlock();
	}
	void sendPacket(RakNetPacketHead packet, bool splits_processed, SAMPPacketHandlerSendFunc mp_send_func, void *extra, bool encrypt) {
		//NetDbgPrintf("Send input packet called\n");
		//const char *direction = "[S->C]";
		//if(send_to_server) {		
			//direction = "[C->S]";
		//}
		//dump_raknet_packet(packet, send_to_server);

		if(!splits_processed) {
			/*
			std::vector<RakNetPacketHead *> split_packets = getSplitPacket(packet, send_to_server);
			std::vector<RakNetPacketHead *>::iterator it = split_packets.begin();
			if(split_packets.size() > 0) {
				//NetDbgPrintf("Got %d split packets\n",split_packets.size());
				while(it != split_packets.end()) {
			
					RakNetPacketHead *p = *it;
					sendPacket(p, send_to_server, true);
					//NetDbgPrintf("Sending split packet : %d\n", send_to_server);
					it++;
				}
				//freeSplitPackets(split_packets); TODO: probs can completely remove
				return;
			}
			*/
		}

		unsigned int slen = sizeof(struct sockaddr_in);
		RakNet::BitStream output(MTUSize);
		bool has_acks = packet.acknowlegements.Size() > 0;
		output.Write(has_acks);
		if(has_acks) {
			//NetDbgPrintf("*** %s Writing %d acks\n", direction,packet.acknowlegements.Size());
			uint16_t messageNumber;
			for (int i=0; i<packet.acknowlegements.ranges.Size();i++)
			{
				if (packet.acknowlegements.ranges[i].minIndex>packet.acknowlegements.ranges[i].maxIndex)
				{
					break;
				}

				for (messageNumber=packet.acknowlegements.ranges[i].minIndex; messageNumber >= packet.acknowlegements.ranges[i].minIndex && messageNumber <= packet.acknowlegements.ranges[i].maxIndex; messageNumber++)
				{
					//printf("\t======= ack msgid: %d\n", messageNumber);
				}
			}
			packet.acknowlegements.Serialize(&output, (MTUSize-UDP_HEADER_SIZE)*8-1, true);
		}
		int count = 0;
		//printf("Writing %d seqs\n", packet.byte_seqs.size());
		for(std::vector<RakNetByteSeq>::iterator it = packet.byte_seqs.begin(); it != packet.byte_seqs.end();it++,count++) {
			RakNetByteSeq &seq = *it;
			output.Write(seq.seqid);

			//printf("Write seq: %d\n", seq.seqid);
			output.WriteBits((unsigned char *)&seq.reliability, 4, true);
			//NetDbgPrintf("Write reliability: %d\n", seq.reliability);
			if(seq.reliability == UNRELIABLE_SEQUENCED || seq.reliability == RELIABLE_SEQUENCED || seq.reliability == RELIABLE_ORDERED ) {
				output.WriteBits((unsigned char *)&seq.orderingChannel, 5, true);
				output.Write(seq.orderingIndexType);
				//NetDbgPrintf("Write Chan: %d Index: %d\n",seq.orderingChannel, seq.orderingIndexType);
			}

			output.Write((bool)seq.has_split_packet); //split packet
			if(seq.has_split_packet) {
				//NetDbgPrintf("%s Write split packet: %d %d %d\n", direction,seq.split_packet_id, seq.split_packet_index, seq.split_packet_count);
				output.Write(seq.split_packet_id);
				output.WriteCompressed(seq.split_packet_index);
				output.WriteCompressed(seq.split_packet_count);
			} else {
				//NetDbgPrintf("%s Not writing split packet\n", direction);
			}

			uint16_t bits = seq.data->GetNumberOfBitsUsed();

			output.WriteCompressed(bits);

			char data[MTUSize];

			seq.data->ResetReadPointer();
			seq.data->ReadBits((unsigned char *)&data, bits);

			#if NETCODE_DEBUG_CHECKSTREAM
				CheckStreams(seq.original_data, seq.data, send_to_server);
			#endif

			#if NETCODE_DEBUG_FILE
			if(seq.has_split_packet) {
				static int cnt = 0;
				FILE *fd;
				char name[256];
				if(send_to_server) {
					sprintf(name, "send_split_packet_%d_CS.bin", cnt++);
				} else {
					sprintf(name, "send_split_packet_%d_SC.bin", cnt++);
				}

				fd = fopen(name, "wb");
				fwrite((unsigned char *)&data, BITS_TO_BYTES(bits), 1, fd);
				fclose(fd);
			}
			#endif

			output.AlignWriteToByteBoundary();
			output.WriteBits((unsigned char *)&data, bits);
			output.AlignWriteToByteBoundary();


			#if NETCODE_DEBUG_CHECKSTREAM
			if(seq.original_data)
				delete seq.original_data;
			#endif

			//NetDbgPrintf("%s Sending packet size %d\n",direction,BITS_TO_BYTES(bits));
			//printf("Sending msg: %d [%d]\n", (uint8_t)data[0], BITS_TO_BYTES(bits));
		}

		if(count == 0 && !has_acks) {
			return;
		}
		if(count == 0) {
			output.AlignWriteToByteBoundary();
			output.Write(false);
		}
		mp_send_func(output, extra, encrypt);
	}
	void sendByteSeqs(RaknetStreamTransState &trans_state, const std::vector<RakNetByteSeq> seqs, SAMPPacketHandlerSendFunc mp_send_func, void *extra, bool encrypt) {
		RakNetPacketHead head;
		std::vector<int>::iterator it = trans_state.m_send_acks.begin();
		while(it != trans_state.m_send_acks.end()) {
			int seq = *it;
			head.acknowlegements.Insert(seq);
			it++;
		}
		trans_state.m_send_acks.clear();

		std::vector<RakNetByteSeq>::const_iterator it2 = seqs.begin();
		while(it2 != seqs.end()){
			const RakNetByteSeq seq = *it2;
			head.byte_seqs.push_back(seq);
			it2++;
		}

		sendPacket(head, false, mp_send_func, extra, encrypt);
	}
	void readRaknetPacket(RakNetPacketHead &head, RakNet::BitStream *input) {
		RakNet::BitStream output;
		bool hasacks;
		input->Read(hasacks);
		//NetDbgPrintf("**** %s msg read loop\n",direction);
		if(hasacks) {
			head.acknowlegements.Deserialize(input);
		}
		
		while(BITS_TO_BYTES(input->GetNumberOfUnreadBits()) > 0) {
			RakNetByteSeq packet;
			memset(&packet,0,sizeof(packet));
			packet.data = new RakNet::BitStream(MTUSize);
			packet.data->Reset();

			if(!input->Read(packet.seqid)) 
				break;


			if(!input->ReadBits(&packet.reliability, 4))
				break;

			
			//NetDbgPrintf("\t*** %s reliability: %d\n", direction,packet.reliability);
			if(packet.reliability == UNRELIABLE_SEQUENCED || packet.reliability == RELIABLE_SEQUENCED || packet.reliability == RELIABLE_ORDERED ) {
				input->ReadBits((unsigned char *)&packet.orderingChannel, 5);
				input->Read(packet.orderingIndexType);
				//NetDbgPrintf("\t*** %s reliability: %d - (chan: %d  index: %d)\n",direction,packet.reliability,packet.orderingChannel, packet.orderingIndexType);
			} else {
				packet.orderingChannel = 0;
				packet.orderingIndexType = 0;
			}



			if(!input->Read(packet.has_split_packet)) {
				break;
			}

			if(packet.has_split_packet) {
				input->Read(packet.split_packet_id);
				input->ReadCompressed(packet.split_packet_index);
				input->ReadCompressed(packet.split_packet_count);
			} else {
				packet.split_packet_id = 0;
				packet.split_packet_count = 0;
				packet.split_packet_index = 0;
			}
			//loop through all data
			uint16_t data_len;
			if(!input->ReadCompressed(data_len)) {
				break;
			}
			char data[4096];
			input->AlignReadToByteBoundary();
			if(data_len > 0) {
				if(!input->ReadBits((unsigned char *)&data, data_len)) {
				}

				packet.data->WriteBits((unsigned char *)&data, data_len);
			}
			input->AlignReadToByteBoundary();

			packet.data->AlignWriteToByteBoundary();
			packet.data->ResetReadPointer();

			head.byte_seqs.push_back(packet);
		}
	}
	void freeRaknetPacket(RakNetPacketHead *packet) {
		std::vector<RakNetByteSeq>::iterator it = packet->byte_seqs.begin();
		while(it != packet->byte_seqs.end()) {
			RakNetByteSeq seq = *it;
			delete seq.data;
			it++;
		}
	}
}