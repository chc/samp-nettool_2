import SAMP

import json
import re
import uuid
import random

class OutboundConnectionManager():
	def proxy_client_rpc_hndlr(self, connection, rpcid, rpc_data):
		#if not "proxy_delegator" in connection.context:
		#	connection.context['proxy_delegator'] = connection.context['proxy_client'].getProxyDelegator()

		print("Got: {} {}\n".format(rpcid, rpc_data))
		self.client.connection.SendRPC(rpcid, rpc_data)
		
	def proxy_client_sync_hndlr(self, connection, type, sync_data):
		self.client.connection.SendSync(type, sync_data)	

	#add to client response, check for if banned/wrong pw/server full, etc
	def proxy_client_accepted(self, connection, playerid, challenge):
		print("Proxy Client accepted {} {} {}\n".format(connection, playerid, challenge))
		RPCData = {}
		RPCData["NetCodeVer"] = 4057
		RPCData["Challenge"] = challenge ^ RPCData["NetCodeVer"]
		RPCData["Mod"] = 1
		RPCData["Unknown"] = RPCData["Challenge"]
		RPCData["Version"] = "0.3.7-R2"
		RPCData["Name"] = "Nardi"
		RPCData["GPCI"] = SAMP.GenGPCI(1001*4)
		self.proxy_connection.SendRPC(SAMP.RPC_ClientJoin, RPCData)

		RPCData = {'class_id': 0}
		self.proxy_connection.SendRPC(SAMP.RPC_RequestClass, RPCData)

	def __init__(self, client):
		self.tool_settings = {}
		self.tool_settings_entries = [
			{'name': 'Health Hacks', 'key': 'health_hacks'},
			{'name': 'Wall Hacks', 'key': 'wall_hacks'},
			{'name': 'Aimbot(Non-locking)', 'key': 'nonlocking_aimbot'},
			{'name': 'Walking Distort', 'key': 'walk_distort'},
			{'name': 'Force local chat', 'key': 'local_chat'},
			{'name': 'Disable vehicle collision', 'key': 'veh_collision'},
			{'name': 'Force time of day', 'on_select': self.showTimeOfDayMenu},
			{'name': 'Force Weather', 'on_select': self.showWeatherMenu},
			{'name': 'Vehicle speed boost(Horn)', 'key': 'veh_speed_boost'},
			{'name': 'Horn Spammer', 'key': 'veh_horn_spam'},
			{'name': 'Vehicle Override', 'on_select': self.showVehicleOverrideMenu},			
			{'name': 'Connect/Quit messages', 'key': 'connect_quit_msgs'},			
			{'name': 'Toggle Spectator mode', 'key': 'spec_mode'},
			{'name': 'Block paused', 'key': 'block_paused'},
			{'name': 'Object finder', 'key': 'object_finder'},
			{'name': 'Textdraw blocker', 'key': 'textdraw_blocker'},			
			{'name': 'Chat morpher', 'key': 'chat_morph'}, #morph menu
			{'name': 'Ping bugger', 'key': 'ping_bugger'}, #65535 ping, 0 ping
			{'name': 'RPC Control', 'on_select': self.showRPCBlockerMenu}, #dump RPC in/out, block RPCs, spoof RPCs
			{'name': 'Server Specifics', 'on_select': self.showServerSpecificMenu},

		]

		for setting in self.tool_settings_entries:
			if "key" in setting:
				self.tool_settings[setting["key"]] = False

		self.servers = [
			{'name': 'UIF', 'connection_string': '51.254.85.134:7776'},
			{'name': 'WC-RP', 'connection_string': 'samp.wc-rp.com:7777'},
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
		item = self.tool_settings_entries[list_index]

		self.tool_settings[item["key"]] = not self.tool_settings[item["key"]]
		self.client.connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': "Tool menu: {}".format(item['name']), "Colour": 0xFFFFFFFF})
		return False
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


	def SendRPC(self, rpc_id, rpc_data):
		self.proxy_connection.SendRPC(rpc_id, rpc_data)
	def SendSync(self, type, data):
		if type == SAMP.PACKET_VEHICLE_SYNC:
			if self.tool_settings["veh_horn_spam"]:
				if data["keys"] == 0:
					data["keys"] = 2
		self.proxy_connection.SendSync(type, data)