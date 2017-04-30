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
			case ERPCVariableType_CompressedBool:
				bool bval;
				bs->ReadCompressed(bval);
				PyDict_SetItemString(rpc_dict, map->mp_rpc_var_desc[i].name, bval ? Py_True : Py_False);
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
			case ERPCVariableType_CompressedBool:
				out->WriteCompressed((bool)(obj == Py_True));
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

void AddMaterialTextToDict(PyObject *dict, RakNet::BitStream *bs) {
	uint32_t temp_uint32;
	uint16_t temp_uint16;
	uint8_t temp_uint8;

	char str[256];
	wchar_t wstr[4096];

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("material_index"), PyLong_FromLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("material_size"), PyLong_FromLong(temp_uint8));

	bs->Read(temp_uint8);
	bs->Read(str, temp_uint8);
	str[temp_uint8] = 0;	
	mbstowcs (wstr, str, strlen(str)+1);
	PyDict_SetItem(dict, PyUnicode_FromString("font"), PyUnicode_FromWideChar(wstr, -1));

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("font_size"), PyLong_FromLong(temp_uint8));

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("bold"), PyLong_FromLong(temp_uint8));

	bs->Read(temp_uint32);
	PyDict_SetItem(dict, PyUnicode_FromString("font_colour"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(dict, PyUnicode_FromString("back_colour"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("text_alignment"), PyLong_FromLong(temp_uint8));


	//compressed content
	StringCompressor::Instance()->DecodeString(str, sizeof(str), bs);
	mbstowcs (wstr, str, strlen(str)+1);
	PyDict_SetItem(dict, PyUnicode_FromString("text"), PyUnicode_FromWideChar(wstr, -1));
}

void AddMaterialToDict(PyObject *dict, RakNet::BitStream *bs) {
	uint32_t temp_uint32;
	uint16_t temp_uint16;
	uint8_t temp_uint8;

	char str[256];
	wchar_t wstr[4096];

	bs->Read(temp_uint8);
	PyDict_SetItem(dict, PyUnicode_FromString("index"), PyLong_FromLong(temp_uint8));

	bs->Read(temp_uint16);
	PyDict_SetItem(dict, PyUnicode_FromString("model"), PyLong_FromLong(temp_uint16));

	bs->Read(temp_uint8);
	bs->Read(str, temp_uint8);
	str[temp_uint8] = 0;	
	mbstowcs (wstr, str, strlen(str)+1);
	PyDict_SetItem(dict, PyUnicode_FromString("txdname"), PyUnicode_FromWideChar(wstr, -1));

	bs->Read(temp_uint8);
	bs->Read(str, temp_uint8);
	str[temp_uint8] = 0;	
	mbstowcs (wstr, str, strlen(str)+1);
	PyDict_SetItem(dict, PyUnicode_FromString("texturename"), PyUnicode_FromWideChar(wstr, -1));

	bs->Read(temp_uint32);
	PyDict_SetItem(dict, PyUnicode_FromString("material_colour"), PyLong_FromUnsignedLong(temp_uint32));
}
PyObject *CreateObjectRPCToPyDict(struct _RPCNameMap *rpc, RakNet::BitStream *bs, bool client_to_server) {
	bool temp_bool;
	bool is_attached = false;
	float temp_float;
	uint32_t temp_uint32;
	uint16_t temp_uint16;
	uint8_t temp_uint8;

	char servername[256];
	wchar_t wstr[4096];

	PyObject *seq_dict = PyDict_New();
	
	printf("CreateObj In size: %d\n",bs->GetNumberOfBytesUsed());
	bs->Read(temp_uint16);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("id"), PyLong_FromLong(temp_uint16));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("modelid"), PyLong_FromLong(temp_uint32));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("x"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("y"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("z"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("rx"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("ry"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("rz"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("drawdist"), PyFloat_FromDouble(temp_float));


	//unknowns
	bs->Read(temp_uint8);


	bs->Read(temp_uint16);
	if(temp_uint16 != -1) is_attached = true;
	PyDict_SetItem(seq_dict, PyUnicode_FromString("attached_playerid"), PyLong_FromLong(temp_uint16));

	bs->Read(temp_uint16);
	if(temp_uint16 != -1) is_attached = true;
	PyDict_SetItem(seq_dict, PyUnicode_FromString("attached_vehicleid"), PyLong_FromLong(temp_uint16));

	if(is_attached) {
		PyObject *attach_dict = PyDict_New();
		PyObject *py_list = PyList_New(3);
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 0, PyFloat_FromDouble(temp_float));
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 1, PyFloat_FromDouble(temp_float));
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 2, PyFloat_FromDouble(temp_float));
		PyDict_SetItem(attach_dict, PyUnicode_FromString("pos"), py_list);


		py_list = PyList_New(3);
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 0, PyFloat_FromDouble(temp_float));
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 1, PyFloat_FromDouble(temp_float));
		bs->Read(temp_float);
		PyList_SET_ITEM(py_list, 2, PyFloat_FromDouble(temp_float));
		PyDict_SetItem(attach_dict, PyUnicode_FromString("rot"), py_list);

		PyDict_SetItem(seq_dict, PyUnicode_FromString("attach_offsets"), attach_dict);
	}

	char struct_types[2];
	bs->Read(struct_types[0]);
	bs->Read(struct_types[1]);


	if(struct_types[0] == 0) return seq_dict;
	
	PyObject *out_dict;
	if(struct_types[0] == 0x01 && struct_types[1] != 0x01) {
		out_dict = PyDict_New();
		AddMaterialTextToDict(out_dict, bs);
		PyDict_SetItem(seq_dict, PyUnicode_FromString("material_text"), out_dict);
	} else {
		out_dict = PyDict_New();
		AddMaterialToDict(out_dict, bs);
		PyDict_SetItem(seq_dict, PyUnicode_FromString("material"), out_dict);
	}

	bs->Read(temp_uint8);//struct identifier??

	if(temp_uint8 == 2) {
		out_dict = PyDict_New();
		AddMaterialTextToDict(out_dict, bs);
		PyDict_SetItem(seq_dict, PyUnicode_FromString("material_text"), out_dict);
	}

	return seq_dict;
}

