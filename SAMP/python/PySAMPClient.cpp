
#include "main.h"
#include "PySAMPClient.h"
#include "SAMP/SAMPClient.h"

#include "SAMP/SAMPSync.h"
#include "PySAMPRPC.h"
#include "PySAMPSync.h"

#include "SelectNetEventManager.h"

#include "structmember.h"

namespace SAMP {
	void dump_raknet_bitstream(RakNet::BitStream *stream, const char *fmt, ...);
}

extern Net::SelectNetEventManager* g_event_mgr;
PyMethodDef SAMPClient_methods[] = {
    								{"SendRPC",  (PyCFunction)pyi_sampclient_sendrpc, METH_VARARGS,
 								    "Sends an RPC"},
    								{"SendSync",  (PyCFunction)pyi_sampclient_sendsync, METH_VARARGS,
 								    "Sends a Sync packet"},
    								{"SendStatsUpdate",  (PyCFunction)pyi_sampclient_send_stats_update, METH_VARARGS,
 								    "Sends a stats update"},
    								{"SendWeaponData",  (PyCFunction)pyi_sampclient_send_weapons_update, METH_VARARGS,
 								    "Sends a stats update"},
									{"Disconnect", (PyCFunction)pyi_sampclient_disconnect, METH_VARARGS,
 								    "Disconnects and cleans up the client"},
									{"Connect", (PyCFunction)pyi_sampclient_connect, METH_VARARGS,
 								    "Connects the client"},
    								{NULL, NULL, 0, NULL}};


//JSON Server
PyTypeObject gs_SAMPClientType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "Client",             /*tp_name*/
    sizeof(gs_SAMPClient), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "SAMP Client Object",       /* tp_doc */
};

