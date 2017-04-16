#ifndef _PYSAMP_SYNC_H
#define _PYSAMP_SYNC_H
#include "SAMP/SAMPSync.h"
#include <Python.h>
namespace Py {
	PyObject* SyncToPyDict_Player(SAMP::PLAYER_SYNC_INFO *sync, bool client_to_server);
	void PyDictToSync_Player(SAMP::PLAYER_SYNC_INFO *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Vehicle(SAMP::VEHICLE_SYNC_INFO *sync, bool client_to_server);
	void PyDictToSync_Vehicle(SAMP::VEHICLE_SYNC_INFO *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Aim(SAMP::SAMPAimSync *sync, bool client_to_server);
	void PyDictToSync_Aim(SAMP::SAMPAimSync *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Bullet(SAMP::SAMPBulletSync *sync, bool client_to_server);
	void PyDictToSync_Bullet(SAMP::SAMPBulletSync *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Passenger(SAMP::PASSENGER_SYNC_DATA *sync, bool client_to_server);
	void PyDictToSync_Passenger(SAMP::PASSENGER_SYNC_DATA *out, PyObject* dict, bool client_to_server);

	PyObject* SyncToPyDict_Spectator(SAMP::SPECTATOR_SYNC_DATA *sync, bool client_to_server);
	void PyDictToSync_Spectator(SAMP::SPECTATOR_SYNC_DATA *out, PyObject* dict, bool client_to_server);

	//PyObject* SyncToPyDict_Marker(SAMP::SAMPMarkerSync *sync, bool client_to_server);
	//void PyDictToSync_Marker(SAMP::SAMPMarkerSync *out, PyObject* dict, bool client_to_server);
	void PySAMP_AddPacketDefs(PyObject *module);
}
#endif //_PYSAMP_SYNC_H