void WriteObjectMaterialInfo(PyObject *dict, RakNet::BitStream *out) {
	char servername[256];
	wchar_t *server_name_wide;
	memset(&servername,0,sizeof(servername));

	PyObject *dict_item;

	dict_item = PyDict_GetItemString(dict, "index");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "model");
	out->Write((uint16_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "txdname");
	server_name_wide = Py::copyPythonString(dict_item);
	wcstombs(servername, server_name_wide, wcslen(server_name_wide));
	free((void *)server_name_wide);

	out->Write((uint8_t)strlen(servername));
	out->Write((char *)&servername, strlen(servername));

	dict_item = PyDict_GetItemString(dict, "texturename");
	server_name_wide = Py::copyPythonString(dict_item);
	wcstombs(servername, server_name_wide, wcslen(server_name_wide));
	free((void *)server_name_wide);

	out->Write((uint8_t)strlen(servername));
	out->Write((char *)&servername, strlen(servername));

	dict_item = PyDict_GetItemString(dict, "material_colour");
	out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));
}

void WriteObjectMaterialTextInfo(PyObject *dict, RakNet::BitStream *out) {
	PyObject *dict_item;
	wchar_t *server_name_wide;
	char servername[256];
	memset(&servername,0,sizeof(servername));

	dict_item = PyDict_GetItemString(dict, "material_index");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "material_size");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "font");
	server_name_wide = Py::copyPythonString(dict_item);
	wcstombs(servername, server_name_wide, wcslen(server_name_wide));
	free((void *)server_name_wide);
	out->Write((uint8_t)strlen(servername));
	out->Write((char *)&servername, strlen(servername));

	dict_item = PyDict_GetItemString(dict, "font_size");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "bold");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "font_colour");
	out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "back_colour");
	out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "text_alignment");
	out->Write((uint8_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "text");
	server_name_wide = Py::copyPythonString(dict_item);
	wcstombs(servername, server_name_wide, wcslen(server_name_wide));
	free((void *)server_name_wide);
	StringCompressor::Instance()->EncodeString(servername, strlen(servername)+1,out);

}
namespace SAMP {
	void dump_raknet_bitstream(RakNet::BitStream *stream, const char *fmt, ...);
}
void CreateObjectPyDictToRPC(RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {
	PyObject *dict_item, *mat_dict = NULL, *mat_text_dict = NULL;
	char servername[256];
	uint16_t temp_uint16;

	bool is_attached = false;
	memset(&servername,0,sizeof(servername));

	dict_item = PyDict_GetItemString(dict, "id");
	out->Write((uint16_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "modelid");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "x");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "y");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "z");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "rx");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "ry");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "rz");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "drawdist");
	out->Write((float)PyFloat_AsDouble(dict_item));

	mat_dict = PyDict_GetItemString(dict, "material");
	mat_text_dict = PyDict_GetItemString(dict, "material_text");

	char struct_types[2];
	struct_types[0] = 0;
	struct_types[1] = 0;
	if(mat_dict) {
		struct_types[0] = 2;
	}

	if(!mat_text_dict) {
		struct_types[1] = 2;
	} else {
		struct_types[1] = 1;
	}


	//unknowns
	out->Write((uint8_t)0);


	dict_item = PyDict_GetItemString(dict, "attached_playerid");
	temp_uint16 = PyLong_AsLong(dict_item);
	if(temp_uint16 != -1) is_attached = true;
	out->Write(temp_uint16);

	dict_item = PyDict_GetItemString(dict, "attached_vehicleid");
	temp_uint16 = PyLong_AsLong(dict_item);
	if(temp_uint16 != -1) is_attached = true;
	out->Write(temp_uint16);
	//

	dict_item = PyDict_GetItemString(dict, "attach_offsets");
	if(dict_item && dict_item != Py_None) {
		PyObject *ao_item;

		PyObject *ar_item = PyDict_GetItemString(dict_item, "pos");

		ao_item = PyList_GetItem(ar_item, 0);
		out->Write((float)PyFloat_AsDouble(ao_item));
		ao_item = PyList_GetItem(ar_item, 1);
		out->Write((float)PyFloat_AsDouble(ao_item));
		ao_item = PyList_GetItem(ar_item, 2);
		out->Write((float)PyFloat_AsDouble(ao_item));

		ar_item = PyDict_GetItemString(dict_item, "rot");

		ao_item = PyList_GetItem(ar_item, 0);
		out->Write((float)PyFloat_AsDouble(ao_item));
		ao_item = PyList_GetItem(ar_item, 1);
		out->Write((float)PyFloat_AsDouble(ao_item));
		ao_item = PyList_GetItem(ar_item, 2);
		out->Write((float)PyFloat_AsDouble(ao_item));
	}

	out->Write((char *)&struct_types,sizeof(struct_types));

	if(mat_dict) {
		//write mat dict
		WriteObjectMaterialInfo(mat_dict, out);
	}
	
	if(mat_text_dict) {
		out->Write((uint8_t)0x02);
		WriteObjectMaterialTextInfo(mat_text_dict, out);
		//out->Write((uint8_t)0); //terminator byte?? maybe alignment byte
	}
	printf("CreateObj Out size: %d\n",out->GetNumberOfBytesUsed());
	SAMP::dump_raknet_bitstream(out, "file.bin");
	out->ResetReadPointer();
}

