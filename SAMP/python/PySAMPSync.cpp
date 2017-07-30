#include "SAMP/SAMPPacket.h"
#include "PySAMPSync.h"
#include <Python.h>

#include "Python.h"
namespace Py {
	PyObject* SyncToPyDict_Player(SAMP::PLAYER_SYNC_INFO *sync, bool client_to_server) {
		PyObject *sync_dict = PyDict_New();
		PyObject *py_obj;

		//build dict
		py_obj = PyLong_FromLong(sync->playerid);
		PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->leftright_keys);
		PyDict_SetItemString(sync_dict, ("leftright_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->updown_keys);
		PyDict_SetItemString(sync_dict, ("updown_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->keys);
		PyDict_SetItemString(sync_dict, ("keys"), py_obj); Py_DECREF(py_obj);


		PyObject *py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItemString(sync_dict, ("pos"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->move_speed[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->move_speed[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->move_speed[2]));
		PyDict_SetItemString(sync_dict, ("vel"), py_pos); Py_DECREF(py_pos);

		PyObject *py_quats = PyList_New(4);
		PyList_SetItem(py_quats,0,PyFloat_FromDouble(sync->quat[0]));
		PyList_SetItem(py_quats,1,PyFloat_FromDouble(sync->quat[1]));
		PyList_SetItem(py_quats,2,PyFloat_FromDouble(sync->quat[2]));
		PyList_SetItem(py_quats,3,PyFloat_FromDouble(sync->quat[3]));	
		PyDict_SetItemString(sync_dict, ("quat"), py_quats); Py_DECREF(py_quats);

		py_obj = PyLong_FromLong(sync->health);
		PyDict_SetItemString(sync_dict, ("health"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->armour);
		PyDict_SetItemString(sync_dict, ("armour"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->weapon);
		PyDict_SetItemString(sync_dict, ("weapon"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->specialaction);
		PyDict_SetItemString(sync_dict, ("specialaction"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->surf_flags);
		PyDict_SetItemString(sync_dict, ("surf_flags"), py_obj); Py_DECREF(py_obj);

		PyObject *py_surfoffset = PyList_New(3);
		PyList_SetItem(py_surfoffset,0,PyFloat_FromDouble(sync->surfoffset[0]));
		PyList_SetItem(py_surfoffset,1,PyFloat_FromDouble(sync->surfoffset[1]));
		PyList_SetItem(py_surfoffset,2,PyFloat_FromDouble(sync->surfoffset[2]));
		PyDict_SetItemString(sync_dict, ("surf_offset"), py_surfoffset);

		py_obj = PyLong_FromLong(sync->anim);
		PyDict_SetItemString(sync_dict, ("anim"), py_obj); Py_DECREF(py_obj);
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
		PyObject *py_obj;

		py_obj = PyLong_FromLong(sync->playerid);
		PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->vehicleid);
		PyDict_SetItemString(sync_dict, ("vehicleid"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->leftright_keys);
		PyDict_SetItemString(sync_dict, ("leftright_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->updown_keys);
		PyDict_SetItemString(sync_dict, ("updown_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->keys);
		PyDict_SetItemString(sync_dict, ("keys"), py_obj); Py_DECREF(py_obj);

		PyObject *py_quats = PyList_New(4);
		PyList_SetItem(py_quats,0,PyFloat_FromDouble(sync->quat[0]));
		PyList_SetItem(py_quats,1,PyFloat_FromDouble(sync->quat[1]));
		PyList_SetItem(py_quats,2,PyFloat_FromDouble(sync->quat[2]));
		PyList_SetItem(py_quats,3,PyFloat_FromDouble(sync->quat[3]));	
		PyDict_SetItemString(sync_dict, ("quat"), py_quats);  Py_DECREF(py_quats);

		PyObject *py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItemString(sync_dict, ("pos"), py_pos); Py_DECREF(py_pos);


		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->vel[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->vel[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->vel[2]));
		PyDict_SetItemString(sync_dict, ("vel"), py_pos); Py_DECREF(py_pos);

		PyObject *py_vel = PyList_New(3);
		PyList_SetItem(py_vel ,0,PyFloat_FromDouble(sync->vel[0]));
		PyList_SetItem(py_vel ,1,PyFloat_FromDouble(sync->vel[1]));
		PyList_SetItem(py_vel ,2,PyFloat_FromDouble(sync->vel[2]));
		PyDict_SetItemString(sync_dict, ("vel"), py_vel); Py_DECREF(py_vel);

		py_obj = PyFloat_FromDouble(sync->health);
		PyDict_SetItemString(sync_dict, ("health"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->player_health);
		PyDict_SetItemString(sync_dict, ("player_health"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->player_armour);
		PyDict_SetItemString(sync_dict, ("player_armour"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->weapon);
		PyDict_SetItemString(sync_dict, ("weapon"), py_obj); Py_DECREF(py_obj);

		PyDict_SetItemString(sync_dict, ("siren"), sync->siren ? Py_True : Py_False);

		PyDict_SetItemString(sync_dict, ("landinggear_state"), sync->landinggear_state ? Py_True : Py_False);

		py_obj = PyFloat_FromDouble(sync->train_speed);
		PyDict_SetItemString(sync_dict, ("train_speed"), py_obj); Py_DECREF(py_obj);

		PyDict_SetItemString(sync_dict, ("hydra"), sync->hydra ? Py_True : Py_False);
		PyDict_SetItemString(sync_dict, ("trailer"), sync->trailer ? Py_True : Py_False);
		py_obj = PyLong_FromLong(sync->angle);
		PyDict_SetItemString(sync_dict, ("angle"), py_obj); Py_DECREF(py_obj);
		PyDict_SetItemString(sync_dict, ("train"), sync->train ? Py_True : Py_False);

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
		PyObject *py_obj;
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItemString(sync_dict, ("pos"), py_pos); Py_DECREF(py_pos);


		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->angle[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->angle[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->angle[2]));
		PyDict_SetItemString(sync_dict, ("angle"), py_pos); Py_DECREF(py_pos);

		py_obj = PyFloat_FromDouble(sync->z);
		PyDict_SetItemString(sync_dict, ("z"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->cam_mode);
		PyDict_SetItemString(sync_dict, ("cam_mode"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->cam_zoom);
		PyDict_SetItemString(sync_dict, ("cam_zoom"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->weapon_state);
		PyDict_SetItemString(sync_dict, ("weapon_state"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->unknown);
		PyDict_SetItemString(sync_dict, ("unknown"), py_obj); Py_DECREF(py_obj);

		if(!client_to_server) {
			py_obj = PyLong_FromLong(sync->player_id);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);
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
		PyObject *py_obj;

		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->origin[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->origin[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->origin[2]));
		PyDict_SetItemString(sync_dict, ("origin"), py_pos); Py_DECREF(py_pos);


		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->target[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->target[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->target[2]));
		PyDict_SetItemString(sync_dict, ("target"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->center[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->center[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->center[2]));
		PyDict_SetItemString(sync_dict, ("center"), py_pos); Py_DECREF(py_pos);


		py_obj = PyLong_FromLong(sync->id);
		PyDict_SetItemString(sync_dict, ("id"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->weapon);
		PyDict_SetItemString(sync_dict, ("weapon"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->type);
		PyDict_SetItemString(sync_dict, ("type"), py_obj); Py_DECREF(py_obj);


		if(!client_to_server) {
			py_obj = PyLong_FromLong(sync->playerid);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);
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
		PyObject *py_obj;

		PyList_SetItem(py_pos,0, PyFloat_FromDouble(sync->position[0]));
		PyList_SetItem(py_pos,1, PyFloat_FromDouble(sync->position[1]));
		PyList_SetItem(py_pos,2, PyFloat_FromDouble(sync->position[2]));
		PyDict_SetItemString(sync_dict, ("position"), py_pos); Py_DECREF(py_pos);


		if (sync->playerid != -1) {
			py_obj = PyLong_FromLong(sync->playerid);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);
		}

		py_obj = PyLong_FromLong(sync->vehicleid);
		PyDict_SetItemString(sync_dict, ("vehicleid"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->leftright_keys);
		PyDict_SetItemString(sync_dict, ("leftright_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->updown_keys);
		PyDict_SetItemString(sync_dict, ("updown_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->keys);
		PyDict_SetItemString(sync_dict, ("keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->currentweapon);
		PyDict_SetItemString(sync_dict, ("currentweapon"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->health);
		PyDict_SetItemString(sync_dict, ("player_health"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->armour);
		PyDict_SetItemString(sync_dict, ("player_armour"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->seat_flags);
		PyDict_SetItemString(sync_dict, ("seat_flags"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->driveby);
		PyDict_SetItemString(sync_dict, ("driveby"), py_obj); Py_DECREF(py_obj);
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
		PyObject *py_obj;

		if (sync->playerid != -1) {
			py_obj = PyLong_FromLong(sync->playerid);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj);  Py_DECREF(py_obj);
		}

		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->position[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->position[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->position[2]));
		PyDict_SetItemString(sync_dict, ("position"), py_pos); Py_DECREF(py_pos);
		
		py_obj = PyLong_FromLong(sync->leftright_keys);
		PyDict_SetItemString(sync_dict, ("leftright_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->updown_keys);
		PyDict_SetItemString(sync_dict, ("updown_keys"), py_obj); Py_DECREF(py_obj);

		py_obj = PyLong_FromLong(sync->keys);
		PyDict_SetItemString(sync_dict, ("keys"), py_obj); Py_DECREF(py_obj);

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
		PyObject *py_obj;

		if (sync->playerid != -1) {
			py_obj = PyLong_FromLong(sync->playerid);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);
		}

		py_obj = PyLong_FromLong(sync->vehicle_id);
		PyDict_SetItemString(sync_dict, ("vehicleid"), py_obj); Py_DECREF(py_obj);

		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->pos[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->pos[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->pos[2]));
		PyDict_SetItemString(sync_dict, ("position"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->velocity[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->velocity[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->velocity[2]));
		PyDict_SetItemString(sync_dict, ("velocity"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->roll[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->roll[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->roll[2]));
		PyDict_SetItemString(sync_dict, ("roll"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->direction[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->direction[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->direction[2]));
		PyDict_SetItemString(sync_dict, ("direction"), py_pos); Py_DECREF(py_pos);

		py_pos = PyList_New(3);
		PyList_SetItem(py_pos ,0,PyFloat_FromDouble(sync->turning_speed[0]));
		PyList_SetItem(py_pos ,1,PyFloat_FromDouble(sync->turning_speed[1]));
		PyList_SetItem(py_pos ,2,PyFloat_FromDouble(sync->turning_speed[2]));
		PyDict_SetItemString(sync_dict, ("turning_speed"), py_pos); Py_DECREF(py_pos);

		py_obj = PyFloat_FromDouble(sync->health);
		PyDict_SetItemString(sync_dict, ("health"), py_obj); Py_DECREF(py_obj);

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
		PyObject *py_obj;
		PyObject *py_list = PyList_New(sync->entries.size());
		for(int i=0;i<sync->entries.size();i++) {
			PyObject *sync_dict = PyDict_New();
			PyObject *py_pos = PyList_New(3);

			py_obj = PyLong_FromLong(sync->entries[i].x);
			PyList_SetItem(py_pos, 0, py_obj);
			py_obj = PyLong_FromLong(sync->entries[i].y);
			PyList_SetItem(py_pos,1, py_obj);
			py_obj = PyLong_FromLong(sync->entries[i].z);
			PyList_SetItem(py_pos,2, py_obj);
			

			PyDict_SetItemString(sync_dict, ("pos"), py_pos); Py_DECREF(py_pos);
			py_obj = PyLong_FromLong(sync->entries[i].playerid);
			PyDict_SetItemString(sync_dict, ("playerid"), py_obj); Py_DECREF(py_obj);
			PyDict_SetItemString(sync_dict, ("active"), sync->entries[i].active ? Py_True : Py_False);
			PyList_SetItem(py_list, i, sync_dict);
		}

		PyDict_SetItemString(dict, ("entries"), py_list); Py_DECREF(py_list);

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

		PyModule_AddIntConstant(module, "WEAPON_FISTS", 0);
		PyModule_AddIntConstant(module, "WEAPON_BRASSKNUCKLE", 1);
		PyModule_AddIntConstant(module, "WEAPON_GOLFCLUB", 2);
		PyModule_AddIntConstant(module, "WEAPON_NITESTICK", 3);
		PyModule_AddIntConstant(module, "WEAPON_KNIFE", 4);
		PyModule_AddIntConstant(module, "WEAPON_BAT", 5);
		PyModule_AddIntConstant(module, "WEAPON_SHOVEL", 6);
		PyModule_AddIntConstant(module, "WEAPON_POOLSTICK", 7);
		PyModule_AddIntConstant(module, "WEAPON_KATANA", 8);
		PyModule_AddIntConstant(module, "WEAPON_CHAINSAW", 9);
		PyModule_AddIntConstant(module, "WEAPON_DILDO", 10);
		PyModule_AddIntConstant(module, "WEAPON_DILDO2", 11);
		PyModule_AddIntConstant(module, "WEAPON_VIBRATOR", 12);
		PyModule_AddIntConstant(module, "WEAPON_VIBRATOR2", 13);
		PyModule_AddIntConstant(module, "WEAPON_FLOWER", 14);
		PyModule_AddIntConstant(module, "WEAPON_CANE", 15);
		PyModule_AddIntConstant(module, "WEAPON_GRENADE", 16);
		PyModule_AddIntConstant(module, "WEAPON_TEARGAS", 17);
		PyModule_AddIntConstant(module, "WEAPON_MOLTOV", 18);
		PyModule_AddIntConstant(module, "WEAPON_COLT45", 22);
		PyModule_AddIntConstant(module, "WEAPON_SILENCED", 23);
		PyModule_AddIntConstant(module, "WEAPON_DEAGLE", 24);
		PyModule_AddIntConstant(module, "WEAPON_SHOTGUN", 25);
		PyModule_AddIntConstant(module, "WEAPON_SAWEDOFF", 26);
		PyModule_AddIntConstant(module, "WEAPON_SHOTGSPA", 27);
		PyModule_AddIntConstant(module, "WEAPON_UZI", 28);
		PyModule_AddIntConstant(module, "WEAPON_MP5", 29);
		PyModule_AddIntConstant(module, "WEAPON_AK47", 30);
		PyModule_AddIntConstant(module, "WEAPON_M4", 31);
		PyModule_AddIntConstant(module, "WEAPON_TEC9", 32);
		PyModule_AddIntConstant(module, "WEAPON_RIFLE", 33);
		PyModule_AddIntConstant(module, "WEAPON_SNIPER", 34);
		PyModule_AddIntConstant(module, "WEAPON_ROCKETLAUNCHER", 35);
		PyModule_AddIntConstant(module, "WEAPON_HEATSEEKER", 36);
		PyModule_AddIntConstant(module, "WEAPON_FLAMETHROWER", 37);
		PyModule_AddIntConstant(module, "WEAPON_MINIGUN", 38);
		PyModule_AddIntConstant(module, "WEAPON_SATCHEL", 39);
		PyModule_AddIntConstant(module, "WEAPON_BOMB", 40);
		PyModule_AddIntConstant(module, "WEAPON_SPRAYCAN", 41);
		PyModule_AddIntConstant(module, "WEAPON_FIREEXTINGUISHER", 42);
		PyModule_AddIntConstant(module, "WEAPON_CAMERA", 43);
		PyModule_AddIntConstant(module, "WEAPON_PARACHUTE", 46);
		PyModule_AddIntConstant(module, "WEAPON_VEHICLE", 49);
		PyModule_AddIntConstant(module, "WEAPON_DROWN", 53);
		PyModule_AddIntConstant(module, "WEAPON_COLLISION", 54);



		PyModule_AddIntConstant(module, "KEY_ACTION", 1);
		PyModule_AddIntConstant(module, "KEY_CROUCH", 2);
		PyModule_AddIntConstant(module, "KEY_FIRE", 4);
		PyModule_AddIntConstant(module, "KEY_SPRINT", 8);
		PyModule_AddIntConstant(module, "KEY_SECONDARY_ATTACK", 16);
		PyModule_AddIntConstant(module, "KEY_JUMP", 32);
		PyModule_AddIntConstant(module, "KEY_LOOK_RIGHT", 64);
		PyModule_AddIntConstant(module, "KEY_HANDBRAKE", 128);
		PyModule_AddIntConstant(module, "KEY_LOOK_LEFT", 256);
		PyModule_AddIntConstant(module, "KEY_SUBMISSION", 512);
		PyModule_AddIntConstant(module, "KEY_LOOK_BEHIND", 512);
		PyModule_AddIntConstant(module, "KEY_WALK", 1024);
		PyModule_AddIntConstant(module, "KEY_ANALOG_UP", 2048);
		PyModule_AddIntConstant(module, "KEY_ANALOG_DOWN", 4096);
		PyModule_AddIntConstant(module, "KEY_ANALOG_LEFT", 8192);
		PyModule_AddIntConstant(module, "KEY_ANALOG_RIGHT", 16384);
		PyModule_AddIntConstant(module, "KEY_YES", 65536);
		PyModule_AddIntConstant(module, "KEY_NO", 131072);
		PyModule_AddIntConstant(module, "KEY_CTRL_BACK", 262144);

		PyModule_AddIntConstant(module, "KEY_UP", -128);
		PyModule_AddIntConstant(module, "KEY_DOWN", 128);
		PyModule_AddIntConstant(module, "KEY_LEFT", -128);
		PyModule_AddIntConstant(module, "KEY_RIGHT", 128);

		PyModule_AddIntConstant(module, "DIALOG_STYLE_MSGBOX", 0);
		PyModule_AddIntConstant(module, "DIALOG_STYLE_INPUT", 1);
		PyModule_AddIntConstant(module, "DIALOG_STYLE_LIST", 2);
		PyModule_AddIntConstant(module, "DIALOG_STYLE_PASSWORD", 3);
		PyModule_AddIntConstant(module, "DIALOG_STYLE_TABLIST", 4);
		PyModule_AddIntConstant(module, "DIALOG_STYLE_TABLIST_HEADERS", 5);



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