static PyMemberDef SAMPClient_members[9] = {
    {"proxy_connection", T_OBJECT, offsetof(gs_SAMPClient, mp_proxy_connection), 0,"proxy connection object"},
	{"source_connection", T_OBJECT, offsetof(gs_SAMPClient, mp_source_connection), 0,"source connection object"},
	{"rpc_handler", T_OBJECT, offsetof(gs_SAMPClient, mp_rpc_handler), 0,"rpc handler function"},
	{"sync_handler", T_OBJECT, offsetof(gs_SAMPClient, mp_sync_handler), 0,"rpc handler function"},
	{"conn_accepted_handler", T_OBJECT, offsetof(gs_SAMPClient, mp_conn_accepted_handler), 0,"connection accepted handler function"},
	{"stats_update_handler", T_OBJECT, offsetof(gs_SAMPClient, mp_stats_update_handler), 0,"stats updatehandler function"},
	{"weapons_update_handler", T_OBJECT, offsetof(gs_SAMPClient, mp_weapons_update_handler), 0,"weapon update handler callback function"},
	{"context", T_OBJECT, offsetof(gs_SAMPClient, mp_context), 0,"stats updatehandler function"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

extern std::vector<gs_SAMPClient *> m_py_clients;


PyObject * SAMPClient_New(PyTypeObject *type, PyObject *args, PyObject *kwd) {
	gs_SAMPClient *obj = (gs_SAMPClient *)PyType_GenericNew(type, args, kwd);
	obj->samp_server = NULL;
	obj->samp_client = NULL;
	obj->mp_rpc_handler = NULL;
	obj->mp_sync_handler = NULL;
	obj->mp_proxy_connection = NULL;

	m_py_clients.push_back(obj);
	return (PyObject *)obj;
}
bool PySAMP_ClientReady() {
	gs_SAMPClientType.tp_methods = SAMPClient_methods;

	gs_SAMPClientType.tp_new = SAMPClient_New;
	gs_SAMPClientType.tp_alloc = PyType_GenericAlloc;
	gs_SAMPClientType.tp_members = SAMPClient_members;

	if (PyType_Ready(&gs_SAMPClientType) < 0)
		return false;
	return true;
}
void PySAMP_InitClientObj(PyObject *m) {

	Py_INCREF(&gs_SAMPClientType);
	PyModule_AddObject(m, "Client", (PyObject *)&gs_SAMPClientType);
}

PyObject *pyi_sampclient_sendrpc(gs_SAMPClient *self, PyObject *args) {
	int rpc_id;
	PyObject *send_dict;
	if (!PyArg_ParseTuple(args, "IO",&rpc_id, &send_dict))
        Py_RETURN_NONE;
	RPCNameMap *map = GetRPCNameMapByID(rpc_id);
	if(!map)
		Py_RETURN_NONE;

	RakNet::BitStream *out_bs = new RakNet::BitStream;

	bool send_to_server = !self->samp_client->GetInbound();

	PyRPCDictToBitStream(map, out_bs, send_dict, send_to_server);
	self->samp_client->SendRPC(rpc_id, out_bs);

	delete out_bs;
	Py_RETURN_NONE;
}
PyObject *pyi_sampclient_disconnect(gs_SAMPClient *self, PyObject *args) {
	Py_RETURN_NONE;
}
PyObject *pyi_sampclient_connect(gs_SAMPClient *self, PyObject *args) {
	const char *str;
	if (!PyArg_ParseTuple(args, "s", &str))
        Py_RETURN_NONE;
	self->samp_client = new SAMP::Client(str);
	g_event_mgr->addNetworkClient(self->samp_client);
	Py_RETURN_NONE;
}
PyObject *pyi_sampclient_sendsync(gs_SAMPClient *self, PyObject *args) {
	int msg_id;
	PyObject *send_dict;

	if (!PyArg_ParseTuple(args, "IO",&msg_id, &send_dict))
        Py_RETURN_NONE;

	

	bool send_to_server = !self->samp_client->GetInbound();

	RakNet::BitStream *out_bs = new RakNet::BitStream;

	SAMP::PLAYER_SYNC_INFO player_sync;
	SAMP::VEHICLE_SYNC_INFO vehicle_sync;
	SAMP::SAMPBulletSync bullet_sync;
	SAMP::SAMPAimSync aim_sync;
	SAMP::PASSENGER_SYNC_DATA passenger_sync;
	SAMP::SPECTATOR_SYNC_DATA spectator_sync;
	SAMP::SAMPUnoccupiedVehData unoccupied_data;
	SAMP::SAMPMarkerSync markers_sync;

	switch(msg_id) {
		case SAMP::ID_PLAYER_SYNC:
			Py::PyDictToSync_Player(&player_sync, send_dict, send_to_server);
			SAMP::WritePlayerSync(&player_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_VEHICLE_SYNC:
			Py::PyDictToSync_Vehicle(&vehicle_sync, send_dict, send_to_server);
			SAMP::WriteVehicleSync(&vehicle_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_BULLET_SYNC:
			Py::PyDictToSync_Bullet(&bullet_sync, send_dict, send_to_server);
			SAMP::WriteBulletSync(&bullet_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_AIM_SYNC:
			Py::PyDictToSync_Aim(&aim_sync, send_dict, send_to_server);
			SAMP::WriteAimSync(&aim_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_PASSENGER_SYNC:
			Py::PyDictToSync_Passenger(&passenger_sync, send_dict, send_to_server);
			SAMP::WritePassengerSync(&passenger_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_SPECTATOR_SYNC:
			Py::PyDictToSync_Spectator(&spectator_sync, send_dict, send_to_server);
			SAMP::WriteSpectatorSync(&spectator_sync,out_bs, send_to_server);
			break;
		case SAMP::ID_UNOCCUPIED_SYNC:
			Py::PyDictToSync_UnoccupiedVeh(&unoccupied_data, send_dict, send_to_server);
			SAMP::WriteUnoccupiedSync(&unoccupied_data, out_bs, send_to_server);
			break;
		case SAMP::ID_MARKERS_SYNC:
			Py::PyDictToSync_Marker(&markers_sync, send_dict, send_to_server);
			SAMP::WriteMarkerSync(&markers_sync, out_bs, send_to_server);
			//SAMP::dump_raknet_bitstream(out_bs, "markers_out.bin");
			break;
	}
	out_bs->ResetReadPointer();
	self->samp_client->SendMessage(msg_id, out_bs);

	delete out_bs;
	Py_RETURN_NONE;
}
PyObject *pyi_sampclient_send_stats_update(gs_SAMPClient *self, PyObject *args) {
	int money, drunk;

	if (!PyArg_ParseTuple(args, "II",&money, &drunk))
        Py_RETURN_NONE;


	RakNet::BitStream *out_bs = new RakNet::BitStream;
	out_bs->Write((int32_t)money);
	out_bs->Write((int32_t)drunk);
	out_bs->ResetReadPointer();

	self->samp_client->SendMessage(SAMP::ID_STATS_UPDATE, out_bs);

	delete out_bs;
	Py_RETURN_NONE;
}


PyObject *pyi_sampclient_send_weapons_update(gs_SAMPClient *self, PyObject *args) {
	PyObject *send_dict, *dict_item, *list;
	if (!PyArg_ParseTuple(args, "O",&send_dict))
        Py_RETURN_NONE;


	RakNet::BitStream *out_bs = new RakNet::BitStream;
	
	dict_item = PyDict_GetItemString(send_dict, "unk");
	out_bs->Write((uint32_t)(dict_item ? PyLong_AsUnsignedLong(dict_item) : 0xFFFFFFFF));

	list = PyDict_GetItemString(send_dict, "weapons");

	int size = PyList_Size(list);

	for(int i=0;i<size;i++) {
		PyObject *list_item = PyList_GET_ITEM(list, i);

		dict_item = PyDict_GetItemString(list_item, "weapon");
		out_bs->Write((uint8_t)PyLong_AsUnsignedLong(dict_item));

		dict_item = PyDict_GetItemString(list_item, "ammo");
		out_bs->Write((uint16_t)PyLong_AsUnsignedLong(dict_item));

		dict_item = PyDict_GetItemString(list_item, "unk");
		out_bs->Write((uint8_t)PyLong_AsUnsignedLong(dict_item));
	}
	out_bs->ResetReadPointer();
	self->samp_client->SendMessage(SAMP::ID_WEAPONS_UPDATE, out_bs);

	delete out_bs;
	Py_RETURN_NONE;
}