PyObject *SetObjectMaterialRPCToPyDict(struct _RPCNameMap *map, RakNet::BitStream *bs, bool client_to_server) {
	PyObject *seq_dict = PyDict_New();
	PyObject *out_dict = PyDict_New();

	uint8_t identifier; //should always be 0x02;
	uint16_t objid;
	bs->Read(objid);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("id"), PyLong_FromUnsignedLong(objid));

	bs->Read(identifier);
	switch(identifier) {
		case 0x01:
			AddMaterialToDict(out_dict, bs);
			PyDict_SetItem(seq_dict, PyUnicode_FromString("material"), out_dict);
			break;
		case 0x02:
			AddMaterialTextToDict(out_dict, bs);
			PyDict_SetItem(seq_dict, PyUnicode_FromString("material_text"), out_dict);
			break;
	}


	return seq_dict;
}
void SetObjectMaterialPyDictToRPC(struct _RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {
	PyObject *item = PyDict_GetItemString(dict, "id");
	PyObject *mat_text_dict = PyDict_GetItemString(dict, "material_text");
	PyObject *mat_dict = PyDict_GetItemString(dict, "material");

	out->Write((uint16_t)PyLong_AsUnsignedLong(item));

	if(mat_text_dict) {
		out->Write((uint8_t)0x02);		
		WriteObjectMaterialTextInfo(mat_text_dict, out);
	} else if(mat_dict) {
		out->Write((uint8_t)0x01);		
		WriteObjectMaterialInfo(mat_dict, out);
	} else {
		out->Write((uint8_t)0x00); //shouldn't happen
	}
}


PyObject *SetPlayerAttachedObjectRPCToPyDict(struct _RPCNameMap *, RakNet::BitStream *bs, bool client_to_server) {
	char tempbuf[2];
	bool temp_bool;
	float temp_float;
	bool bval;
	uint32_t temp_uint32;
	uint16_t temp_uint16;
	uint8_t temp_uint8;

	PyObject *seq_dict = PyDict_New();
	
	bs->Read(temp_uint16);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("id"), PyLong_FromLong(temp_uint16));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("index"), PyLong_FromLong(temp_uint32));

	tempbuf[0] = 0;
	//bs->ReadBits((unsigned char *)&tempbuf, 1);
	bs->ReadCompressed(bval);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("unk1"), bval == true ? Py_True : Py_False);//PyLong_FromLong(tempbuf[0]));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("modelid"), PyLong_FromLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("bone"), PyLong_FromLong(temp_uint32));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("x"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("y"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("z"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("rx"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("ry"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("rz"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("sx"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("sy"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_float);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("sz"), PyFloat_FromDouble(temp_float));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("mat_1"), PyLong_FromUnsignedLong(temp_uint32));

	bs->Read(temp_uint32);
	PyDict_SetItem(seq_dict, PyUnicode_FromString("mat_2"), PyLong_FromUnsignedLong(temp_uint32));

	return seq_dict;

}
void SetPlayerAttachedObjectPyDictToRPC(struct _RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {
	PyObject *dict_item;

	dict_item = PyDict_GetItemString(dict, "id");
	out->Write((uint16_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "index");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "unk1");
	out->WriteCompressed((bool)(dict_item == Py_True));

	dict_item = PyDict_GetItemString(dict, "modelid");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "bone");
	out->Write((uint32_t)PyLong_AsLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "x");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "y");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "z");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "rx");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "ry");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "rz");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "sx");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "sy");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "sz");
	out->Write((float)PyFloat_AsDouble(dict_item));

	dict_item = PyDict_GetItemString(dict, "mat_1");
	out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));

	dict_item = PyDict_GetItemString(dict, "mat_2");
	out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));
}

