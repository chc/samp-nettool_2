import SAMP

import json
import re
import uuid
import random

class OutboundConnectionManager():
	def proxy_client_rpc_hndlr(self, connection, rpcid, rpc_data):
		delegator = self.getRPCDelegator()
		ret = True
		#print("Got RPC {} - {}\n".format(rpcid, rpc_data))
		if rpcid in delegator:
			ret = delegator[rpcid](connection, rpcid, rpc_data)
		
		if ret:
			self.client.connection.SendRPC(rpcid, rpc_data)
		
	def proxy_client_sync_hndlr(self, connection, type, data):
		if type == SAMP.PACKET_VEHICLE_SYNC:
			self.player_info[data["playerid"]]["health"] = data["player_health"]
			self.player_info[data["playerid"]]["armour"] = data["player_armour"]
		elif type == SAMP.PACKET_PLAYER_SYNC:
			self.player_info[data["playerid"]]["health"] = data["health"]
			self.player_info[data["playerid"]]["armour"] = data["armour"]
		elif type == SAMP.PACKET_BULLET_SYNC:
			if self.tool_settings["no_bullets"]:
				return False

		if "keys" in data:
			self.player_info[data["playerid"]]["keys"] = data["keys"]

		if type == SAMP.PACKET_PLAYER_SYNC or type == SAMP.PACKET_VEHICLE_SYNC or type == SAMP.PACKET_PASSENGER_SYNC:
			if "pos" in data:
				self.player_info[data["playerid"]]["pos"] = data["pos"]

			if self.tool_settings["player_info"] and data["playerid"] in self.player_info:
				self.updatePlayerLabel(self.player_info[data["playerid"]])

		self.client.connection.SendSync(type, data)	

	#add to client response, check for if banned/wrong pw/server full, etc
	def proxy_client_accepted(self, connection, playerid, challenge):
		#print("Proxy Client accepted {} {} {}\n".format(connection, playerid, challenge))
		RPCData = {}
		RPCData["NetCodeVer"] = 4057
		RPCData["Challenge"] = challenge ^ RPCData["NetCodeVer"]
		RPCData["Mod"] = 1
		RPCData["Unknown"] = RPCData["Challenge"]
		RPCData["Version"] = "0.3.7-R2"
		RPCData["Name"] = "Nardi"
		RPCData["GPCI"] = "342791B35BB3761EF389D7746B78BC0075932049AC0"#SAMP.GenGPCI(1001*4)
		self.proxy_connection.SendRPC(SAMP.RPC_ClientJoin, RPCData)

		RPCData = {'class_id': 0}
		self.proxy_connection.SendRPC(SAMP.RPC_RequestClass, RPCData)

	def __init__(self, client):
		self.tool_settings = {}
		self.player_info = {}
		self.injected_labels_offset = 500
		self.tool_settings_entries = [
			{'name': 'Health Hacks', 'key': 'health_hacks'},
			{'name': 'Anti-TP', 'key': 'anti_tp'},
			#{'name': 'Wall Hacks', 'key': 'wall_hacks'},
			#{'name': 'Aimbot(Non-locking)', 'key': 'nonlocking_aimbot'},
			{'name': 'Walking Distort', 'key': 'walk_distort'},
			{'name': 'Vehicle Distort', 'key': 'car_distort'},
			{'name': 'Force local chat', 'key': 'local_chat'},
			{'name': 'Disable vehicle collision', 'key': 'veh_collision', 'onselect': self.toggleVehicleCollision},
			{'name': 'Force full player Alpha', 'key': 'full_alpha', 'onselect': self.togglePlayerAlphaMask},
			{'name': 'Show Player info', 'key': 'player_info', 'onselect': self.togglePlayerInfo},
			#{'name': 'Force time of day', 'on_select': self.showTimeOfDayMenu},
			#{'name': 'Force Weather', 'on_select': self.showWeatherMenu},
			#{'name': 'Vehicle speed boost(Horn)', 'key': 'veh_speed_boost'},
			{'name': 'Horn Spammer', 'key': 'veh_horn_spam'},
			#{'name': 'Vehicle Override', 'on_select': self.showVehicleOverrideMenu},			
			{'name': 'Connect/Quit messages', 'key': 'connect_quit_msgs'},
			{'name': 'Disable Bullets', 'key': 'no_bullets'},
			#{'name': 'Toggle Spectator mode', 'key': 'spec_mode'},
			#{'name': 'Block paused', 'key': 'block_paused'},
			#{'name': 'Object finder', 'key': 'object_finder'},
			#{'name': 'Textdraw blocker', 'key': 'textdraw_blocker'},			
			#{'name': 'Chat morpher', 'key': 'chat_morph'}, #morph menu
			#{'name': 'Ping bugger', 'key': 'ping_bugger'}, #65535 ping, 0 ping
			#{'name': 'RPC Control', 'on_select': self.showRPCBlockerMenu}, #dump RPC in/out, block RPCs, spoof RPCs
			#{'name': 'Server Specifics', 'on_select': self.showServerSpecificMenu},

		]

		self.playerid = 0

		for setting in self.tool_settings_entries:
			if "key" in setting:
				self.tool_settings[setting["key"]] = False

		self.servers = [
			{'name': 'UIF', 'connection_string': '51.254.85.134:7776'},
			{'name': 'WC-RP', 'connection_string': 'samp.wc-rp.com:7777'},
			{'name': 'CrazyBobs #1', 'connection_string': 's1.crazybobs.net:7777'},
			{'name': 'CrazyBobs #2', 'connection_string': 's2.crazybobs.net:7777'},
			{'name': 'PTP', 'connection_string': '37.187.196.107:7777'},
			{'name': 'DayZ', 'connection_string': '185.11.146.127:7778'},
			{'name': 'Swat vs Terrorists', 'connection_string': '158.69.221.102:7777'},
			{'name': 'Minigames', 'connection_string': '149.202.65.49:7777'},
			{'name': 'LS CnR', 'connection_string': '164.132.241.61:7777'},
			{'name': 'CoD', 'connection_string': '91.134.156.45:7777'},
			{'name': 'SA CnR', 'connection_string': '192.169.82.202:7777'}
		]
		self.client = client

		self.proxy_connection = SAMP.Client()
		self.proxy_connection.conn_accepted_handler = (self.proxy_client_accepted)
		self.proxy_connection.rpc_handler = (self.proxy_client_rpc_hndlr)
		self.proxy_connection.sync_handler = (self.proxy_client_sync_hndlr)
		#self.proxy_connection.context = {'proxy_client': self}


	#CONNECT MENU
	def handleConnectionDialogResponse(self, response, list_index):
		if response:
			connection_string = self.servers[list_index]["connection_string"]
			self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "Got conn string: {}".format(connection_string), "Colour": 0xFFFFFFFF})
			self.proxy_connection.Connect(connection_string)
		return True
	def showConnectionMenu(self):
		dialog_string = "Name\tConnection String\n"
		for server in self.servers:
			dialog_string += "{}\t{}\n".format(server["name"], server["connection_string"])

		rpc_data = {'msg': dialog_string, 'id': 1, 'title': 'Connection Menu', 'button_1': 'Select', 'button_2': 'Cancel', 'type': 5}
		self.client.connection.SendRPC(SAMP.RPC_SendPlayerDialog, rpc_data)
		return self.handleConnectionDialogResponse
	###

	## TOOL MENU
	def showRPCBlockerMenu(self):
		return None
	def showServerSpecificMenu(self):
		return None
	def showVehicleOverrideMenu(self):
		return None
	def showWeatherMenu(self):
		return None
	def showTimeOfDayMenu(self):
		return None
	def handleToolMenuResponse(self, response, list_index):
		if not response:
			return False
		item = self.tool_settings_entries[list_index]

		self.tool_settings[item["key"]] = not self.tool_settings[item["key"]]
		self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "{} now {}".format(item["key"],self.tool_settings[item["key"]]), "Colour": 0xFFFFFFFF})
		if "onselect" in item:
			item["onselect"](item["key"], self.tool_settings[item["key"]])
		return True
	def showToolMenu(self):
		dialog_string = "Name\tOn/Off\n"
		for entry in self.tool_settings_entries:
			if "key" in entry:
				dialog_string += "{}\t{}\n".format(entry["name"], self.tool_settings[entry["key"]])
			else:
				dialog_string += "{}\n".format(entry["name"])

		rpc_data = {'msg': dialog_string, 'id': 1, 'title': 'Tool Settings', 'button_1': 'Select', 'button_2': 'Cancel', 'type': 5}
		self.client.connection.SendRPC(SAMP.RPC_SendPlayerDialog, rpc_data)
		return self.handleToolMenuResponse

	def toggleVehicleCollision(self, key, current_value):
		self.client.connection.SendRPC(SAMP.RPC_DisableRemoteVehicleCollisions, {"enabled": current_value})
	def togglePlayerAlphaMask(self, key, current_value):
		for playerid, player in self.player_info.items():
			colour = player["colour"]
			if current_value:
				colour = colour | 0x000000FF
			self.client.connection.SendRPC(SAMP.RPC_SetPlayerColour, {"id": playerid, "colour": colour})
	def togglePlayerInfo(self, key, current_value):
		if not current_value:
			self.deleteAllPlayerLabels()
	#delegators
	def handleServerJoin(self, connection, rpcid, rpc_data):
		if self.tool_settings["connect_quit_msgs"]:
			self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "** Join: {}".format(rpc_data["name"]), "Colour": 0xFF0000FF})

		self.player_info[rpc_data["id"]] = {'name': rpc_data["name"], 'streamed': False, 'colour': rpc_data["colour"], 'id': rpc_data["id"], 'health': 0.0, "armour": 0.0, "fightstyle": 0, "keys": 0, "skin": 0, 'pos': [0,0,0]}

		if self.tool_settings["full_alpha"]:
			rpc_data["colour"] = rpc_data["colour"] | 0x000000FF
			self.client.connection.SendRPC(rpcid, rpc_data)
			return False

		
		return True
	def handleServerQuit(self, connection, rpcid, rpc_data):
		if self.tool_settings["connect_quit_msgs"]:
			reason = "Timed out"
			if rpc_data["reason"] == 1:
				reason = "Quit"
			elif rpc_data["reason"] == 2:
				reason = "Kick/Banned"
			self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "** Quit: {} - {}".format(self.player_info[rpc_data["id"]]["name"], reason), "Colour": 0xFF0000FF})

		del self.player_info[rpc_data["id"]]
		return True
	def handleSetPlayerName(self, connection, rpcid, rpc_data):
		if rpc_data["id"] != self.playerid:
			if self.tool_settings["connect_quit_msgs"]:
				self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "** Name Change: {} to {}".format(self.player_info[rpc_data["id"]]["name"], rpc_data["name"]), "Colour": 0xFF0000FF})
			self.player_info[rpc_data["id"]]["name"] = rpc_data["name"]
		return True
	def handleSetPlayerColour(self, connection, rpcid, rpc_data):
		if rpc_data["id"] != self.playerid and rpc_data["id"] in self.player_info:
			self.player_info[rpc_data["id"]]["colour"] = rpc_data["colour"]
		if self.tool_settings["full_alpha"]:
			rpc_data["colour"] = rpc_data["colour"] | 0x000000FF
			self.client.connection.SendRPC(rpcid, rpc_data)
			return False
		return True
	def handleAddPlayerToWorld(self, connection, rpcid, rpc_data):
		self.player_info[rpc_data["id"]]["streamed"] = True
		self.player_info[rpc_data["id"]]["skin"] = rpc_data["skin"]
		self.player_info[rpc_data["id"]]["team"] = rpc_data["team"]

		if self.tool_settings["full_alpha"]:
			rpc_data["colour"] = rpc_data["colour"] | 0x000000FF
			self.client.connection.SendRPC(rpcid, rpc_data)
			return False
		return True
	def handleRemovePlayerToWorld(self, connection, rpcid, rpc_data):
		self.player_info[rpc_data["id"]]["streamed"] = False
		return True
	def handleChatMessage(self, connection, rpcid, rpc_data):
		ret = True
		if self.tool_settings["local_chat"]:
			ret = False
			if rpc_data["id"] in self.player_info and "streamed" in self.player_info[rpc_data["id"]] and self.player_info[rpc_data["id"]]["streamed"]:
				ret = True
		return ret
	def handleInitGame(self, connection, rpcid, rpc_data):
		print("Client init: {}\n".format(rpc_data))
		self.playerid = rpc_data["playerid"]
		return True
	def getRPCDelegator(self):
		ret = {}
		ret[SAMP.RPC_ServerJoin] = self.handleServerJoin
		ret[SAMP.RPC_ServerQuit] = self.handleServerQuit
		ret[SAMP.RPC_SetPlayerName] = self.handleSetPlayerName
		ret[SAMP.RPC_SetPlayerColour] = self.handleSetPlayerColour
		ret[SAMP.RPC_AddPlayerToWorld] = self.handleAddPlayerToWorld
		ret[SAMP.RPC_DeletePlayerFromWorld] = self.handleRemovePlayerToWorld
		ret[SAMP.RPC_ChatMessage] = self.handleChatMessage
		ret[SAMP.RPC_InitGame] = self.handleInitGame
		return ret

	def SendRPC(self, rpc_id, rpc_data):
		if self.tool_settings["health_hacks"]:
			if rpc_id == SAMP.RPC_SetPlayerHealth or rpc_id == SAMP.RPC_SetPlayerArmour:
				return False

		if self.tool_settings["anti_tp"]:
			if rpc_id == SAMP.RPC_SetPlayerPos or rpc_id == SAMP.RPC_SetPlayerFacingAngle or rpc_id == SAMP.RPC_SetPlayerPosFindZ or rpc_id == SAMP.RPC_SetPlayerInterior:
				return False
		self.proxy_connection.SendRPC(rpc_id, rpc_data)
	def SendSync(self, type, data):
		if type == SAMP.PACKET_VEHICLE_SYNC:
			if self.tool_settings["veh_horn_spam"]:
				if data["keys"] == 0:
					data["keys"] = 2
			if self.tool_settings["car_distort"]:
				if "quat" in data:
					quat = data["quat"]
					data["quat"][0] = quat[2]
					data["quat"][1] = quat[3]
					data["quat"][2] = quat[1]
					data["quat"][3] = quat[0]
		elif type == SAMP.PACKET_PLAYER_SYNC:
			if self.tool_settings["walk_distort"]:
				if "quat" in data:
					quat = data["quat"]
					data["quat"][0] = quat[2]
					data["quat"][1] = quat[3]
					data["quat"][2] = quat[1]
					data["quat"][3] = quat[0]

		self.proxy_connection.SendSync(type, data)
	def deleteAllPlayerLabels(self):
		for playerid, player in self.player_info.items():
			label_id = self.injected_labels_offset + playerid
			self.client.connection.SendRPC(SAMP.RPC_Destroy3DTextLabel, {'id': label_id})
	def updatePlayerLabel(self, player):
		label_id = self.injected_labels_offset + player["id"]
		text = "Player: {}({})\nHP: {}\nArmour: {}\nKeys: {}\nFightstyle: {}\nSkin: {}\nTeam: {}".format(player["name"],player["id"],player["health"],player["armour"], player["keys"], player["fightstyle"], player["skin"], player["team"])
		
		label_rpc = {'id': label_id, 'playerid': player["id"], 'vehicle': 65535, "test_los": 0, 'x': 0.0, 'y': 0.0, 'z': 1.0, 'drawdist': 125.0, 'colour': 0xFFFF00FF, 'text': text}
		self.client.connection.SendRPC(SAMP.RPC_Create3DTextLabel, label_rpc)