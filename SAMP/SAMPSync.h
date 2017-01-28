#ifndef _SAMPSYNC_H
#define _SAMPSYNC_H
#include <stdint.h>
#include <BitStream.h>
#include <Python.h>
namespace SAMP {
	#pragma pack(1)
	typedef struct {
		uint16_t playerid;
		uint16_t leftright_keys;
		uint16_t updown_keys;
		uint16_t keys;
		float pos[3];
		float quat[4];
		uint8_t health;
		uint8_t armour;
		uint8_t weapon;
		uint8_t specialaction;
		float move_speed[3];
		float surfoffset[3];
		uint16_t surf_flags;
		uint32_t anim; //anim id, or delta??
	} PLAYER_SYNC_INFO;

	typedef struct {
		uint16_t playerid;
		uint16_t vehicleid;

		uint16_t leftright_keys;
		uint16_t updown_keys;
		uint16_t keys;

		float quat[4];
		float pos[3];
		float vel[3];
		float health;
		uint8_t player_health;
		uint8_t player_armour;

		uint8_t weapon;

		bool siren;

		bool landing_gear;

		uint8_t landinggear_state;
		uint8_t siren_on;
		uint8_t trailerid_or_thurstangle;
		float train_speed;

		//todo: train stuff, thrust angles/trailerid
	} VEHICLE_SYNC_INFO;

	typedef struct {
		uint8_t type;
		uint16_t player_id;
		uint16_t id;
		float origin[3];
		float target[3];
		float center[3];
		uint8_t weapon;
	} SAMPBulletSync;

	typedef struct {
		uint16_t player_id;
		uint8_t cam_mode;
		float angle[3]; //??
		float pos[3];
		float z;
		uint8_t cam_zoom; //6 bits
		uint8_t weapon_state; //2 bits
		uint8_t unknown;
	} SAMPAimSync;

	typedef struct {
		uint16_t vehicle_id;
		uint16_t roll[3];
		uint16_t direction[3];
		uint8_t unk[13];
		float pos[3];
		float velocity[3];
		float turning_speed[3];
		float health;
	} SAMPUnoccupiedVehData;

	void ReadPlayerSync(PLAYER_SYNC_INFO *player_sync, RakNet::BitStream *in, bool client_to_server);
	void WritePlayerSync(PLAYER_SYNC_INFO *player_sync, RakNet::BitStream *out, bool client_to_server);

	void ReadVehicleSync(VEHICLE_SYNC_INFO *vehicle_sync, RakNet::BitStream *in, bool client_to_server);
	void WriteVehicleSync(VEHICLE_SYNC_INFO *vehicle_sync, RakNet::BitStream *out, bool client_to_server);

	void ReadAimSync(SAMPAimSync *aim_sync, RakNet::BitStream *in, bool client_to_server);
	void WriteAimSync(SAMPAimSync *aim_sync, RakNet::BitStream *out, bool client_to_server);

	void ReadBulletSync(SAMPBulletSync *bullet_sync, RakNet::BitStream *in, bool client_to_server);
	void WriteBulletSync(SAMPBulletSync *bullet_sync, RakNet::BitStream *out, bool client_to_server);

	//void ReadMarkerSync(SAMPMarkerSync *map_sync, RakNet::BitStream *in, bool client_to_server);
	//void WriteMarkerSync(SAMPMarkerSync *map_sync, RakNet::BitStream *out, bool client_to_server);

	/*
	PyObject* SyncToPyDict_Player(PLAYER_SYNC_INFO *sync, bool client_to_server);
	void PyDictToSync_Player(PLAYER_SYNC_INFO *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Vehicle(VEHICLE_SYNC_INFO *sync, bool client_to_server);
	void PyDictToSync_Vehicle(VEHICLE_SYNC_INFO *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Aim(SAMPAimSync *sync, bool client_to_server);
	void PyDictToSync_Aim(SAMPAimSync *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Bullet(SAMPBulletSync *sync, bool client_to_server);
	void PyDictToSync_Bullet(SAMPBulletSync *out, PyObject* dict, bool client_to_server);
	*/

	//PyObject* SyncToPyDict_Marker(SAMPMarkerSync *sync, bool client_to_server);
	//void PyDictToSync_Marker(SAMPMarkerSync *out, PyObject* dict, bool client_to_server);
}
#endif //_SAMPSYNC_H