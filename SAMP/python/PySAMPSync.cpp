#include "SAMP/SAMPPacket.h"
#include "PySAMPSync.h"
#include <Python.h>

#include "Python.h"
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

		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->playerid = PyLong_AsLong(dict_item);
		} else {
			out->playerid = 0;
		}

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

		PyDict_SetItem(sync_dict, PyUnicode_FromString("landinggear_state"), sync->landinggear_state ? Py_True : Py_False);
		PyDict_SetItem(sync_dict, PyUnicode_FromString("train_speed"), PyFloat_FromDouble(sync->train_speed));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("hydra"), sync->hydra ? Py_True : Py_False);
		PyDict_SetItem(sync_dict, PyUnicode_FromString("trailer"), sync->trailer ? Py_True : Py_False);
		PyDict_SetItem(sync_dict, PyUnicode_FromString("angle"), PyLong_FromLong(sync->angle));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("train"), sync->train ? Py_True : Py_False);

		/*
		out->siren = PyLong_AsLong(PyDict_GetItemString(dict, "siren"));
		out->landinggear_state = PyLong_AsLong(PyDict_GetItemString(dict, "landinggear_state"));

		out->train_speed = PyFloat_AsDouble(PyDict_GetItemString(dict, "train_speed"));

		out->hydra = PyLong_AsLong(PyDict_GetItemString(dict, "hydra"));
		out->trailer = PyLong_AsLong(PyDict_GetItemString(dict, "trailer"));
		out->angle = PyLong_AsLong(PyDict_GetItemString(dict, "angle"));
		out->train = PyDict_GetItemString(dict, "train") == Py_True;
		*/
		return sync_dict;
	}
	void PyDictToSync_Vehicle(SAMP::VEHICLE_SYNC_INFO *out, PyObject* dict, bool client_to_server) {
 		PyObject *dict_item = PyDict_GetItemString(dict, "playerid");
		if(dict_item)
			out->playerid = PyLong_AsLong(dict_item);
		else
			out->playerid = -1;

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

		out->siren = PyDict_GetItemString(dict, "siren") == Py_True;
		out->landinggear_state = PyDict_GetItemString(dict, "landinggear_state") == Py_True;

		out->train_speed = PyFloat_AsDouble(PyDict_GetItemString(dict, "train_speed"));

		out->hydra = PyDict_GetItemString(dict, "hydra") == Py_True;
		out->trailer = PyDict_GetItemString(dict, "trailer") == Py_True;
		out->angle = PyLong_AsLong(PyDict_GetItemString(dict, "angle"));
		out->train = PyDict_GetItemString(dict, "train") == Py_True;
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
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));
		}

		return sync_dict;
	}
	void PyDictToSync_Bullet(SAMP::SAMPBulletSync *out, PyObject* dict, bool client_to_server) {
		PyObject *dict_item;
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->playerid = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->playerid = (uint16_t)-1;
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
	PyObject* SyncToPyDict_Passenger(SAMP::PASSENGER_SYNC_DATA *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyObject *py_pos = PyList_New(3);
		PyObject *dict_item;

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->position[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->position[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->position[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("position"), py_pos);


		if(sync->playerid != -1)
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("vehicleid"), PyLong_FromLong(sync->vehicleid));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("leftright_keys"), PyLong_FromLong(sync->leftright_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("updown_keys"), PyLong_FromLong(sync->updown_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("keys"), PyLong_FromLong(sync->keys));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("currentweapon"), PyLong_FromLong(sync->currentweapon));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("player_health"), PyLong_FromLong(sync->health));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("player_armour"), PyLong_FromLong(sync->armour));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("seat_flags"), PyLong_FromLong(sync->seat_flags));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("driveby"), PyLong_FromLong(sync->driveby));
		return sync_dict;
	}
	void PyDictToSync_Passenger(SAMP::PASSENGER_SYNC_DATA *out, PyObject* dict, bool client_to_server) {

		PyObject *dict_item;
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->playerid = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->playerid = (uint16_t)-1;
		}

		dict_item = PyDict_GetItemString(dict, "vehicleid");
		out->vehicleid = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "leftright_keys");
		out->leftright_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "updown_keys");
		out->updown_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "keys");
		out->keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "currentweapon");
		out->currentweapon = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "player_health");
		out->health= PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "player_armour");
		out->armour = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "seat_flags");
		out->seat_flags = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "driveby");
		out->driveby = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "position");
		out->position[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->position[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->position[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));
	}
	PyObject* SyncToPyDict_Spectator(SAMP::SPECTATOR_SYNC_DATA *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyObject *py_pos = PyList_New(3);
		PyObject *dict_item;

		if(sync->playerid != -1)
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->position[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->position[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->position[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("position"), py_pos);
		
		PyDict_SetItem(sync_dict, PyUnicode_FromString("leftright_keys"), PyLong_FromLong(sync->leftright_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("updown_keys"), PyLong_FromLong(sync->updown_keys));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("keys"), PyLong_FromLong(sync->keys));

		return sync_dict;
	}
	void PyDictToSync_Spectator(SAMP::SPECTATOR_SYNC_DATA *out, PyObject* dict, bool client_to_server) {
		PyObject *dict_item;
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->playerid = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->playerid = (uint16_t)-1;
		}

		dict_item = PyDict_GetItemString(dict, "position");
		out->position[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->position[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->position[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));


		dict_item = PyDict_GetItemString(dict, "leftright_keys");
		out->leftright_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "updown_keys");
		out->updown_keys = PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "keys");
		out->keys = PyLong_AsLong(dict_item);
	}

	PyObject* SyncToPyDict_UnoccupiedVeh(SAMP::SAMPUnoccupiedVehData *sync, bool client_to_server) {

		PyObject *sync_dict = PyDict_New();
		PyObject *py_pos = PyList_New(3);
		PyObject *dict_item;

		if(sync->playerid != -1)
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->playerid));

		PyDict_SetItem(sync_dict, PyUnicode_FromString("vehicleid"), PyLong_FromLong(sync->vehicle_id));

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("position"), py_pos);

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->velocity[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->velocity[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->velocity[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("velocity"), py_pos);

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->roll[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->roll[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->roll[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("roll"), py_pos);

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->direction[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->direction[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->direction[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("direction"), py_pos);

		PyList_SET_ITEM(py_pos ,0,PyFloat_FromDouble(sync->turning_speed[0]));
		PyList_SET_ITEM(py_pos ,1,PyFloat_FromDouble(sync->turning_speed[1]));
		PyList_SET_ITEM(py_pos ,2,PyFloat_FromDouble(sync->turning_speed[2]));
		PyDict_SetItem(sync_dict, PyUnicode_FromString("turning_speed"), py_pos);

		PyDict_SetItem(sync_dict, PyUnicode_FromString("health"), PyFloat_FromDouble(sync->health));

		return sync_dict;
	}
	void PyDictToSync_UnoccupiedVeh(SAMP::SAMPUnoccupiedVehData *out, PyObject* dict, bool client_to_server) {
		PyObject *dict_item;
		if(!client_to_server) {
			dict_item = PyDict_GetItemString(dict, "playerid");
			out->playerid = (uint16_t)PyLong_AsLong(dict_item);
		} else {
			out->playerid = (uint16_t)-1;
		}
		dict_item = PyDict_GetItemString(dict, "vehicleid");
		out->vehicle_id = (uint16_t)PyLong_AsLong(dict_item);

		dict_item = PyDict_GetItemString(dict, "roll");
		out->roll[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->roll[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->roll[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "direction");
		out->direction[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->direction[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->direction[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "position");
		out->pos[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->pos[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->pos[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "velocity");
		out->velocity[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->velocity[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->velocity[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "turning_speed");
		out->turning_speed[0] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 0));
		out->turning_speed[1] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 1));
		out->turning_speed[2] = PyFloat_AsDouble(PyList_GET_ITEM(dict_item, 2));

		dict_item = PyDict_GetItemString(dict, "health");
		out->health = PyFloat_AsDouble(dict_item);
	}

	
	PyObject* SyncToPyDict_Marker(SAMP::SAMPMarkerSync *sync, bool client_to_server) {
		if(client_to_server) {
			return NULL;
		}

		PyObject *dict = PyDict_New();
	

		PyObject *py_list = PyList_New(sync->entries.size());
		for(int i=0;i<sync->entries.size();i++) {
			PyObject *sync_dict = PyDict_New();
			PyObject *py_pos = PyList_New(3);

			PyList_SET_ITEM(py_pos,0, PyLong_FromLong(sync->entries[i].x));
			PyList_SET_ITEM(py_pos,1, PyLong_FromLong(sync->entries[i].y));
			PyList_SET_ITEM(py_pos,2, PyLong_FromLong(sync->entries[i].z));

			PyDict_SetItem(sync_dict, PyUnicode_FromString("pos"), py_pos);
			PyDict_SetItem(sync_dict, PyUnicode_FromString("playerid"), PyLong_FromLong(sync->entries[i].playerid));
			PyDict_SetItem(sync_dict, PyUnicode_FromString("active"), sync->entries[i].active ? Py_True : Py_False);

			PyList_SET_ITEM(py_list, i, sync_dict);
		}

		PyDict_SetItem(dict, PyUnicode_FromString("entries"), py_list);

		return dict;
	}

	void PyDictToSync_Marker(SAMP::SAMPMarkerSync *out, PyObject* dict, bool client_to_server) {
	
		SAMP::SAMPMarkerEntry entry;

		PyObject *entries_dict = PyDict_GetItemString(dict, "entries");
		int count = PyList_Size(entries_dict);
		for(int i=0;i<count;i++) {
			PyObject* dict_item = PyList_GetItem(entries_dict, i);
			PyObject *item = PyDict_GetItemString(dict_item, "playerid");
			entry.playerid = PyLong_AsLong(item);

			item = PyDict_GetItemString(dict_item, "active");
			entry.active = item == Py_True;

			item = PyDict_GetItemString(dict_item, "pos");
			entry.x = PyLong_AsLong(PyList_GetItem(item, 0));
			entry.y = PyLong_AsLong(PyList_GetItem(item, 1));
			entry.z = PyLong_AsLong(PyList_GetItem(item, 2));

			out->entries.push_back(entry);

		}
	}

	void PySAMP_AddPacketDefs(PyObject *module) {
		PyModule_AddIntConstant(module, "PACKET_PLAYER_SYNC", SAMP::ID_PLAYER_SYNC);
		PyModule_AddIntConstant(module, "PACKET_VEHICLE_SYNC", SAMP::ID_VEHICLE_SYNC);
		PyModule_AddIntConstant(module, "PACKET_PASSENGER_SYNC", SAMP::ID_PASSENGER_SYNC);
		PyModule_AddIntConstant(module, "PACKET_MARKERS_SYNC", SAMP::ID_MARKERS_SYNC);
		PyModule_AddIntConstant(module, "PACKET_UNOCCUPIED_SYNC", SAMP::ID_UNOCCUPIED_SYNC);
		PyModule_AddIntConstant(module, "PACKET_BULLET_SYNC", SAMP::ID_BULLET_SYNC);
		PyModule_AddIntConstant(module, "PACKET_AIM_SYNC", SAMP::ID_AIM_SYNC);

		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_ACCEPTED", EConnRejectReason_Accepted);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_INVALID_PASS", EConnRejectReason_InvalidPass);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_BANNED", EConnRejectReason_Banned);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_SERVER_FULL", EConnRejectReason_ServerFull);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_CONN_FAILED", EConnRejectReason_ConnFailed);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_CONN_LOST", EConnRejectReason_ConnLost);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_DISCONNECTED", EConnRejectReason_Disconnected);
		PyModule_AddIntConstant(module, "CONN_RESPONSE_REASON_FAILED_ENCRYPTION", EConnRejectReason_FailedEncryption);
	}
}