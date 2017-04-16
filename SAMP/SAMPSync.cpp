#include <Python.h>
#include <BitStream.h>
#include "SAMPSync.h"

namespace SAMP {
	void ReadPlayerSync(SAMP::PLAYER_SYNC_INFO *player_sync, RakNet::BitStream *in, bool client_to_server) {
		bool temp_bool;
		uint8_t type;

		uint8_t byteHealth, byteArmour;
		uint8_t byteArmTemp=0,byteHlTemp=0;

		if(client_to_server) {
			player_sync->playerid = (uint16_t)-1;
			in->Read(player_sync->leftright_keys);
			in->Read(player_sync->updown_keys);
			in->Read(player_sync->keys);
			in->Read(player_sync->pos[0]);
			in->Read(player_sync->pos[1]);
			in->Read(player_sync->pos[2]);
			in->Read(player_sync->quat[0]);
			in->Read(player_sync->quat[1]);
			in->Read(player_sync->quat[2]);
			in->Read(player_sync->quat[3]);
			in->Read(player_sync->health);
			in->Read(player_sync->armour);
			in->Read(player_sync->weapon);
			in->Read(player_sync->specialaction);
			in->Read(player_sync->move_speed[0]);
			in->Read(player_sync->move_speed[1]);
			in->Read(player_sync->move_speed[2]);
			in->Read(player_sync->surfoffset[0]);
			in->Read(player_sync->surfoffset[1]);
			in->Read(player_sync->surfoffset[2]);
			in->Read(player_sync->surf_flags);
			in->Read(player_sync->anim);
		} else {
			//read sync data
			in->Read(player_sync->playerid);

			in->Read(temp_bool);
			if(temp_bool) {
				in->Read(player_sync->leftright_keys);
			} else {
				player_sync->leftright_keys = 0;
			}

			in->Read(temp_bool);

			if(temp_bool) {
				in->Read(player_sync->updown_keys);
			} else {
				player_sync->updown_keys = 0;
			}

			in->Read(player_sync->keys);
			in->Read(player_sync->pos[0]);
			in->Read(player_sync->pos[1]);
			in->Read(player_sync->pos[2]);
			in->ReadNormQuat(player_sync->quat[0], player_sync->quat[1], player_sync->quat[2], player_sync->quat[3]);

			in->Read(type); //health/armour

			byteArmTemp = (type & 0x0F);
			byteHlTemp = (type >> 4);

			if(byteArmTemp == 0xF) byteArmour = 100;
			else if(byteArmTemp == 0) byteArmour = 0;
			else byteArmour = byteArmTemp * 7;

			if(byteHlTemp == 0xF) byteHealth = 100;
			else if(byteHlTemp == 0) byteHealth = 0;
			else byteHealth = byteHlTemp * 7;

			player_sync->armour = byteArmour;
			player_sync->health = byteHealth;


			in->Read(player_sync->weapon);
			in->Read(player_sync->specialaction);


			in->ReadVector(player_sync->move_speed[0], player_sync->move_speed[1], player_sync->move_speed[2]);;

			in->Read(temp_bool);
			if(temp_bool) {
				in->Read(player_sync->surf_flags);
				in->Read(player_sync->surfoffset[0]);
				in->Read(player_sync->surfoffset[1]);
				in->Read(player_sync->surfoffset[2]);
			} else {
				player_sync->surf_flags = 0;
			}

			in->Read(temp_bool);
			if(temp_bool) {
				in->Read(player_sync->anim);
			} else {
				player_sync->anim = 0;
			}
		}
	}
	void WritePlayerSync(SAMP::PLAYER_SYNC_INFO *player_sync, RakNet::BitStream *out, bool client_to_server) {
		uint8_t health_armour;
		if(client_to_server) {
			out->Write(player_sync->leftright_keys);
			out->Write(player_sync->updown_keys);
			out->Write(player_sync->keys);

			out->Write(player_sync->pos[0]);
			out->Write(player_sync->pos[1]);
			out->Write(player_sync->pos[2]);

			out->Write(player_sync->quat[0]);
			out->Write(player_sync->quat[1]);
			out->Write(player_sync->quat[2]);
			out->Write(player_sync->quat[3]);
			out->Write(player_sync->health);
			out->Write(player_sync->armour);
			out->Write(player_sync->weapon);
			out->Write(player_sync->specialaction);
			out->Write(player_sync->move_speed[0]);
			out->Write(player_sync->move_speed[1]);
			out->Write(player_sync->move_speed[2]);

			out->Write(player_sync->surfoffset[0]);
			out->Write(player_sync->surfoffset[1]);
			out->Write(player_sync->surfoffset[2]);
			out->Write(player_sync->surf_flags);
			out->Write(player_sync->anim);
		} else {
			//write sync data
			out->Write(player_sync->playerid);

			out->Write((bool)(player_sync->leftright_keys != 0));
			if(player_sync->leftright_keys != 0)
				out->Write(player_sync->leftright_keys);

			out->Write((bool)(player_sync->updown_keys != 0));
			if(player_sync->updown_keys != 0)
				out->Write(player_sync->updown_keys);

			out->Write(player_sync->keys);

			out->Write(player_sync->pos[0]);
			out->Write(player_sync->pos[1]);
			out->Write(player_sync->pos[2]);

			out->WriteNormQuat(player_sync->quat[0], player_sync->quat[1], player_sync->quat[2],player_sync->quat[3]);

			if(player_sync->health > 0 && player_sync->health < 100) {
				health_armour = ((uint8_t)(player_sync->health / 7)) << 4;
			} else if(player_sync->health >= 100) {
				health_armour = 0xF << 4;
			}
			if(player_sync->armour > 0 && player_sync->armour < 100) {
				health_armour |=  (uint8_t)(player_sync->armour / 7);
			} else if(player_sync->armour >= 100) {
				health_armour |= 0xF;
			}
			out->Write(health_armour);

			out->Write(player_sync->weapon);
			out->Write(player_sync->specialaction);

			out->WriteVector(player_sync->move_speed[0], player_sync->move_speed[1], player_sync->move_speed[2]);
				
			out->Write((bool)(player_sync->surf_flags != 0));
			if(player_sync->surf_flags) {
				out->Write(player_sync->surf_flags);
				out->Write(player_sync->surfoffset[0]);
				out->Write(player_sync->surfoffset[1]);
				out->Write(player_sync->surfoffset[2]);
			}
			out->Write((bool)(player_sync->anim != 0));
			if(player_sync->anim) {
				out->Write(player_sync->anim);
			}
		}
		out->AlignWriteToDWORDBoundary();
	}