PyObject *UpdateScoreBoardPingsIPRPCToPyDict(struct _RPCNameMap *nap, RakNet::BitStream *bs, bool) {
	PyObject *seq_dict = PyDict_New();
	std::vector<PyObject *> dicts;
	while(bs->GetNumberOfUnreadBits() > 0) {
		uint16_t playerid;
		int32_t score;
		uint32_t ping;
		if(!bs->Read(playerid))
			break;
		if(!bs->Read(score))
			break;
		if(!bs->Read(ping))
			break;

		PyObject *dict = PyDict_New();
		PyDict_SetItem(dict, PyUnicode_FromString("id"), PyLong_FromUnsignedLong(playerid));
		PyDict_SetItem(dict, PyUnicode_FromString("score"), PyLong_FromLong(score));
		PyDict_SetItem(dict, PyUnicode_FromString("ping"), PyLong_FromUnsignedLong(ping));

		dicts.push_back(dict);
	}


	PyObject *py_list = PyList_New(dicts.size());
	for(int i=0;i<dicts.size();i++) {
		PyList_SET_ITEM(py_list, i, dicts[i]);
	}
	PyDict_SetItem(seq_dict, PyUnicode_FromString("players"), py_list);

	return seq_dict;
}


void UpdateScoreBoardPingsIPPyDictToRPC(struct _RPCNameMap *map, RakNet::BitStream *out, PyObject* dict, bool client_to_server) {
	PyObject *player_list = PyDict_GetItemString(dict, "players");
	if(player_list) {
		int num_players = PyList_Size(player_list);
		for(int i=0;i<num_players;i++) {
			PyObject *item = PyList_GetItem(player_list,i);

			PyObject *dict_item = PyDict_GetItemString(item, "id");
			out->Write((uint16_t)PyLong_AsUnsignedLong(dict_item));

			dict_item = PyDict_GetItemString(item, "score");
			out->Write((int32_t)PyLong_AsLong(dict_item));

			dict_item = PyDict_GetItemString(item, "ping");
			out->Write((uint32_t)PyLong_AsUnsignedLong(dict_item));

		}
	}
}


//