#include "Python.h"
#include "PySAMPRPC.h"

#include <StringCompressor.h>

PyObject* RPCToPyDict(RPCNameMap *map, RakNet::BitStream *bs, bool client_to_server) {
	PyObject *rpc_dict = PyDict_New();
	if(map->mp_rpc_var_desc[0].type == ERPCVariableType_NoInit) return rpc_dict;
	
	int i=0;
	char str[4096];
	wchar_t wstr[4096];
	while(map->mp_rpc_var_desc[i].type != ERPCVariableType_NoInit) {
		if((!map->mp_rpc_var_desc[i].client_var && client_to_server) || (!map->mp_rpc_var_desc[i].server_var && !client_to_server)) {
			i++;
			continue;
		}
		switch(map->mp_rpc_var_desc[i].type) {
			case ERPCVariableType_Uint32:
				uint32_t u32_val;
				bs->Read(u32_val);
				PyDict_SetItem(rpc_dict, PyUnicode_FromString(map->mp_rpc_var_desc[i].name), PyLong_FromUnsignedLong(u32_val));
				break;
			case ERPCVariableType_Uint8:
				uint8_t u8_val;
				bs->Read(u8_val);
				PyDict_SetItem(rpc_dict, PyUnicode_FromString(map->mp_rpc_var_desc[i].name), PyLong_FromUnsignedLong(u8_val));
				break;

			case ERPCVariableType_Uint16:
				uint16_t u16_val;
				bs->Read(u16_val);
				PyDict_SetItem(rpc_dict, PyUnicode_FromString(map->mp_rpc_var_desc[i].name), PyLong_FromUnsignedLong(u16_val));
				break;
			case ERPCVariableType_Float:
				float f_val;
				bs->Read(f_val);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, PyFloat_FromDouble(f_val));
			break;
			case ERPCVariableType_LenStr:
				uint8_t len;
				bs->Read(len);
				if(len != (uint8_t)-1 && len != 0) {
					bs->Read(str,len);
					str[len] = 0;
				} else str[0] = 0;
				mbstowcs (wstr, str, strlen(str)+1);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, PyUnicode_FromWideChar(wstr, -1));
			break;
			case ERPCVariableType_LenU32Str:
				uint32_t len32;
				bs->Read(len32);
				if(len32 != (uint32_t)-1 && len != 0) {
					bs->Read(str,len32);
					str[len32] = 0;
				} else str[0] = 0;

				mbstowcs (wstr, str, strlen(str)+1);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, PyUnicode_FromWideChar(wstr, -1));
			break;
			case ERPCVariableType_LenU16Str:
				uint16_t len16;
				bs->Read(len16);
				if(len16 != (uint16_t)-1 && len != 0) {
					bs->Read(str,len16);
					str[len16] = 0;
				} else str[0] = 0;
				mbstowcs (wstr, str, strlen(str)+1);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, PyUnicode_FromWideChar(wstr, -1));
			break;
			
			case ERPCVariableType_LenStr_Compressed:
				StringCompressor::Instance()->DecodeString(str, sizeof(str), bs);
				mbstowcs (wstr, str, strlen(str)+1);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, PyUnicode_FromWideChar(wstr, -1));
			break;
			case ERPCVariableType_Custom:
				Py_DECREF(rpc_dict);
				return map->mp_cust_bitstream_to_py_dict(map, bs, true);
			break;
			default:
			break;
		}
		i++;
	}
	//Py_INCREF(rpc_dict);
	return rpc_dict;
}
void PyRPCDictToBitStream(RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {
	int i=0;
	char *str;
	const wchar_t *wstr;
	while(map->mp_rpc_var_desc[i].type != ERPCVariableType_NoInit) {
		PyObject *obj = PyDict_GetItemString(dict, map->mp_rpc_var_desc[i].name);
		if((!map->mp_rpc_var_desc[i].client_var && client_to_server) || (!map->mp_rpc_var_desc[i].server_var && !client_to_server) || (!obj && map->mp_rpc_var_desc[i].type != ERPCVariableType_Custom)) {
			i++;
			continue;
		}
		switch(map->mp_rpc_var_desc[i].type) {
			case ERPCVariableType_Uint32:
			out->Write((uint32_t)PyLong_AsUnsignedLong(obj));
			break;
			case ERPCVariableType_Uint8:
			out->Write((uint8_t)PyLong_AsUnsignedLong(obj));
			break;
			case ERPCVariableType_Uint16:
			out->Write((uint16_t)PyLong_AsUnsignedLong(obj));
			break;
			case ERPCVariableType_Float:
			out->Write((float)PyFloat_AsDouble(obj));
			break;
			case ERPCVariableType_LenStr:
				wstr = Py::copyPythonString(obj);
				str = (char *)malloc(wcslen(wstr)+1);
				memset(str,0, wcslen(wstr)+1);
				wcstombs(str, wstr, wcslen(wstr));
				if(strlen(str) == 0) {
					out->Write((uint8_t)-1);
				} else {
					out->Write((uint8_t)strlen(str));
					out->Write(str, strlen(str));
				}
				free((void *)wstr);
				free((void *)str);
			break;
			case ERPCVariableType_LenU16Str:
				wstr = Py::copyPythonString(obj);
				str = (char *)malloc(wcslen(wstr)+1);
				memset(str,0, wcslen(wstr)+1);
				wcstombs(str, wstr, wcslen(wstr));
				if(strlen(str) == 0) {
					out->Write((uint16_t)-1);
				} else {
					out->Write((uint16_t)strlen(str));
					out->Write(str, strlen(str));
				}
				free((void *)str);
				free((void *)wstr);
				break;
			case ERPCVariableType_LenU32Str:
				wstr = Py::copyPythonString(obj);
				str = (char *)malloc(wcslen(wstr)+1);
				memset(str,0, wcslen(wstr)+1);
				wcstombs(str, wstr, wcslen(wstr));
				if(strlen(str) == 0) {
					out->Write((uint32_t)-1);
				} else {
					out->Write((uint32_t)strlen(str));
					out->Write(str, strlen(str));
				}
				free((void *)str);
				free((void *)wstr);
			break;
			case ERPCVariableType_LenStr_Compressed:
				wstr = Py::copyPythonString(obj);
				str = (char *)malloc(wcslen(wstr)+1);
				memset(str,0, wcslen(wstr)+1);
				wcstombs(str, wstr, wcslen(wstr));
				StringCompressor::Instance()->EncodeString(str, strlen(str)+1,out);
				free((void *)str);
				free((void *)wstr);
			break;
			case ERPCVariableType_Custom:
				return map->py_rpcdict_to_bitsteam(map, out, dict, client_to_server);
			break;
		}
		i++;
	}
}

//custom rpc handlers
PyObject *GameInitRPCToPyDict(RPCNameMap *map, RakNet::BitStream *bs, bool client_to_server) {
	bool temp_bool;
	float temp_float;
	uint32_t temp_uint32;
	uint16_t temp_uint16;
	uint8_t temp_uint8;

	char servername[256];
	wchar_t wstr[4096];

	PyObject *seq_dict = PyDict_New();
	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("zone_names"), temp_bool ? Py_True : Py_False);

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("cj_walk"), temp_bool ? Py_True : Py_False);

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("allow_weapons"), temp_bool ? Py_True : Py_False);

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("limit_chat_radius"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("chat_radius"), PyFloat_FromDouble(temp_float));

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("stunt_bonus"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("nametag_dist"), PyFloat_FromDouble(temp_float));

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("disable_enter_exit"), temp_bool ? Py_True : Py_False);

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("nametag_los"), temp_bool ? Py_True : Py_False);

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("manual_veh_lighting"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("num_spawn_classes"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint16);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("playerid"), PyLong_FromUnsignedLong(temp_uint16));

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("show_nametags"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("show_player_markers"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("server_hour"), PyLong_FromUnsignedLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("server_weather"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("gravity"), PyFloat_FromDouble(temp_float));

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("lan_mode"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("drop_money_on_death"), PyLong_FromUnsignedLong(temp_uint32));

	bs->ReadCompressed(temp_bool);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("unknown"), temp_bool ? Py_True : Py_False);

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("onfoot_sendrate"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("incar_sendrate"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("firing_sendrate"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("send_multiplier"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("lagcomp"), PyLong_FromUnsignedLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("unknown_2"), PyLong_FromUnsignedLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("unknown_3"), PyLong_FromUnsignedLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("unknown_4"), PyLong_FromUnsignedLong(temp_uint8));

	bs->Read(temp_uint8); //servername len
	bs->Read((char *)&servername, temp_uint8);
	servername[temp_uint8] = 0;
	
	mbstowcs (wstr, servername, strlen(servername)+1);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("hostname"), PyUnicode_FromWideChar(wstr, -1));

	PyObject *py_list = PyList_New(212);
	for(int i=0;i<212;i++) {
		bs->Read(temp_uint8);
		PyList_SET_ITEM(py_list, i, PyLong_FromUnsignedLong(temp_uint8));
	}
	PyDict_SetItem(seq_dict, PyUnicode_FromString("preloaded_models"), py_list);	

	return seq_dict;
}
void GameInitPyDictToRPC(RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {

	PyObject *dict_item;
	char servername[256];
	memset(&servername,0,sizeof(servername));

	dict_item = PyDict_GetItemString(dict, "zone_names");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "cj_walk");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "allow_weapons");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "limit_chat_radius");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "chat_radius");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "stunt_bonus");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "nametag_dist");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "disable_enter_exit");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "nametag_los");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "manual_veh_lighting");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "num_spawn_classes");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "playerid");
	out->Write((uint16_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "show_nametags");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "show_player_markers");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "server_hour");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "server_weather");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "gravity");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "lan_mode");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "drop_money_on_death");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "unknown");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "onfoot_sendrate");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "incar_sendrate");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "firing_sendrate");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "send_multiplier");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "lagcomp");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "unknown_2");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "unknown_3");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "unknown_4");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "hostname");
	wchar_t *server_name_wide = Py::copyPythonString(dict_item);
	wcstombs(servername, server_name_wide, wcslen(server_name_wide));
	free((void *)server_name_wide);

	out->Write((uint8_t)strlen(servername));
	out->Write((char *)&servername, strlen(servername));

	dict_item = PyDict_GetItemString(dict, "preloaded_models");
	int num_mdls = PyList_Size(dict_item);
	for(int i=0;i<num_mdls;i++) {
		uint8_t val = PyLong_AsLong(PyList_GetItem(dict_item, i));
		out->Write((uint8_t)val);
	}
}
//