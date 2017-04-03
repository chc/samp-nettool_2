#include "PySAMPRPC.h"

RPCNameMap mp_rpc_map[] = {
	{"SetPlayerPos", ESAMPRPC_SetPlayerPos, {{"x", ERPCVariableType_Float,true, true}, 
												{"y", ERPCVariableType_Float,true, true}, 
												{"z", ERPCVariableType_Float,true, true}, 
												{NULL, ERPCVariableType_NoInit}}},

	{"SetPlayerArmour", ESAMPRPC_SetPlayerArmour, {
													{"Armour", ERPCVariableType_Float,true, true}, 
													{NULL, ERPCVariableType_NoInit}}},
	{"SetPlayerSkin", ESAMPRPC_SetPlayerSkin, {		{"ID", ERPCVariableType_Uint32,true, true}, 
													{"Skin", ERPCVariableType_Uint32,true, true}, 
													{NULL, ERPCVariableType_NoInit}}},
	{"SetPlayerHealth", ESAMPRPC_SetPlayerHealth, {		{"Health", ERPCVariableType_Float,true, true},  
													{NULL, ERPCVariableType_NoInit}}},
	{"ClientJoin", ESAMPRPC_ClientJoin, {{"NetCodeVer", ERPCVariableType_Uint32,true, true}, 
													{"Mod", ERPCVariableType_Uint8,true, true}, 
													{"Name", ERPCVariableType_LenStr,true, true}, 
													{"Challenge", ERPCVariableType_Uint32,true, true}, 
													{"GPCI", ERPCVariableType_LenStr,true, true}, 
													{"Version", ERPCVariableType_LenStr,true, true}, 
													{"Unknown", ERPCVariableType_Uint32,true, true}, 
													{NULL, ERPCVariableType_NoInit}}},
	{"NPCJoin", ESAMPRPC_NPCJoin, {{"NetCodeVer", ERPCVariableType_Uint32,true, true}, 
													{"Mod", ERPCVariableType_Uint8,true, true}, 
													{"Name", ERPCVariableType_LenStr,true, true}, 
													{"Unknown", ERPCVariableType_Uint32,true, true}, 
													{NULL, ERPCVariableType_NoInit}}},
	{"EnterVehicle", ESAMPRPC_EnterVehicle, {NULL, ERPCVariableType_NoInit}},
	{"AddPlayerToWorld", ESAMPRPC_AddPlayerToWorld,  {{"id", ERPCVariableType_Uint16,false, true},
													  {"team", ERPCVariableType_Uint8,false, true},
													  {"skin", ERPCVariableType_Uint32,false, true},
													  {"x", ERPCVariableType_Float,false, true},
													  {"y", ERPCVariableType_Float,false, true},
													  {"z", ERPCVariableType_Float,false, true},
													  {"z_angle", ERPCVariableType_Float,false, true},
													  {"colour", ERPCVariableType_Uint32,false, true},
													  {"fightstyle", ERPCVariableType_Uint8,false, true},
													{NULL, ERPCVariableType_NoInit}}},
	{"Death", ESAMPRPC_Death, {
								{"reason", ERPCVariableType_Uint8,true, true}, 
								{"killerid", ERPCVariableType_Uint16,true, true}, 
								{NULL, ERPCVariableType_NoInit}}},
	{"DeletePlayerFromWorld", ESAMPRPC_DeletePlayerFromWorld, {NULL, ERPCVariableType_NoInit}},
	{"Create3DTextLabel", ESAMPRPC_Create3DTextLabel, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"colour", ERPCVariableType_Uint32,true, true}, 
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"z", ERPCVariableType_Float,true, true}, 
											{"drawdist", ERPCVariableType_Float,true, true}, 
											{"test_los", ERPCVariableType_Uint8,true, true}, 
											{"playerid", ERPCVariableType_Uint16,true, true}, 
											{"vehicle", ERPCVariableType_Uint16,true, true}, 
											{"text", ERPCVariableType_LenStr_Compressed,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"Destroy3DTextLabel", ESAMPRPC_Delete3DTextLabel, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
											
	{"ClientCmd", ESAMPRPC_ClientCommand, {
											{"Command", ERPCVariableType_LenU32Str,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"ClientSpawn", ESAMPRPC_ClientSpawned, {NULL, ERPCVariableType_NoInit}},
	{"Delete3DTextLabel", ESAMPRPC_Delete3DTextLabel, {NULL, ERPCVariableType_NoInit}},
	{"SendPlayerDialog", ESAMPRPC_SendPlayerDialog, {
													{"id", ERPCVariableType_Uint16,true, true}, 
													{"type", ERPCVariableType_Uint8,true, true}, 
													{"title", ERPCVariableType_LenStr,true, true}, 
													{"button_1", ERPCVariableType_LenStr,true, true}, 
													{"button_2", ERPCVariableType_LenStr,true, true}, 
													{"msg", ERPCVariableType_LenStr_Compressed,true, true}, 
													{NULL, ERPCVariableType_NoInit}}
													},
	{"DialogResponse", ESAMPRPC_DialogResponse, {
													{"id", ERPCVariableType_Uint16,true, true}, 
													{"button", ERPCVariableType_Uint8,true, true}, 
													{"selected_item", ERPCVariableType_Uint16,true, true}, 
													{"response", ERPCVariableType_LenStr,true, true}, 
													{NULL, ERPCVariableType_NoInit}}
													},
	{"DestroyPickup", ESAMPRPC_DestroyPickup, {NULL, ERPCVariableType_NoInit}},
	{"ShowGameText", ESAMPRPC_ShowGameText, {NULL, ERPCVariableType_NoInit}},
	{"RequestClass", ESAMPRPC_RequestClass, {
													{"class_id", ERPCVariableType_Uint32,true, false}, 


													{"unknown", ERPCVariableType_Uint8,false, true}, 
													{"team", ERPCVariableType_Uint8,false, true}, 
													{"skin", ERPCVariableType_Uint32,false, true}, 
													{"unknown2", ERPCVariableType_Uint8,false, true}, 
													{"x", ERPCVariableType_Float,false, true}, 
													{"y", ERPCVariableType_Float,false, true}, 
													{"z", ERPCVariableType_Float,false, true}, 
													{"z_angle", ERPCVariableType_Float,false, true}, 

													{"weapon_1", ERPCVariableType_Uint32,false, true},
													{"weapon_2", ERPCVariableType_Uint32,false, true},
													{"weapon_3", ERPCVariableType_Uint32,false, true},

													{"ammo_1", ERPCVariableType_Uint32,false, true},
													{"ammo_2", ERPCVariableType_Uint32,false, true},
													{"ammo_3", ERPCVariableType_Uint32,false, true},
													{NULL, ERPCVariableType_NoInit}}
													},
	{"RequestSpawn", ESAMPRPC_RequestSpawn, {
													{"unknown", ERPCVariableType_Uint32,false, true},
													{"unknown2", ERPCVariableType_Uint32,false, true},
													{NULL, ERPCVariableType_NoInit}}
													},
	{"ConnectionRejected", ESAMPRPC_ConnectionRejected, {
													{"reason", ERPCVariableType_Uint8,false, true}, 
														/*
															reason = 0 - unknown
															reason 1 = invalid version
															reason 2 = invalid nick
															reason 3 = bad mod version
															reason 4 = unable to allocate player slot
														*/
													{NULL, ERPCVariableType_NoInit}}
													},
														
	{"VehicleCreate", ESAMPRPC_VehicleCreate, {
												{"id", ERPCVariableType_Uint16,true, true}, 
												{"modelid", ERPCVariableType_Uint32,true, true}, 
												{"x", ERPCVariableType_Float,true, true}, 
												{"y", ERPCVariableType_Float,true, true}, 
												{"z", ERPCVariableType_Float,true, true}, 
												
												{"zrot", ERPCVariableType_Float,true, true}, 
												
												{"col1", ERPCVariableType_Uint8,true, true}, 
												{"col2", ERPCVariableType_Uint8,true, true}, 
												{"health", ERPCVariableType_Float,true, true}, 
												{"interior", ERPCVariableType_Uint8,true, true}, 
												{"door_damage", ERPCVariableType_Uint32,true, true}, 
												{"panel_damage", ERPCVariableType_Uint32,true, true}, 
												{"light_damage", ERPCVariableType_Uint8,true, true}, 
												{"tire_damage", ERPCVariableType_Uint8,true, true}, 
												{"siren", ERPCVariableType_Uint8,true, true}, 												
												//lame
												{"comp1", ERPCVariableType_Uint8,true, true}, 
												{"comp2", ERPCVariableType_Uint8,true, true}, 
												{"comp3", ERPCVariableType_Uint8,true, true}, 
												{"comp4", ERPCVariableType_Uint8,true, true}, 
												{"comp5", ERPCVariableType_Uint8,true, true}, 
												{"comp6", ERPCVariableType_Uint8,true, true}, 
												{"comp7", ERPCVariableType_Uint8,true, true}, 
												{"comp8", ERPCVariableType_Uint8,true, true}, 
												{"comp9", ERPCVariableType_Uint8,true, true}, 
												{"comp10", ERPCVariableType_Uint8,true, true}, 
												{"comp11", ERPCVariableType_Uint8,true, true}, 
												{"comp12", ERPCVariableType_Uint8,true, true}, 
												{"comp13", ERPCVariableType_Uint8,true, true}, 
												{"comp14", ERPCVariableType_Uint8,true, true}, 
												{"col32_1", ERPCVariableType_Uint32,true, true}, 
												{"col32_2", ERPCVariableType_Uint32,true, true}, 
												{"unknown", ERPCVariableType_Uint8,true, true}, 
												{NULL, ERPCVariableType_NoInit}
											}},
	{"VehicleDestroy", ESAMPRPC_VehicleDelete, {
											{"id", ERPCVariableType_Uint16,false, true}, 
											{"unknown1", ERPCVariableType_Uint32,false, true}, //literally just an uninitalized integer from SAMP server, possibly from alignment
											{NULL, ERPCVariableType_NoInit}
										}},
	{"ExitVehicle", ESAMPRPC_ExitVehicle, {NULL, ERPCVariableType_NoInit}},
	{"SendClientMessage", ESAMPRPC_SendClientMessage, {
											{"Colour", ERPCVariableType_Uint32,true, true}, 
											{"Message", ERPCVariableType_LenU32Str,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"ShowTextDraw", ESAMPRPC_ShowTextDraw, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"flags", ERPCVariableType_Uint8,true, true}, 
											{"font_width", ERPCVariableType_Float,true, true}, 
											{"font_height", ERPCVariableType_Float,true, true}, 
											{"font_colour", ERPCVariableType_Uint32,true, true}, 
											{"box_width", ERPCVariableType_Float,true, true}, 
											{"box_height", ERPCVariableType_Float,true, true}, 
											{"box_colour", ERPCVariableType_Uint32,true, true}, 
											{"shadow", ERPCVariableType_Uint8,true, true}, 
											{"outline", ERPCVariableType_Uint8,true, true}, 
											{"background_colour", ERPCVariableType_Uint32,true, true}, 
											{"style", ERPCVariableType_Uint8,true, true}, 
											{"selectable", ERPCVariableType_Uint8,true, true}, 
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"model", ERPCVariableType_Uint16,true, true}, 
											{"rx", ERPCVariableType_Float,true, true}, 
											{"ry", ERPCVariableType_Float,true, true}, 
											{"rz", ERPCVariableType_Float,true, true}, 
											{"zoom", ERPCVariableType_Float,true, true}, 
											{"model_col1", ERPCVariableType_Uint16,true, true}, 
											{"model_col2", ERPCVariableType_Uint16,true, true}, 
											{"msg", ERPCVariableType_LenU16Str,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"EditTextDraw", ESAMPRPC_EditTextDraw, {NULL, ERPCVariableType_NoInit}},
	{"SendChatMessage", ESAMPRPC_ChatMessage, {
											{"playerid", ERPCVariableType_Uint16,false, true}, 
											{"Message", ERPCVariableType_LenStr,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"RemovePlayerFromVehicle", ESAMPRPC_RemovePlayerFromVehicle, {NULL, ERPCVariableType_NoInit}},
	{"SetPlayerColour", ESAMPRPC_SetPlayerColor, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"colour", ERPCVariableType_Uint32,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"SetObjectMaterial", ESAMPRPC_SetObjectMaterial, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"index", ERPCVariableType_Uint16,true, true}, 
											{"model", ERPCVariableType_Uint16,true, true}, 
											{"txdname", ERPCVariableType_LenStr,true, true}, 
											{"texturename", ERPCVariableType_LenStr,true, true}, 
											{"materialcolour", ERPCVariableType_Uint32,true, true}, 
											{"unknown", ERPCVariableType_Uint8,true, true},
											{NULL, ERPCVariableType_NoInit}
										}},
	{"PlayerUpdate", ESAMPRPC_PlayerUpdate, {
											{"unknown", ERPCVariableType_Uint32,true, true}, 
											{"unknown2", ERPCVariableType_Uint32,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"SetMapIcon", ESAMPRPC_SetMapIcon, {
											{"iconid", ERPCVariableType_Uint8,true, true}, 
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"z", ERPCVariableType_Float,true, true},
											{"markertype", ERPCVariableType_Uint8,true, true}, 
											{"colour", ERPCVariableType_Uint32,true, true}, 
											{"style", ERPCVariableType_Uint8,true, true}, 
											}},
	{"ServerJoin", ESAMPRPC_ServerJoin, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"colour", ERPCVariableType_Uint32,true, true}, 
											{"npc", ERPCVariableType_Uint8,true, true}, 
											{"name", ERPCVariableType_LenStr,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"ServerQuit", ESAMPRPC_ServerQuit, {
											{"id", ERPCVariableType_Uint16,true, true}, 
											{"reason", ERPCVariableType_Uint8,true, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"PlayerDeath", ESAMPRPC_PlayerDeath, {{"id", ERPCVariableType_Uint16,true, true},  {NULL, ERPCVariableType_NoInit}}},
	{"HideTextDraw", ESAMPRPC_HideTextDraw, {NULL, ERPCVariableType_NoInit}},
	{"CreatePickup", ESAMPRPC_CreatePickup, {
											{"id", ERPCVariableType_Uint32,true, true}, 
											{"model", ERPCVariableType_Uint32,true, true}, 
											{"type", ERPCVariableType_Uint32,true, true}, 
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"z", ERPCVariableType_Float,true, true}, 
											{NULL, ERPCVariableType_NoInit}}
										  },
	{"SetCarPlate", ESAMPRPC_SetCarPlate, {
											{"vehicleid", ERPCVariableType_Uint16,true, true}, 
											{"plate", ERPCVariableType_LenStr,true, true}, 
											{NULL, ERPCVariableType_NoInit}}
										  },
	{"SetPlayerWantedLevel", ESAMPRPC_SetPlayerWantedLevel, {
											{"level", ERPCVariableType_Uint8, true, true}, 
											{NULL, ERPCVariableType_NoInit}}
										  },
	{"RemoveBuilding", ESAMPRPC_RemoveBuilding, {
											{"modelid", ERPCVariableType_Uint32,true, true}, 
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"z", ERPCVariableType_Float,true, true}, 
											{"radius", ERPCVariableType_Float,true, true}, 
											{NULL, ERPCVariableType_NoInit}}
										  },
	{"CreateObject", ESAMPRPC_CreateObject, {
												{"id", ERPCVariableType_Uint16,true, true}, 
												{"modelid", ERPCVariableType_Uint32,true, true}, 
												{"x", ERPCVariableType_Float,true, true}, 
												{"y", ERPCVariableType_Float,true, true}, 
												{"z", ERPCVariableType_Float,true, true}, 
												{"rx", ERPCVariableType_Float,true, true}, 
												{"ry", ERPCVariableType_Float,true, true}, 
												{"rz", ERPCVariableType_Float,true, true}, 
												{"drawdist", ERPCVariableType_Float,true, true}, 
												{NULL, ERPCVariableType_NoInit}
													}},
	{"DeleteObject", ESAMPRPC_DeleteObject, {
		{"id", ERPCVariableType_Uint16,false, true},
		{NULL, ERPCVariableType_NoInit}
	}},
	{"SetCameraPos", ESAMPRPC_SetCameraPos, {
											{"x", ERPCVariableType_Float,true, true}, 
											{"y", ERPCVariableType_Float,true, true}, 
											{"z", ERPCVariableType_Float,true, true}
											}},
	{"SetCameraLookAt", ESAMPRPC_SetCameraLookAt, {
											{"x", ERPCVariableType_Float,false, true}, 
											{"y", ERPCVariableType_Float,false, true}, 
											{"z", ERPCVariableType_Float,false, true},
											{"cut", ERPCVariableType_Uint8,false, true}
											}},
												
	{"ResetMoney", ESAMPRPC_ResetMoney, {NULL, ERPCVariableType_NoInit}},
	{"StopAudioStream", ESAMPRPC_StopAudioStream, {NULL, ERPCVariableType_NoInit}},
	{"SetInterior", ESAMPRPC_SetInterior, {
											{"id", ERPCVariableType_Uint8,true, true}, 
											}},
	{"SetPlayerName", ESAMPRPC_SetPlayerName, {
											{"id", ERPCVariableType_Uint16,false, true}, 
											{"name", ERPCVariableType_LenStr,false, true}, 
											{"unknown1", ERPCVariableType_Uint8,false, true}, 
											{"unknown2", ERPCVariableType_Uint8,false, true}, 
											{"unknown3", ERPCVariableType_Uint8,false, true}, 
											{"unknown4", ERPCVariableType_Uint8,false, true}, 
											{"unknown5", ERPCVariableType_Uint8,false, true}, 
											{NULL, ERPCVariableType_NoInit}
										}},
	{"AddGangZone", ESAMPRPC_AddGangZone, {NULL, ERPCVariableType_NoInit}},
	{"GiveMoney", ESAMPRPC_GiveMoney, {NULL, ERPCVariableType_NoInit}},
	{"UpdateScoreboardPingIPS", ESAMPRPC_UpdateScoresPingsIP, {NULL, ERPCVariableType_NoInit}},
	{"TogglePlayerControllable", ESAMPRPC_TogglePlayerControllable, {
								{"enabled", ERPCVariableType_Uint8,true, true}, 
								{NULL, ERPCVariableType_NoInit}}},
	{"PlaySound", ESAMPRPC_PlaySound, {NULL, ERPCVariableType_NoInit}},
	{"SetWorldBounds", ESAMPRPC_SetWorldBounds, {NULL, ERPCVariableType_NoInit}},
	{"GiveMoney",ESAMPRPC_GiveMoney, {NULL, ERPCVariableType_NoInit}},
	{"SetPlayerFacingAngle", ESAMPRPC_SetPlayerFacingAngle, {NULL, ERPCVariableType_NoInit}},
	{"PutPlayerInVehicle", ESAMPRPC_PutPlayerInVehicle, {
				{"carid", ERPCVariableType_Uint16,true, true}, 
				{"seat", ERPCVariableType_Uint8,true, true}, 
				{NULL, ERPCVariableType_NoInit}
			}},
	{"SetPlayerSkillLevel", ESAMPRPC_SetPlayerSkillLevel, {
				{"id", ERPCVariableType_Uint16,false, true}, 
				{"weapon", ERPCVariableType_Uint32,false, true}, 
				{"level", ERPCVariableType_Uint16,false, true}, 
				{"unknown", ERPCVariableType_Uint8,false, true}, 
				{NULL, ERPCVariableType_NoInit}
			}},
	{"SetPlayerDrunkLevel", ESAMPRPC_SetPlayerDrunkLevel, {
				{"level", ERPCVariableType_Uint32, false, true},
				{NULL, ERPCVariableType_NoInit}
			}},
	{"OnPickupPickup", ESAMPRPC_PickupPickup, {
									{"id", ERPCVariableType_Uint32,true, true}, 
									{NULL, ERPCVariableType_NoInit}
								}},
	{"ScmEvent", ESAMPRPC_ScmEvent, {
									{"playerid", ERPCVariableType_Uint16,true, true}, 
									{"event", ERPCVariableType_Uint32,true, true}, 
									{"params1", ERPCVariableType_Uint32,true, true}, 
									{"params2", ERPCVariableType_Uint32,true, true}, 
									{"params3", ERPCVariableType_Uint32,true, true}, 
									{NULL, ERPCVariableType_NoInit}
								}},

	{"GiveTakeDamage", ESAMPRPC_GiveTakeDamage, {{NULL, ERPCVariableType_NoInit}}},
	{"SetPlayerTeam", ESAMPRPC_SetPlayerTeam, {
									{"id", ERPCVariableType_Uint8,true, true}, 
									{NULL, ERPCVariableType_NoInit}
								}},
									
	{"SetPlayerSpawnInfo", ESAMPRPC_SetPlayerSpawnInfo, {
									{"team", ERPCVariableType_Uint8,false, true}, 
									{"skin", ERPCVariableType_Uint32,false, true}, 
									{"unknown", ERPCVariableType_Uint8,false, true},
									{"x", ERPCVariableType_Float,false, true}, 
									{"y", ERPCVariableType_Float,false, true}, 
									{"z", ERPCVariableType_Float,false, true}, 
									{"z_angle", ERPCVariableType_Float,false, true}, 
									{"weapon_1", ERPCVariableType_Uint32, false, true}, 
									{"weapon_2", ERPCVariableType_Uint32,false, true}, 
									{"weapon_3", ERPCVariableType_Uint32,false, true}, 
									{"ammo_1", ERPCVariableType_Uint32,false, true}, 
									{"ammo_2", ERPCVariableType_Uint32,false, true}, 
									{"ammo_3", ERPCVariableType_Uint32,false, true}, 
									{NULL, ERPCVariableType_NoInit}
								}},
	{"SetPlayerHoldingWeapon", ESAMPRPC_SetPlayerHoldingWeapon, {
									{"weaponid", ERPCVariableType_Uint32,true, true}, 
									{NULL, ERPCVariableType_NoInit}
								}},
	{"SetVehicleParams", ESAMPRPC_SetVehicleParams, {
		{"vehicleid", ERPCVariableType_Uint16,true, true}, 
		{"engine", ERPCVariableType_Uint8,true, true}, 
		{"lights", ERPCVariableType_Uint8,true, true}, 
		{"unknown", ERPCVariableType_Uint8,true, true}, 
		{"locked", ERPCVariableType_Uint8,true, true}, 
		{"unknown2", ERPCVariableType_Uint8,true, true}, 
		{"unknown2", ERPCVariableType_Uint8,true, true}, 
		{"unknown_flags", ERPCVariableType_Uint32,true, true}, 
		{"unknown_flags2", ERPCVariableType_Uint32,true, true}, 
		{"unknown_flags3", ERPCVariableType_Uint16,true, true}, 
		{"unknown_flags4", ERPCVariableType_Uint32,true, true}, 
		{NULL, ERPCVariableType_NoInit}
	}},
	{"SetPlayerSpecialAction", ESAMPRPC_SetPlayerSpecialAction, {
		{"action", ERPCVariableType_Uint8,true, true}, 
		{NULL, ERPCVariableType_NoInit}
	}},
	{"SetPlayerFightingStyle", ESAMPRPC_SetPlayerFightingStyle, {
		{"playerid", ERPCVariableType_Uint16,false, true}, 
		{"action", ERPCVariableType_Uint8,false, true}, 
		{NULL, ERPCVariableType_NoInit}
	}},
	{"ApplyAnimation", ESAMPRPC_ApplyAnimation, {
		{"playerid", ERPCVariableType_Uint16,true, true}, 
		{"animlib", ERPCVariableType_LenStr,true, true}, 
		{"animname", ERPCVariableType_LenStr,true, true}, 
		{"delta", ERPCVariableType_Float,true, true}, 
		{"unk1", ERPCVariableType_Uint32,true, true}, 
		{"unk2", ERPCVariableType_Uint8,true, true}, 
		{NULL, ERPCVariableType_NoInit}
	}},
	{"ClearAnimations", ESAMPRPC_ClearAnimations, {
		{NULL, ERPCVariableType_NoInit}
	}},
	{
		"InitGame",  ESAMPRPC_InitGame, {
			{"callback", ERPCVariableType_Custom,true, true}
		},
		GameInitRPCToPyDict, GameInitPyDictToRPC
	},
	{"RemovePlayerMapIcon", ESAMPRPC_RemoveMapIcon, {
		{"id", ERPCVariableType_Uint8,true, true},
		{NULL, ERPCVariableType_NoInit}
	}},
		{"SetVehicleParamsPacked", ESAMPRPC_SetVehicleParamsPacked, {
			{"vehicleid", ERPCVariableType_Uint16,true, true},
			{"flags", ERPCVariableType_Uint16,true, true},
			{"align", ERPCVariableType_Uint8,true, true}, //always u
			{NULL, ERPCVariableType_NoInit}
	}},
	{"CreateActor", ESAMPRPC_CreateActor, {
			{"id", ERPCVariableType_Uint16,true, true},
			{"model", ERPCVariableType_Uint16,true, true},
			{"unknown", ERPCVariableType_Uint16,true, true},
			{"x", ERPCVariableType_Float,true, true},
			{"y", ERPCVariableType_Float,true, true},
			{"z", ERPCVariableType_Float,true, true},
			{"z_angle", ERPCVariableType_Float,true, true},
			{"health", ERPCVariableType_Float,true, true},
			{"unknown", ERPCVariableType_Uint16,true, true},
			{NULL, ERPCVariableType_NoInit}
	}},
	{"DestroyActor", ESAMPRPC_DestroyActor, {
			{"id", ERPCVariableType_Uint16,true, true},
			{"unknown", ERPCVariableType_Uint8,true, true},
			{NULL, ERPCVariableType_NoInit}
	}},
};


const char *GetRPCNameByID(uint32_t id) {
	for(int i=0;i<sizeof(mp_rpc_map)/sizeof(RPCNameMap);i++) {
		if(mp_rpc_map[i].rpc_id == id)
			return mp_rpc_map[i].name;
	}
	return NULL;
}
RPCNameMap *GetRPCNameMapByID(uint32_t id) {
	for(int i=0;i<sizeof(mp_rpc_map)/sizeof(RPCNameMap);i++) {
		if(mp_rpc_map[i].rpc_id == id)
			return &mp_rpc_map[i];
	}
	return NULL;
}

void PySAMP_AddRPCDefs(PyObject *module) {
	char name[256];
	for(int i=0;i<sizeof(mp_rpc_map)/sizeof(RPCNameMap);i++) {
		sprintf(name, "RPC_%s",mp_rpc_map[i].name);
		PyModule_AddIntConstant(module, name, mp_rpc_map[i].rpc_id);
	}
}