	void ReadVehicleSync(SAMP::VEHICLE_SYNC_INFO *vehicle_sync, RakNet::BitStream *in, bool client_to_server) {
		uint8_t type;

		uint8_t byteHealth, byteArmour;
		uint8_t byteArmTemp=0,byteHlTemp=0;

		if(client_to_server) {
			vehicle_sync->playerid = (uint16_t)-1;
			in->Read(vehicle_sync->vehicleid);
			in->Read(vehicle_sync->leftright_keys);
			in->Read(vehicle_sync->updown_keys);
			in->Read(vehicle_sync->keys);
			in->Read(vehicle_sync->quat[0]);
			in->Read(vehicle_sync->quat[1]);
			in->Read(vehicle_sync->quat[2]);
			in->Read(vehicle_sync->quat[3]);
			in->Read(vehicle_sync->pos[0]);
			in->Read(vehicle_sync->pos[1]);
			in->Read(vehicle_sync->pos[2]);
			in->Read(vehicle_sync->vel[0]);
			in->Read(vehicle_sync->vel[1]);
			in->Read(vehicle_sync->vel[2]);
			in->Read(vehicle_sync->health);
			in->Read(vehicle_sync->player_health);
			in->Read(vehicle_sync->player_armour);
			in->Read(vehicle_sync->weapon);

			uint8_t val;
			in->Read(val);
			vehicle_sync->siren = val;

			in->Read(val);
			vehicle_sync->landinggear_state = val;

			
			uint16_t angle;
			in->Read(angle);
			vehicle_sync->angle = angle;

			in->Read(vehicle_sync->train_speed);
			vehicle_sync->train = vehicle_sync->train_speed != 0.0;

		} else {
			in->Read(vehicle_sync->playerid);
			in->Read(vehicle_sync->vehicleid);

			in->Read(vehicle_sync->leftright_keys);
			in->Read(vehicle_sync->updown_keys);
			in->Read(vehicle_sync->keys);

			in->ReadNormQuat(vehicle_sync->quat[0], vehicle_sync->quat[1], vehicle_sync->quat[2], vehicle_sync->quat[3]);

			in->Read(vehicle_sync->pos[0]);
			in->Read(vehicle_sync->pos[1]);
			in->Read(vehicle_sync->pos[2]);

			in->ReadVector(vehicle_sync->vel[0], vehicle_sync->vel[1], vehicle_sync->vel[2]);

			uint16_t health;
			in->Read(health);
			vehicle_sync->health = (float)health;

			in->Read(type); //health/armour

			byteArmTemp = (type & 0x0F);
			byteHlTemp = (type >> 4);

			if(byteArmTemp == 0xF) byteArmour = 100;
			else if(byteArmTemp == 0) byteArmour = 0;
			else byteArmour = byteArmTemp * 7;

			if(byteHlTemp == 0xF) byteHealth = 100;
			else if(byteHlTemp == 0) byteHealth = 0;
			else byteHealth = byteHlTemp * 7;

			vehicle_sync->player_health = byteHealth;
			vehicle_sync->player_armour = byteArmour;


			in->Read(vehicle_sync->weapon);

			in->ReadCompressed(vehicle_sync->siren);

			in->ReadCompressed(vehicle_sync->landinggear_state);

			in->ReadCompressed(vehicle_sync->hydra);
			in->ReadCompressed(vehicle_sync->trailer);
			in->Read(vehicle_sync->angle);


			in->ReadCompressed(vehicle_sync->train);
			if(vehicle_sync->train) {
				uint16_t val;
				in->Read(val);
				vehicle_sync->train_speed = val;
			}


		}
	}
	void WriteVehicleSync(SAMP::VEHICLE_SYNC_INFO *vehicle_sync, RakNet::BitStream *out, bool client_to_server) {
		uint8_t type;
		if(client_to_server) {
			out->Write(vehicle_sync->vehicleid);
			out->Write(vehicle_sync->leftright_keys);
			out->Write(vehicle_sync->updown_keys);
			out->Write(vehicle_sync->keys);

			out->Write(vehicle_sync->quat[0]);
			out->Write(vehicle_sync->quat[1]);
			out->Write(vehicle_sync->quat[2]);
			out->Write(vehicle_sync->quat[3]);

			out->Write(vehicle_sync->pos[0]);
			out->Write(vehicle_sync->pos[1]);
			out->Write(vehicle_sync->pos[2]);

			out->Write(vehicle_sync->vel[0]);
			out->Write(vehicle_sync->vel[1]);
			out->Write(vehicle_sync->vel[2]);
			out->Write(vehicle_sync->health);
			out->Write(vehicle_sync->player_health);
			out->Write(vehicle_sync->player_armour);
			out->Write(vehicle_sync->weapon);
			out->Write((uint8_t)vehicle_sync->siren);
			out->Write((uint8_t)vehicle_sync->landinggear_state);
			out->Write((uint16_t)vehicle_sync->angle); //thrust/bike angle
			out->Write(vehicle_sync->train_speed);
			//out->Write((uint8_t)0); //unknown
		} else {
			out->Write(vehicle_sync->playerid);
			out->Write(vehicle_sync->vehicleid);

			out->Write(vehicle_sync->leftright_keys);
			out->Write(vehicle_sync->updown_keys);
			out->Write(vehicle_sync->keys);

			out->WriteNormQuat(vehicle_sync->quat[0], vehicle_sync->quat[1], vehicle_sync->quat[2], vehicle_sync->quat[3]);

			out->Write(vehicle_sync->pos[0]);
			out->Write(vehicle_sync->pos[1]);
			out->Write(vehicle_sync->pos[2]);

			out->WriteVector(vehicle_sync->vel[0], vehicle_sync->vel[1], vehicle_sync->vel[2]);

			out->Write((uint16_t)vehicle_sync->health);

			if(vehicle_sync->player_health > 0 && vehicle_sync->player_health < 100) {
				type = ((uint8_t)(vehicle_sync->player_health / 7)) << 4;
			} else if(vehicle_sync->player_health >= 100) {
				type = 0xF << 4;
			}
			if(vehicle_sync->player_armour > 0 && vehicle_sync->player_armour < 100) {
				type |=  (uint8_t)(vehicle_sync->player_armour / 7);
			} else if(vehicle_sync->player_armour >= 100) {
				type |= 0xF;
			}
			out->Write(type);

			out->Write(vehicle_sync->weapon);
			out->WriteCompressed(vehicle_sync->siren);
			out->WriteCompressed(vehicle_sync->landinggear_state);

			out->WriteCompressed(vehicle_sync->hydra); //hydra
			out->WriteCompressed(vehicle_sync->trailer); //trailer

			out->Write(vehicle_sync->angle);

			out->WriteCompressed(vehicle_sync->train);

			if(vehicle_sync->train) {
				out->Write((uint16_t)vehicle_sync->train_speed);
			}
		}

		out->AlignWriteToDWORDBoundary();
	}

