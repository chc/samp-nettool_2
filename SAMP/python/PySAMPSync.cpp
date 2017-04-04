#include "SAMP/SAMPPacket.h"
#include "PySAMPSync.h"
#include <Python.h>
namespace Py {
	PyObject* SyncToPyDict_Player(SAMP::PLAYER_SYNC_INFO *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();

		//build dict
		PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("leftright_keys"), PyLong_FromLong(sync->leftright_keys));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("updown_keys"), PyLong_FromLong(sync->updown_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("keys"), PyLong_FromLong(sync->keys));


		PyObject *py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("pos"), py_pos);

		py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->move_speed[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->move_speed[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->move_speed[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("vel"), py_pos);

		PyObject *py_quats = PyList_New(4);
		PyList_SET_ITEM(py_quats,0,PyFloat_FromDouble(sync->quat[0]));
		PyList_SET_ITEM(py_quats,1,PyFloat_FromDouble(sync->quat[1]));
		PyList_SET_ITEM(py_quats,2,PyFloat_FromDouble(sync->quat[2]));
		PyList_SET_ITEM(py_quats,3,PyFloat_FromDouble(sync->quat[3]));	
		PyDict_SetItem(sync_dict, PyUnicode_FromString("quat"), py_quats);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("health"), PyLong_FromLong(sync->health));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("armour"), PyLong_FromLong(sync->armour));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("weapon"), PyLong_FromLong(sync->weapon));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("specialaction"), PyLong_FromLong(sync->specialaction));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("surf_flags"), PyLong_FromLong(sync->surf_flags));

		PyObject *py_surfoffset = PyList_New(3);
		PyList_SET_ITEM(py_surfoffset,0,PyFloat_FromDouble(sync->surfoffset[0]));
		PyList_SET_ITEM(py_surfoffset,1,PyFloat_FromDouble(sync->surfoffset[1]));
		PyList_SET_ITEM(py_surfoffset,2,PyFloat_FromDouble(sync->surfoffset[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("surf_offset"), py_surfoffset);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("anim"), PyLong_FromLong(sync->anim));
		//Py_INCREF(sync_dict);
		return sync_dict;
	}
	void PyDictToSync_Player(SAMP::PLAYER_SYNC_INFO *out, PyObject* dict, bool client_to_server) {
		//move back to player sync struct
		PyObject *dict_item;

		dict_item = PyDict_GetItemString(dict, "playerid");
		out->playerid = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "leftright_keys");
		out->leftright_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "updown_keys");
		out->updown_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "keys");
		out->keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "pos");
		out->pos[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->pos[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->pos[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));


		dict_item = PyDict_GetItemString(dict, "quat");
		out->quat[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->quat[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->quat[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));
		out->quat[3] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 3));

		dict_item = PyDict_GetItemString(dict, "vel");
		out->move_speed[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->move_speed[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->move_speed[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "health");
		out->health = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "armour");
		out->armour = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "weapon");
		out->weapon = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "specialaction");
		out->specialaction = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "surf_flags");
		out->surf_flags = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "surf_offset");
		out->surfoffset[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->surfoffset[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->surfoffset[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "anim");
		out->anim = PyLong_AsLong(dict_item);
	}

	PyObject* SyncToPyDict_Vehicle(SAMP::VEHICLE_SYNC_INFO *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("vehicleid"), PyLong_FromLong(sync->vehicleid));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("leftright_keys"), PyLong_FromLong(sync->leftright_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("updown_keys"), PyLong_FromLong(sync->updown_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("keys"), PyLong_FromLong(sync->keys));

		PyObject *py_quats = PyList_New(4);
		PyList_SET_ITEM(py_quats,0,PyFloat_FromDouble(sync->quat[0]));
		PyList_SET_ITEM(py_quats,1,PyFloat_FromDouble(sync->quat[1]));
		PyList_SET_ITEM(py_quats,2,PyFloat_FromDouble(sync->quat[2]));
		PyList_SET_ITEM(py_quats,3,PyFloat_FromDouble(sync->quat[3]));	
		PyDict_SetItem(sync_dict, PyUnicode_FromString("quat"), py_quats);

		PyObject *py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("pos"), py_pos);


		py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->vel[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->vel[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->vel[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("vel"), py_pos);

		PyObject *py_vel = PyList_New(3);
		PyList_SET_ITEM(py_vel ,0,PyFloat_FromDouble(sync->vel[0]));
		PyList_SET_ITEM(py_vel ,1,PyFloat_FromDouble(sync->vel[1]));
		PyList_SET_ITEM(py_vel ,2,PyFloat_FromDouble(sync->vel[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("vel"), py_vel);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("health"), PyFloat_FromDouble(sync->health));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("player_health"), PyLong_FromLong(sync->player_health));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("player_armour"), PyLong_FromLong(sync->player_armour));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("weapon"), PyLong_FromLong(sync->weapon));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("siren"), sync->siren ? Py_True : Py_False);
		PyDict_SetItem(sync_dict, PyUnicode_FromString("landing_gear"), sync->landing_gear ? Py_True : Py_False);

		return sync_dict;
	}
	void PyDictToSync_Vehicle(SAMP::VEHICLE_SYNC_INFO *out, PyObject* dict, bool client_to_server) {
 		PyObject *dict_item = PyDict_GetItemString(dict, "playerid");
		out->playerid = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "vehicleid");
		out->vehicleid = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "leftright_keys");
		out->leftright_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "updown_keys");
		out->updown_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "keys");
		out->keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "pos");
		out->pos[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->pos[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->pos[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "vel");
		out->vel[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->vel[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->vel[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));


		dict_item = PyDict_GetItemString(dict, "quat");
		out->quat[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->quat[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->quat[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));
		out->quat[3] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 3));

		dict_item = PyDict_GetItemString(dict, "health");
		out->health = PyFloat_AsDouble(dict_item);

		dict_item = PyDict_GetItemString(dict, "player_health");
		out->player_health = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "player_armour");
		out->player_armour = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "weapon");
		out->weapon = PyLong_AsLong(dict_item);

	}

	PyObject* SyncToPyDict_Aim(SAMP::SAMPAimSync *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyObject *py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("pos"), py_pos);


		py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->angle[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->angle[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->angle[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("angle"), py_pos);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("z"), PyFloat_FromDouble(sync->z));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("cam_mode"), PyLong_FromLong(sync->cam_mode));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("cam_zoom"), PyLong_FromLong(sync->cam_zoom));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("weapon_state"), PyLong_FromLong(sync->weapon_state));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("unknown"), PyLong_FromLong(sync->unknown));

		if(!client_to_server) {
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->player_id));
		}

		return sync_dict;

	}
	void PyDictToSync_Aim(SAMP::SAMPAimSync *out, PyObject* dict, bool client_to_server) {
 			PyObject *dict_item = PyDict_GetItemString(dict, "unknown");
			out->unknown = PyLong_AsLong(dict_item);

			dict_item = PyDict_GetItemString(dict, "weapon_state");
			out->weapon_state = PyLong_AsLong(dict_item);

			dict_item = PyDict_GetItemString(dict, "cam_mode");
			out->cam_mode = PyLong_AsLong(dict_item);

			dict_item = PyDict_GetItemString(dict, "cam_zoom");
			out->cam_zoom = PyLong_AsLong(dict_item);

			dict_item = PyDict_GetItemString(dict, "z");
			out->z = PyFloat_AsDouble(dict_item);

			dict_item = PyDict_GetItemString(dict, "pos");
			out->pos[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
			out->pos[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
			out->pos[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

			dict_item = PyDict_GetItemString(dict, "angle");
			out->angle[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
			out->angle[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
			out->angle[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->player_id = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->player_id = (uint16_t)-1;
		}
	}
	PyObject* SyncToPyDict_Bullet(SAMP::SAMPBulletSync *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyObject *py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->origin[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->origin[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->origin[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("origin"), py_pos);


		py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->target[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->target[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->target[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("target"), py_pos);

		py_pos = PyList_New(3);
		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->center[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->center[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->center[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("center"), py_pos);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("id"), PyLong_FromLong(sync->id));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("weapon"), PyLong_FromLong(sync->weapon));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("type"), PyLong_FromLong(sync->type));


		if(!client_to_server) {
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->player_id));
		}

		return sync_dict;
	}
	void PyDictToSync_Bullet(SAMP::SAMPBulletSync *out, PyObject* dict, bool client_to_server) {
		PyObject *dict_item;
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->player_id = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->player_id = (uint16_t)-1;
		}

		dict_item = PyDict_GetItemString(dict, "origin");
		out->origin[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->origin[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->origin[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "target");
		out->target[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->target[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->target[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "center");
		out->center[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->center[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->center[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "weapon");
		out->weapon = (uint8_t)PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "type");
		out->type= (uint8_t)PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "id");
		out->id = (uint8_t)PyLong_AsLong(dict_item);
	}
	/*
	PyObject* SyncToPyDict_Marker(SAMP::SAMPMarkerSync *sync, bool client_to_server) {
		if(client_to_server) {
			return NULL;
		}
	

		PyObject *py_list = PyList_New(sync->count);
		for(int i=0;i<sync->count;i++) {
			PyObject *sync_dict = PyDict_New();
			PyObject *py_pos = PyList_New(3);

			PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->pos[i][0]));
			PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->pos[i][1]));
			//PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->pos[i][2]));
			PyDict_SetItem(sync_dict, PyUnicode_FromString("pos"), py_pos);
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid[i]));
			PyDict_SetItem(sync_dict, PyUnicode_FromString("unk"), PyLong_FromLong(sync->unk));

			PyList_SET_ITEM(py_list, i, sync_dict);
		}



		return py_list;
	}

	void PyDictToSync_Marker(SAMP::SAMPMarkerSync *out, PyObject* dict, bool client_to_server) {
	
		out->count = PyList_Size(dict);
		for(int i=0;i<out->count;i++) {
			PyObject* dict_item = PyList_GetItem(dict, i);
			PyObject *item = PyDict_GetItemString(dict_item, "playerid");
			out->playerid[i] = PyLong_AsLong(item);

			item = PyDict_GetItemString(dict_item, "unk");
			out->unk = PyLong_AsLong(item);

			item = PyDict_GetItemString(dict_item, "pos");
			out->pos[i][0] = PyFloat_AsDouble(PyList_GetItem(item, 0));
			out->pos[i][1] = PyFloat_AsDouble(PyList_GetItem(item, 1));
			//out->pos[i][2] = PyFloat_AsDouble(PyList_GetItem(item, 2));
		}
	}*/
	void PySAMP_AddPacketDefs(PyObject *module) {
		PyModule_AddIntConstant(module, "PACKET_PLAYER_SYNC", SAMP::ID_PLAYER_SYNC);
		PyModule_AddIntConstant(module, "PACKET_VEHICLE_SYNC", SAMP::ID_VEHICLE_SYNC);
		PyModule_AddIntConstant(module, "PACKET_BULLET_SYNC", SAMP::ID_BULLET_SYNC);
		PyModule_AddIntConstant(module, "PACKET_AIM_SYNC", SAMP::ID_AIM_SYNC);
	}
}