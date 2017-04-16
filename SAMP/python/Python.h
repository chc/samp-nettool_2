#ifndef _SAMP_PYTHON_H
#define _SAMP_PYTHON_H
#include <Python.h>
#include <stdint.h>
#include <BitStream.h>
#include <string>
#include "PySAMPClient.h"
#include <SAMP/SAMPPacket.h>
namespace SAMP {
	class Server;
	class Client;
};

enum EConnRejectReason {
		EConnRejectReason_Accepted,
		EConnRejectReason_InvalidPass,
		EConnRejectReason_Banned,
		EConnRejectReason_ServerFull,
		EConnRejectReason_ConnFailed, //spoof failed bad internet, w/e
		EConnRejectReason_ConnLost, //dcs, "the server is restarting", reconnets
		EConnRejectReason_Disconnected, //drop connection
		EConnRejectReason_FailedEncryption, //confuse the player!!
};

namespace Py {
	void Init();
	void Tick();
	void PySAMP_CheckAndPrintErrors();


	EConnRejectReason OnNewConnection(SAMP::Server *server, SAMP::Client *client, std::string password = "");
	void OnGotRPC(SAMP::Client *client, uint32_t rpc_id, RakNet::BitStream *data, bool client_to_server);
	void OnConnectionAccepted(SAMP::Client *client, int playerid, uint32_t challenge);
	void OnGotSync(SAMP::Client *client, SAMP::PacketEnumeration type, RakNet::BitStream *data, bool client_to_server);
	void OnGotStatsUpdate(SAMP::Client *client, int32_t money, int32_t drunk);
	gs_SAMPClient *GetPyClient(SAMP::Client *client);
	wchar_t *copyPythonString(PyObject *string);
}
#endif //_SAMP_PYTHON_H