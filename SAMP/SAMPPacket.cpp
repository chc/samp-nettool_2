#include "SAMPPacket.h"
#include "SAMPClient.h"
#include "python/Python.h"
#include <algorithm>
namespace SAMP {
	void SAMPPacketHandler::AddToOutputStream(RakNet::BitStream *bs, PacketReliability reliability, PacketPriority priority) {
		
		RakNetByteSeq seq;
		seq.seqid = m_transtate_out.m_out_seq++;
		seq.reliability = reliability;
		seq.has_split_packet = false;

		if(seq.reliability == UNRELIABLE_SEQUENCED || seq.reliability == RELIABLE_SEQUENCED || seq.reliability == RELIABLE_ORDERED ) {
			seq.orderingChannel = m_transtate_out.m_ordering_channel;
			seq.orderingIndexType = m_transtate_out.m_ordering_index[seq.orderingChannel]++;
		}

		seq.data = new RakNet::BitStream();

		bs->ResetReadPointer();
		seq.data->Write(bs);

		seq.data->ResetReadPointer();

		mp_mutex->lock();
		m_send_queue.push_back(seq);
		mp_mutex->unlock();
	}
	void SAMPPacketHandler::sendPacket(RakNetPacketHead packet, bool splits_processed, SAMPPacketHandlerSendFunc mp_send_func, void *extra, bool encrypt) {
		//NetDbgPrintf("Send input packet called\n");
		//const char *direction = "[S->C]";
		//if(send_to_server) {		
			//direction = "[C->S]";
		//}
		//dump_raknet_packet(packet, send_to_server);

		if(!splits_processed) {
			
			std::vector<RakNetPacketHead *> split_packets = getSplitPacket(&packet);
			std::vector<RakNetPacketHead *>::iterator it = split_packets.begin();
			if(split_packets.size() > 0) {
				//NetDbgPrintf("Got %d split packets\n",split_packets.size());
				while(it != split_packets.end()) {
			
					RakNetPacketHead *p = *it;
					p->acknowlegements = packet.acknowlegements;
					packet.acknowlegements.Clear();
					sendPacket(*p, true, mp_send_func, extra, encrypt);
					//NetDbgPrintf("Sending split packet : %d\n", send_to_server);
					it++;
				}
				freeSplitPackets(split_packets);
				//freeRaknetPacket(&packet);
				return;
			}
		}

		unsigned int slen = sizeof(struct sockaddr_in);
		RakNet::BitStream output(MTUSize);
		bool has_acks = packet.acknowlegements.Size() > 0;
		output.Write(has_acks);

		if(has_acks) {
			/*if(encrypt) {
				uint16_t messageNumber;
				for (int i=0; i<packet.acknowlegements.ranges.Size();i++)
				{
					if (packet.acknowlegements.ranges[i].minIndex>packet.acknowlegements.ranges[i].maxIndex)
					{
						break;
					}

					for (messageNumber=packet.acknowlegements.ranges[i].minIndex; messageNumber >= packet.acknowlegements.ranges[i].minIndex && messageNumber <= packet.acknowlegements.ranges[i].maxIndex; messageNumber++)
					{
						printf("\t ======= ack msgid: %d\n", messageNumber);
					}
				}
			}
			*/
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
				//printf("Write Chan: %d Index: %d\n",seq.orderingChannel, seq.orderingIndexType);
			}

			if(seq.reliability == RELIABLE || seq.reliability == RELIABLE_SEQUENCED || seq.reliability == RELIABLE_ORDERED) {
				m_transtate_out.m_send_acks.push_back(seq.seqid);
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

		//freeRaknetPacket(&packet); //will need to stay for resends later
	}
	void SAMPPacketHandler::sendByteSeqs(RaknetStreamTransState &trans_state, const std::vector<RakNetByteSeq> seqs, SAMPPacketHandlerSendFunc mp_send_func, void *extra, bool encrypt) {
		RakNetPacketHead head;
		std::vector<int>::iterator it = trans_state.m_send_acks.begin();
		bool has_acks = false;
		while(it != trans_state.m_send_acks.end()) {
			int seq = *it;
			head.acknowlegements.Insert(seq);
			it++;
			has_acks = true;
		}
		trans_state.m_send_acks.clear();

		std::vector<RakNetByteSeq>::const_iterator it2 = seqs.begin();
		while(it2 != seqs.end()){
			const RakNetByteSeq seq = *it2;
			head.byte_seqs.push_back(seq);
			it2++;
		}

		if(seqs.size() == 0 && !has_acks)
			return;
		
		if(head.acknowlegements.Size() > 0) {
			if(encrypt) {
				//printf("C->S Sending %d acks\n",head.acknowlegements.Size());
			} else {
				//printf("S->C Sending %d acks\n",head.acknowlegements.Size());
			}
		}
		
		

		sendPacket(head, false, mp_send_func, extra, encrypt);
	}
	void SAMPPacketHandler::readRaknetPacket(RakNetPacketHead &head, RakNet::BitStream *input) {
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
				//printf("\t*** reliability: %d - (chan: %d  index: %d)\n",packet.reliability,packet.orderingChannel, packet.orderingIndexType);
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
	std::map<void *, int> deleted;
	void SAMPPacketHandler::freeRaknetPacket(RakNetPacketHead *packet) {
		std::vector<RakNetByteSeq>::iterator it = packet->byte_seqs.begin();
		while(it != packet->byte_seqs.end()) {
			RakNetByteSeq seq = *it;
			deleted[seq.data]++;
			if(deleted[seq.data] > 1) {
				printf("** Double delete %p %d\n",seq.data, deleted[seq.data]);
			}
			delete seq.data;
			it++;
		}
	}
	int SAMPPacketHandler::getPacketAckSerializedLen(RakNetPacketHead *packet) {
		return sizeof(uint16_t) * 50;
	}
	int SAMPPacketHandler::GetHeaderLength(RakNetPacketHead *packet, bool with_data) {	
		#define SEQ_HEAD_MAX_LEN 10
		int seq_len = (packet->byte_seqs.size() * SEQ_HEAD_MAX_LEN);
		std::vector<RakNetByteSeq>::iterator it = packet->byte_seqs.begin();
		while(it != packet->byte_seqs.end()) {
			RakNetByteSeq seq = *it;
			seq_len += seq.data->GetNumberOfBytesUsed();
			it++;
		}
		return seq_len;
	}
	int SAMPPacketHandler::GetSeqLength(RakNetByteSeq *seq) {
		return SEQ_HEAD_MAX_LEN + seq->data->GetNumberOfBytesUsed();
	} 
	/*
		TODO: free split packets
	*/
	std::vector<RakNetPacketHead *> SAMPPacketHandler::getSplitPacket(RakNetPacketHead *packet) {
		std::vector<RakNetPacketHead *> ret;
		int ack_size = getPacketAckSerializedLen(packet);
		int seq_total_size = GetHeaderLength(packet, true);
		int total_size = BYTES_TO_BITS(ack_size + seq_total_size);
		if(total_size > SPLIT_CHUNK_SIZE_BITS) { //must perform split
			//NetDbgPrintf("Must perform split: %d\n",total_size);
			performPacketSplitting(ret, packet);
		}  else {
			/*
			RakNetPacketHead *cpy_pkt = new RakNetPacketHead;
			cpy_pkt->acknowlegements = packet->acknowlegements;
			cpy_pkt->byte_seqs = packet->byte_seqs;
			ret.push_back(cpy_pkt);
			*/
		}

		return ret;
	}

	/*
		No input packets should be split at input

		Step 1:
			Go through all seqs splitting sequences > MTUSize into seperate chunks
		Step 2:
			Go through all seqs, calculating header + seq size, adding sequences until cursize + seq size > MTUSize
	*/
	void SAMPPacketHandler::performPacketSplitting(std::vector<RakNetPacketHead *> &ret, RakNetPacketHead *packet) {
		//RakNetByteSeq seq = packet->byte_seqs.at(0);

		std::vector<RakNetByteSeq> split_seqs;

		//step 1
		int header_len = GetHeaderLength(packet, false);
		std::vector<RakNetByteSeq>::iterator it = packet->byte_seqs.begin();
		while(it != packet->byte_seqs.end()) {
			int cur_channel;
			RakNetByteSeq seq = *it;

			if(header_len + seq.data->GetNumberOfBytesUsed() < MTUSize) {
				seq.has_split_packet = false;
				split_seqs.push_back(seq);
			} else { //must do splitting
				RakNet::BitStream *full_bs = seq.data;
				int cur_seq = 0;
				cur_channel = m_transtate_out.m_out_split_id++;
				int i = 0;
				int num_packets = ceil((float)full_bs->GetNumberOfBitsUsed() / (float)SPLIT_CHUNK_SIZE_BITS);
				full_bs->ResetReadPointer();
				seq.split_packet_count = num_packets;
				seq.has_split_packet = true;

				int write_len = full_bs->GetNumberOfBitsUsed();
				while(write_len > 0) {
					
					cur_seq = m_transtate_out.m_out_seq++;
					seq.data = perform_packet_split(SPLIT_CHUNK_SIZE_BITS, write_len, full_bs);
					seq.has_split_packet = true;
					seq.seqid = (uint16_t)cur_seq;
					seq.split_packet_index = i++;
					seq.split_packet_id = cur_channel;
					seq.split_packet_count = num_packets;
					split_seqs.push_back(seq);
				}
			}
			it++;
		}

		//step 2 - TODO: free
		int packet_size = 0;
		RakNetPacketHead *head = new RakNetPacketHead;
		ret.push_back(head);
		it = split_seqs.begin();
		while(it != split_seqs.end()) {
			RakNetByteSeq seq = *it;
			if(this->GetHeaderLength(head, true) + GetSeqLength(&seq) > SPLIT_CHUNK_SIZE) {
				head = new RakNetPacketHead;
				ret.push_back(head);
			}
			head->byte_seqs.push_back(seq);
			it++;
		}
	
		if(std::find(ret.begin(),ret.end(),head) == ret.end()) {
			ret.push_back(head);
		}
		//NetDbgPrintf("Sending split packet size %d\n",ret.size());
	}
	RakNet::BitStream *SAMPPacketHandler::perform_packet_split(int chunk_bits, int &remaining, RakNet::BitStream *bitstream) {
		RakNet::BitStream *ret = new RakNet::BitStream(BITS_TO_BYTES(chunk_bits));
		if(remaining >= chunk_bits) {
			remaining -= chunk_bits;
			ret->Write(bitstream, chunk_bits);
		} else {
			ret->Write(bitstream, remaining);
			remaining = 0;
		}
		return ret;
	}
	void SAMPPacketHandler::freeSplitPackets(std::vector<RakNetPacketHead *> split_packets) {
		std::vector<RakNetPacketHead *>::iterator it = split_packets.begin();
		while(it != split_packets.end()) {
			RakNetPacketHead *p = *it;		
			std::vector<RakNetByteSeq>::iterator seq_it = p->byte_seqs.begin();
			/*
			while(seq_it != p->byte_seqs.end()) {
				RakNetByteSeq seq = *seq_it;
				deleted[seq.data]++;
				if(deleted[seq.data] > 1) {
					printf("** Double delete %p %d\n",seq.data, deleted[seq.data]);
				}
				delete seq.data; //deleted by free packet
				seq_it++;
			}
			*/
			delete p;
			it++;
		}
	}
}