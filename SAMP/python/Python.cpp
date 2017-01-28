#include <stdio.h>
#include <Python.h>

#include "SAMP\SAMPClient.h"

#include "Python.h"
#include "PySAMPServer.h"
#include "PySAMPClient.h"
#include "PySAMPRPC.h"
#include "PySAMPSync.h"
#include "SAMP\SAMPAuth.h"
#include "SAMP\SAMPSync.h"
#include <vector>
#include <algorithm>

#include <StringCompressor.h>
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
	
		Py_SetPythonHome(L"E:\\Code\\Python-3.5.0"); //temp windows fix
		Py_Initialize();

		const char *path = "scripts//__init__.py";
		FILE *fd = fopen(path, "rb");
		PyRun_AnyFile(fd, path);
		fclose(fd);
	
	}
	void Tick() {
		//maybe this should be somewhere else
		std::vector<gs_SAMPClient *> py_clients_cpy = m_py_clients;
		std::vector<gs_SAMPClient *>::iterator it = py_clients_cpy.begin();
		while(it != py_clients_cpy.end()) {
			gs_SAMPClient *client = *it;
			if(client->samp_client)
				client->samp_client->think(NULL);
			it++;
		}
	}

	void PySAMP_CheckAndPrintErrors() {
		PyObject *ptype, *pvalue, *ptraceback;
		if(PyErr_Occurred()) {
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);
			printf("%s\n", PyBytes_AsString(ptype));
			printf("%s\n", PyBytes_AsString(pvalue));
			printf("%s\n", PyBytes_AsString(ptraceback));

			PyErr_Clear();
		}
	}
	void OnNewConnection(SAMP::Server *server, SAMP::Client *client) {
		std::vector<gs_SAMPServer *>::iterator it = m_py_servers.begin();
		while(it != m_py_servers.end()) {
			gs_SAMPServer *serv = *it;
			if(serv->samp_server == server) {
				gs_SAMPClient *conn_obj = (gs_SAMPClient *)PyObject_CallObject((PyObject *) &gs_SAMPClientType, NULL);
				PySAMP_CheckAndPrintErrors();

				conn_obj->samp_server = server;
				conn_obj->samp_client = client;				

				PyObject *arglist = Py_BuildValue("OO",serv, conn_obj);
				PyObject *ret = (PyObject *)PyObject_CallObject(serv->mp_new_conn_hndlr, arglist);
				PySAMP_CheckAndPrintErrors();


				if(std::find(m_py_clients.begin(),m_py_clients.end(), conn_obj) == m_py_clients.end())
					m_py_clients.push_back(conn_obj);

				Py_XDECREF(ret);
				Py_DECREF(arglist);
			}
			it++;
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
		if(!rpc_map)
			return;
		std::vector<gs_SAMPClient *> py_clients_cpy = m_py_clients;
		std::vector<gs_SAMPClient *>::iterator it = py_clients_cpy.begin();
		while(it != py_clients_cpy.end()) {
			gs_SAMPClient *py_client = *it;
			if(py_client->samp_client == client) {
				PyObject* py_dict = RPCToPyDict(rpc_map, data, client_to_server);
				PyObject *arglist = Py_BuildValue("OiO",py_client, rpc_id, py_dict);
				PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_rpc_handler, arglist);
				PySAMP_CheckAndPrintErrors();

				Py_DECREF(py_dict);
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
			case SAMP::ID_AIM_SYNC:
				ReadAimSync(&aim_sync, data, client_to_server);
				dict = SyncToPyDict_Aim(&aim_sync, client_to_server);
				break;
			case SAMP::ID_BULLET_SYNC:
				ReadBulletSync(&bullet_sync, data, client_to_server);
				dict = SyncToPyDict_Bullet(&bullet_sync, client_to_server);
				break;
			default:
				return;
		}
		gs_SAMPClient *py_client = GetPyClient(client);
		if(py_client == NULL)
			return;
		PyObject *arglist = Py_BuildValue("OIO",py_client, (int)type, dict);
		PyObject *ret = (PyObject *)PyObject_CallObject(py_client->mp_sync_handler, arglist);
		PySAMP_CheckAndPrintErrors();
		Py_DECREF(dict);
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
	if(!PyArg_ParseTuple(args, "I", &factor)) {
		Py_RETURN_NONE;
	}
	gen_gpci(gpci, factor);
	
	return PyUnicode_FromString(gpci);
}