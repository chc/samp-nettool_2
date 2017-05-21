#ifndef _PYSAMPCLIENT_H
#define _PYSAMPCLIENT_H
#include <Python.h>
#include "PySAMPServer.h"
PyMODINIT_FUNC PyInit_SAMPClient(void);
namespace SAMP {
	class Server;
	class Client;
}
typedef struct {
    PyObject_HEAD
	PyObject *mp_rpc_handler; //S->C rpc hndlr
	PyObject *mp_sync_handler;//S->C sync hndlr
	PyObject *mp_conn_accepted_handler;//S->C sync hndlr
	PyObject *mp_stats_update_handler; //C->S stats hndlr
	PyObject *mp_weapons_update_handler; //C->S weapon update hndlr
	PyObject *mp_disconnect_handler;
	PyObject *mp_proxy_connection;
	PyObject *mp_source_connection;
	PyObject *mp_context; //just a custom object you can add anything to
	

	SAMP::Server *samp_server;
	SAMP::Client *samp_client;
	
} gs_SAMPClient;

extern PyTypeObject gs_SAMPClientType;

bool PySAMP_ClientReady();
void PySAMP_InitClientObj(PyObject *m);

void PySAMP_ClientDelete(PyObject *m);

PyObject *pyi_sampclient_sendrpc(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_sendsync(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_sendreject(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_connect(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_disconnect(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_send_stats_update(gs_SAMPClient *self, PyObject *args);
PyObject *pyi_sampclient_send_weapons_update(gs_SAMPClient *self, PyObject *args);

#endif //_PYSAMPSERVER_H