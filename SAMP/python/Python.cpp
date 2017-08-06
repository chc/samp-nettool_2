#include <stdio.h>
#include <Python.h>

#include "SAMP/SAMPClient.h"

#include "Python.h"
#include "PySAMPServer.h"
#include "PySAMPClient.h"
#include "PySAMPRPC.h"
#include "PySAMPSync.h"
#include "SAMP/SAMPAuth.h"
#include "SAMP/SAMPSync.h"
#include <vector>
#include <algorithm>

#include <StringCompressor.h>

namespace SAMP {
	void dump_raknet_bitstream(RakNet::BitStream *stream, const char *fmt, ...);
}

std::vector<gs_SAMPServer *> m_py_servers;
std::vector<gs_SAMPClient *> m_py_clients;
PyObject *nf_gen_gpci_method(PyObject *self, PyObject *args);
static PyMethodDef SAMPMethods[] = {
	{"GenGPCI", nf_gen_gpci_method, METH_VARARGS, "Generates a GPCI"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


static struct PyModuleDef samp_module = {
   PyModuleDef_HEAD_INIT,
   "SAMP",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   SAMPMethods
};

PyMODINIT_FUNC PyInit_SAMP(void) {
	if(!PySAMP_ServerReady())
		return NULL;
	if(!PySAMP_ClientReady())
		return NULL;
	PyObject *m = PyModule_Create(&samp_module);
	
	PySAMP_InitServerObj(m);

	PySAMP_InitClientObj(m);

	PySAMP_AddRPCDefs(m);
	Py::PySAMP_AddPacketDefs(m);

	return m;
}

namespace Py {
	void Init() {
		StringCompressor::AddReference();

		printf("PyVer: %s\n", Py_GetVersion());

		PyImport_AppendInittab("SAMP", PyInit_SAMP);
	
		//Py_SetProgramName(L"scripts");
		
		//Py_SetPythonHome(L"E:\\Code\\Python-3.5.0"); //temp windows fix
		Py_SetPath(L"D:\\Python36\\python35_d.zip;D:\\Python36\\DLLs;D:\\Python36\\lib;D:\\Code\\nettool_2\\scripts");

		//Py_SetPath(L"/usr/lib/python35.zip:/usr/lib/python3.5:/usr/lib/python3.5/plat-x86_64-linux-gnu:/usr/lib/python3.5/lib-dynload:/usr/local/lib/python3.5/dist-packages:/usr/lib/python3/dist-packages:/home/nettool/nettool_2/scripts");
		//Py_SetPath(L"scripts");
		
		
		Py_Initialize();
		

		PyEval_InitThreads();

		const char *path = "scripts/__init__.py";
		FILE *fd = fopen(path, "r");
		PyRun_AnyFile(fd, path);
		fclose(fd);		
	}
	void Tick() {
		Py_BEGIN_ALLOW_THREADS
		//maybe this should be somewhere else
		std::vector<gs_SAMPClient *> py_clients_cpy = m_py_clients;
		std::vector<gs_SAMPClient *>::iterator it = py_clients_cpy.begin();
		while(it != py_clients_cpy.end()) {
			gs_SAMPClient *client = *it;
			if(client->samp_client)
				client->samp_client->think(NULL);
			it++;
		}
		Py_END_ALLOW_THREADS
	}

	void PySAMP_CheckAndPrintErrors() {
		PyObject *ptype, *pvalue, *ptraceback;
		PyObject* exceptionType = PyErr_Occurred();
		PyObject* pRepr;
		if(exceptionType) {
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);
			if(ptype != NULL) {
				pRepr = PyObject_Repr(ptype);
				wprintf(L"%s\n", PyUnicode_AsWideCharString(pRepr, NULL));
				Py_DECREF(pRepr);
			}
			if(pvalue != NULL) {
				pRepr = PyObject_Repr(pvalue);
				wprintf(L"%s\n", PyUnicode_AsWideCharString(pRepr, NULL));
				Py_DECREF(pRepr);
			}
			if(ptraceback != NULL) {
				pRepr = PyObject_Repr(ptraceback);
				wprintf(L"%s\n", PyUnicode_AsWideCharString(ptraceback, NULL));
				Py_DECREF(pRepr);
			}
			PyErr_Clear();
		}
	}
	EConnRejectReason OnNewConnection(SAMP::Server *server, SAMP::Client *client, std::string password) {
		EConnRejectReason response;
		std::vector<gs_SAMPServer *>::iterator it = m_py_servers.begin();
		while(it != m_py_servers.end()) {
			gs_SAMPServer *serv = *it;
			if(serv->samp_server == server) {
				gs_SAMPClient *conn_obj = (gs_SAMPClient *)PyObject_CallObject((PyObject *) &gs_SAMPClientType, NULL);
				PySAMP_CheckAndPrintErrors();

				conn_obj->samp_server = server;
				conn_obj->samp_client = client;				

				PyObject *arglist = Py_BuildValue("OOs",serv, conn_obj, password.c_str());
				PyObject *ret = (PyObject *)PyObject_CallObject(serv->mp_new_conn_hndlr, arglist);
				PySAMP_CheckAndPrintErrors();

				response = (EConnRejectReason)PyLong_AsLong(ret);


				if(std::find(m_py_clients.begin(),m_py_clients.end(), conn_obj) == m_py_clients.end()) {
					m_py_clients.push_back(conn_obj);
				}
				else {
					Py_DECREF(conn_obj); //shouldn't happen
				}

				Py_XDECREF(ret);
				Py_DECREF(arglist);
			}
			it++;
		}
		return response;
	}
	void OnClientDisconnect(SAMP::Server *server, SAMP::Client *client, EConnRejectReason reason) {
		std::vector<gs_SAMPClient *>::iterator it2 = m_py_clients.begin();		
		while(it2 != m_py_clients.end()) {
			gs_SAMPClient *samp_client = *it2;
			if(samp_client->samp_client == client) {
				PyObject *arglist = Py_BuildValue("OI",samp_client, (int)reason);
				PyObject *ret = (PyObject *)PyObject_CallObject(samp_client->mp_disconnect_handler, arglist);
				PySAMP_CheckAndPrintErrors();
				Py_DECREF(arglist);
				Py_XDECREF(ret);
				break;
			}
			it2++;
		}
	}
	void OnClientDelete(SAMP::Server *server, SAMP::Client *client) {
		std::vector<gs_SAMPClient *>::iterator it2 = m_py_clients.begin();		
		while(it2 != m_py_clients.end()) {
			gs_SAMPClient *samp_client = *it2;
			if(samp_client->samp_client == client) {
				//Py_DECREF(samp_client);
				m_py_clients.erase(it2);
				break;
			}
			it2++;
		}
	}
	wchar_t *copyPythonString(PyObject *string) {
		if(string) {
			if(PyUnicode_Check(string)) {
				wchar_t *strval = PyUnicode_AsWideCharString(string, NULL);
				wchar_t *retval = wcsdup(strval);
				PyMem_Free(strval);
				return retval;
			}
		}
		return NULL;
	}
	void OnGotRPC(SAMP::Client *client, uint32_t rpc_id, RakNet::BitStream *data, bool client_to_server) {
		RPCNameMap *rpc_map = GetRPCNameMapByID(rpc_id);
		/*
		if(client_to_server) {
			printf("C->S Got RPC %d(%s) - %d (%d)\n",rpc_id, ((rpc_map && rpc_map->name) ? rpc_map->name : NULL), data->GetNumberOfBitsUsed(), data->GetNumberOfBytesUsed());
		} else {
			printf("S->C Got RPC %d(%s) - %d (%d)\n",rpc_id, ((rpc_map && rpc_map->name) ? rpc_map->name : NULL), data->GetNumberOfBitsUsed(), data->GetNumberOfBytesUsed());
		}its 
		*/
		if(!rpc_map) {
			static int i = 0;
			SAMP::dump_raknet_bitstream(data, "%c_rpc_%d_%d.bin", client_to_server ? 'C' : 'S', rpc_id,i++);
		}
		assert(rpc_map);

		std::vector<gs_SAMPClient *> py_clients_cpy = m_py_clients;
		std::vector<gs_SAMPClient *>::iterator it = py_clients_cpy.begin();
		while(it != py_clients_cpy.end()) {
			gs_SAMPClient *py_client = *it;
			if(py_client->samp_client == client) {
				PyObject* py_dict = RPCToPyDict(rpc_map, data, client_to_server);
				PyObject *arglist = Py_BuildValue("OiN",py_client, rpc_id, py_dict);
				PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_rpc_handler, arglist);
				PySAMP_CheckAndPrintErrors();

				//Py_DECREF(py_dict);
				Py_DECREF(arglist);
				Py_XDECREF(ret);
			}
			it++;
		}
	}
	void OnConnectionAccepted(SAMP::Client *client, int playerid, uint32_t challenge) {
		std::vector<gs_SAMPClient *>::iterator it = m_py_clients.begin();
		while(it != m_py_clients.end()) {
			gs_SAMPClient *py_client = *it;
			if(py_client->samp_client == client) {
				PyObject *arglist = Py_BuildValue("OII",py_client, playerid, challenge);
				PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_conn_accepted_handler, arglist);
				PySAMP_CheckAndPrintErrors();

				Py_DECREF(arglist);
				Py_XDECREF(ret);
			}
			it++;
		}
	}
	void OnGotSync(SAMP::Client *client, SAMP::PacketEnumeration type, RakNet::BitStream *data, bool client_to_server) {
		SAMP::PLAYER_SYNC_INFO player_sync;
		SAMP::VEHICLE_SYNC_INFO vehicle_sync;
		SAMP::SAMPBulletSync bullet_sync;
		SAMP::SAMPAimSync aim_sync;
		SAMP::PASSENGER_SYNC_DATA passenger_sync;
		SAMP::SPECTATOR_SYNC_DATA spectator_sync;
		SAMP::SAMPUnoccupiedVehData unoccupied_sync;
		SAMP::SAMPMarkerSync markers_sync;
		PyObject *dict = NULL;
		switch(type) {
			case SAMP::ID_PLAYER_SYNC:
				ReadPlayerSync(&player_sync, data, client_to_server);
				dict = SyncToPyDict_Player(&player_sync, client_to_server);
				break;
			case SAMP::ID_VEHICLE_SYNC:
				ReadVehicleSync(&vehicle_sync, data, client_to_server);
				dict = SyncToPyDict_Vehicle(&vehicle_sync, client_to_server);
				break;
			case SAMP::ID_PASSENGER_SYNC:
				ReadPassengerSync(&passenger_sync, data, client_to_server);
				dict = SyncToPyDict_Passenger(&passenger_sync, client_to_server);
				break;
			case SAMP::ID_AIM_SYNC:
				ReadAimSync(&aim_sync, data, client_to_server);
				dict = SyncToPyDict_Aim(&aim_sync, client_to_server);
				break;
			case SAMP::ID_BULLET_SYNC:
				ReadBulletSync(&bullet_sync, data, client_to_server);
				dict = SyncToPyDict_Bullet(&bullet_sync, client_to_server);
				break;
			case SAMP::ID_SPECTATOR_SYNC:
				ReadSpectatorSync(&spectator_sync, data, client_to_server);
				dict = SyncToPyDict_Spectator(&spectator_sync, client_to_server);
				break;
			case SAMP::ID_UNOCCUPIED_SYNC:
				ReadUnoccupiedSync(&unoccupied_sync, data, client_to_server);
				dict = SyncToPyDict_UnoccupiedVeh(&unoccupied_sync, client_to_server);
				break;
			case SAMP::ID_MARKERS_SYNC:
				//SAMP::dump_raknet_bitstream(data, "markers_in.bin");
				ReadMarkerSync(&markers_sync, data, client_to_server);
				dict = SyncToPyDict_Marker(&markers_sync, client_to_server);
				break;
			default:
				return;
		}
		gs_SAMPClient *py_client = GetPyClient(client);
		if (py_client == NULL) {
			Py_XDECREF(dict);
			return;
		}
		PyObject *arglist = Py_BuildValue("OIN",py_client, (int)type, dict);
		PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_sync_handler, arglist);
		PySAMP_CheckAndPrintErrors();
		//Py_DECREF(dict);
		Py_DECREF(arglist);
		Py_XDECREF(ret);
	}
	void OnGotStatsUpdate(SAMP::Client *client, int32_t money, int32_t drunk) {
		gs_SAMPClient *py_client = GetPyClient(client);
		if(py_client == NULL)
			return;
		PyObject *arglist = Py_BuildValue("OII",py_client, (int)money, drunk);
		PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_stats_update_handler, arglist);
		PySAMP_CheckAndPrintErrors();
		Py_DECREF(arglist);
		Py_XDECREF(ret);
	}

	struct WepData{
		uint8_t wep_id;
		uint16_t ammo;
		uint8_t weap_unk;
	} _WepData;
	void OnGotWeaponsUpdate(SAMP::Client *client, RakNet::BitStream *data, bool client_to_server) {
		//there is no server to client so no need to check
		PyObject *list;
		PyObject *py_obj;
		uint32_t unk;
		PyObject *dict = PyDict_New();
		
		data->Read(unk);

		py_obj = PyLong_FromUnsignedLong(unk);
		PyDict_SetItemString(dict, ("unk"), py_obj); Py_DECREF(py_obj);


		std::vector<WepData> weps;
		while(data->GetNumberOfUnreadBits() > 0) {
			data->Read(_WepData.wep_id);
			data->Read(_WepData.ammo);
			data->Read(_WepData.weap_unk);
			weps.push_back(_WepData);
		}
		list = PyList_New(weps.size());
		for(int i=0;i<weps.size();i++) {
			PyObject *item_dict = PyDict_New();
			py_obj = PyLong_FromUnsignedLong(weps[i].wep_id);
			PyDict_SetItemString(item_dict, ("weapon"), py_obj); Py_DECREF(py_obj);
			py_obj = PyLong_FromUnsignedLong(weps[i].ammo);
			PyDict_SetItemString(item_dict, ("ammo"), py_obj); Py_DECREF(py_obj);
			py_obj = PyLong_FromUnsignedLong(weps[i].weap_unk);
			PyDict_SetItemString(item_dict, ("unk"), py_obj); Py_DECREF(py_obj);
			PyList_SetItem(list, i, item_dict);
		}

		PyDict_SetItemString(dict, ("weapons"), list); Py_DECREF(list);

		gs_SAMPClient *py_client = GetPyClient(client);
		if (py_client == NULL) {
			Py_DECREF(dict);
			return;
		}
			

		PyObject *arglist = Py_BuildValue("ON",py_client, dict);
		PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_weapons_update_handler, arglist);

		Py_DECREF(arglist);
		Py_XDECREF(ret);
	}
	gs_SAMPClient *GetPyClient(SAMP::Client *client) {
		std::vector<gs_SAMPClient *>::iterator it = m_py_clients.begin();
		while(it != m_py_clients.end()) {
			gs_SAMPClient *py_client = *it;
			if(py_client->samp_client == client) {
				return py_client;
			}
			it++;
		}
		return NULL;
	}
}

PyObject *nf_gen_gpci_method(PyObject *self, PyObject *args) {
	unsigned long factor = 0;
	char gpci[4*16] = {0};

	PyObject *src_obj;
	if(!PyArg_ParseTuple(args, "IO", &factor, &src_obj)) {
		Py_RETURN_NONE;
	}
	wchar_t *wstr = Py::copyPythonString(src_obj);
	char *str = (char *)malloc(wcslen(wstr)+1);
	memset(str,0, wcslen(wstr)+1);
	wcstombs(str, wstr, wcslen(wstr));
	gen_gpci(gpci, factor, str);

	free(wstr);
	free(str);
	
	return PyUnicode_FromString(gpci);
}