	void ReadAimSync(SAMP::SAMPAimSync *aim_sync, RakNet::BitStream *in, bool client_to_server) {
		if(!client_to_server) {
			in->Read(aim_sync->player_id);
		} else {
			aim_sync->player_id = (uint16_t)-1;
		}
		in->Read(aim_sync->cam_mode);
		in->Read(aim_sync->angle[0]);
		in->Read(aim_sync->angle[1]);
		in->Read(aim_sync->angle[2]);
		in->Read(aim_sync->pos[0]);
		in->Read(aim_sync->pos[1]);
		in->Read(aim_sync->pos[2]);
		in->Read(aim_sync->z);
		in->ReadBits(&aim_sync->cam_zoom, 6);
		in->ReadBits(&aim_sync->weapon_state, 2);
		in->Read(aim_sync->unknown);
	}
	void WriteAimSync(SAMP::SAMPAimSync *aim_sync, RakNet::BitStream *out, bool client_to_server) {
		if(!client_to_server) {
			out->Write(aim_sync->player_id);
		}

		out->Write(aim_sync->cam_mode);
		out->Write(aim_sync->angle[0]);
		out->Write(aim_sync->angle[1]);
		out->Write(aim_sync->angle[2]);
		out->Write(aim_sync->pos[0]);
		out->Write(aim_sync->pos[1]);
		out->Write(aim_sync->pos[2]);
		out->Write(aim_sync->z);
		out->WriteBits(&aim_sync->cam_zoom, 6);
		out->WriteBits(&aim_sync->weapon_state, 2);
		out->Write(aim_sync->unknown);
		out->AlignWriteToDWORDBoundary();
	}

