#ifndef _PYSAMPSERVER_H
#define _PYSAMPSERVER_H
#include <Python.h>
PyMODINIT_FUNC PyInit_SAMPServer(void);
namespace SAMP {
	class Server;
}
typedef struct {
    PyObject_HEAD
	PyObject *mp_new_conn_hndlr;
	PyObject *mp_context;

	PyObject *mp_rpc_hndlr; //C->S rpc hndlr
	PyObject *mp_sync_hndlr;//C->S sync hndlr
	SAMP::Server *samp_server;
} gs_SAMPServer;


bool PySAMP_ServerReady();
void PySAMP_InitServerObj(PyObject *m);

PyObject *pyi_sampserver_listen(gs_SAMPServer *self, PyObject *args);
#endif //_PYSAMPSERVER_H