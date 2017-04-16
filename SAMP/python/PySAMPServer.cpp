
#include "main.h"
#include "PySAMPServer.h"
#include "PySAMPClient.h"
#include "SAMP/SAMPServer.h"
#include "SAMP/SAMPClient.h"
#include "SAMP/python/Python.h"

#include "structmember.h"
PyMethodDef SAMPServer_methods[] = {
    								{"Listen",  (PyCFunction)pyi_sampserver_listen, METH_VARARGS,
 								    "Initiates the listening socket"},
									{"SetNewConnectionHandler", (PyCFunction)pyi_sampserver_setnewconnhndlr, METH_VARARGS,
 								    "Sets the New connection handler"},
    								{NULL, NULL, 0, NULL}};


PyTypeObject gs_SAMPServerType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "Server",             /*tp_name*/
    sizeof(gs_SAMPServer), /*tp_basicsize*/
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
    "SAMP Server Object",       /* tp_doc */
};

static PyMemberDef SAMPServer_members[] = {
	{"context", T_OBJECT_EX, offsetof(gs_SAMPServer, mp_context), 0,"stats updatehandler function"},
    {NULL}  /* Sentinel */
};

extern std::vector<gs_SAMPServer *> m_py_servers;

PyObject *pyi_sampserver_listen(gs_SAMPServer *self, PyObject *args) {
	PyObject *hostname_str;
	if (!PyArg_ParseTuple(args, "O", &hostname_str))
        Py_RETURN_NONE;
	
	wchar_t *str = Py::copyPythonString(hostname_str);
	char mb_str[256];
	wcstombs(mb_str, str, sizeof(mb_str));
	free((void *)str);

	SAMP::Server *serv = new SAMP::Server(mb_str);
	self->samp_server = serv;
	addSAMPServer(serv);
	Py_RETURN_NONE;
}
PyObject *pyi_sampserver_setnewconnhndlr(gs_SAMPServer *self, PyObject *args) {
	if (!PyArg_ParseTuple(args, "O", &self->mp_new_conn_hndlr))
        Py_RETURN_NONE;
	Py_RETURN_NONE;
}
PyObject * SAMPServer_New(PyTypeObject *type, PyObject *args, PyObject *kwd) {
	gs_SAMPServer *obj = (gs_SAMPServer *)PyType_GenericNew(type, args, kwd);
	obj->samp_server = NULL;
	m_py_servers.push_back(obj);
	return (PyObject *)obj;
}
bool PySAMP_ServerReady() {
	gs_SAMPServerType.tp_methods = SAMPServer_methods;

	gs_SAMPServerType.tp_new = SAMPServer_New;
	gs_SAMPServerType.tp_alloc = PyType_GenericAlloc;
	gs_SAMPServerType.tp_members = SAMPServer_members;
	if (PyType_Ready(&gs_SAMPServerType) < 0)
		return false;
	return true;
}
void PySAMP_InitServerObj(PyObject *m) {

	Py_INCREF(&gs_SAMPServerType);
	PyModule_AddObject(m, "Server", (PyObject *)&gs_SAMPServerType);
}