	void ReadBulletSync(SAMP::SAMPBulletSync *bullet_sync, RakNet::BitStream *in, bool client_to_server) {
		if(!client_to_server) {
			in->Read(bullet_sync->playerid);
		} else {
			bullet_sync->playerid = (uint16_t)-1;
		}
		in->Read(bullet_sync->type);	
		in->Read(bullet_sync->id);
		in->Read(bullet_sync->origin[0]);
		in->Read(bullet_sync->origin[1]);
		in->Read(bullet_sync->origin[2]);
		in->Read(bullet_sync->target[0]);
		in->Read(bullet_sync->target[1]);
		in->Read(bullet_sync->target[2]);
		in->Read(bullet_sync->center[0]);
		in->Read(bullet_sync->center[1]);
		in->Read(bullet_sync->center[2]);
		in->Read(bullet_sync->weapon);
	}
	void WriteBulletSync(SAMP::SAMPBulletSync *bullet_sync, RakNet::BitStream *out, bool client_to_server) {
		if(!client_to_server) {
			out->Write(bullet_sync->playerid);
		}
		out->Write(bullet_sync->type);
		out->Write(bullet_sync->id);
		out->Write(bullet_sync->origin[0]);
		out->Write(bullet_sync->origin[1]);
		out->Write(bullet_sync->origin[2]);
		out->Write(bullet_sync->target[0]);
		out->Write(bullet_sync->target[1]);
		out->Write(bullet_sync->target[2]);
		out->Write(bullet_sync->center[0]);
		out->Write(bullet_sync->center[1]);
		out->Write(bullet_sync->center[2]);
		out->Write(bullet_sync->weapon);
		out->AlignWriteToDWORDBoundary();
	}
	void ReadPassengerSync(SAMP::PASSENGER_SYNC_DATA *passenger_sync, RakNet::BitStream *in, bool client_to_server) {
		if(!client_to_server) {
			in->Read(passenger_sync->playerid);
		} else {
			passenger_sync->playerid = (uint16_t)-1;
		}
		in->Read(passenger_sync->vehicleid);
		in->ReadBits((unsigned char *)&passenger_sync->seat_flags, 7);
		in->ReadBits((unsigned char *)&passenger_sync->driveby, 1);
		in->Read(passenger_sync->currentweapon);
		in->Read(passenger_sync->health);
		in->Read(passenger_sync->armour);
		in->Read(passenger_sync->leftright_keys);
		in->Read(passenger_sync->updown_keys);
		in->Read(passenger_sync->keys);

		in->Read(passenger_sync->position[0]);
		in->Read(passenger_sync->position[1]);
		in->Read(passenger_sync->position[2]);
	}
	void WritePassengerSync(SAMP::PASSENGER_SYNC_DATA *passenger_sync, RakNet::BitStream *out, bool client_to_server) {
		if(!client_to_server) {
			out->Write(passenger_sync->playerid);
		}
		out->Write(passenger_sync->vehicleid);
		out->WriteBits((unsigned char *)&passenger_sync->seat_flags, 7);
		out->WriteBits((unsigned char *)&passenger_sync->driveby, 1);
		out->Write(passenger_sync->currentweapon);
		out->Write(passenger_sync->health);
		out->Write(passenger_sync->armour);
		out->Write(passenger_sync->leftright_keys);
		out->Write(passenger_sync->updown_keys);
		out->Write(passenger_sync->keys);
		out->Write(passenger_sync->position[0]);
		out->Write(passenger_sync->position[1]);
		out->Write(passenger_sync->position[2]);
		out->AlignWriteToDWORDBoundary();
	}

	void ReadSpectatorSync(SPECTATOR_SYNC_DATA *spectator_sync, RakNet::BitStream *in, bool client_to_server) {
		if(!client_to_server) {
			in->Read(spectator_sync->playerid);
		} else {
			spectator_sync->playerid = -1;
		}
		in->Read(spectator_sync->leftright_keys);
		in->Read(spectator_sync->updown_keys);
		in->Read(spectator_sync->keys);

		in->Read(spectator_sync->position[0]);
		in->Read(spectator_sync->position[1]);
		in->Read(spectator_sync->position[2]);
	}
	void WriteSpectatorSync(SPECTATOR_SYNC_DATA *spectator_sync, RakNet::BitStream *out, bool client_to_server) {
		if(!client_to_server) {
			out->Write(spectator_sync->playerid);
		}
		out->Write(spectator_sync->leftright_keys);
		out->Write(spectator_sync->updown_keys);
		out->Write(spectator_sync->keys);
		out->Write(spectator_sync->position[0]);
		out->Write(spectator_sync->position[1]);
		out->Write(spectator_sync->position[2]);
		out->AlignWriteToDWORDBoundary();
	}

}