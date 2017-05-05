import SAMP

import json
import re
import uuid
import random

import _thread
import time

import decimal


from OutboundConnectionManager import OutboundConnectionManager



class ProxyClient():

	def server_conn_sync_hndlr(self, connection, type, sync_data):
		self.connection_mgr.SendSync(type, sync_data)

	def server_conn_stats_update_hndlr(self, connection, money, drunk):
		#if connection.proxy_connection != None:
			#connection.proxy_connection.SendStatsUpdate(money, drunk)
		return None

	def server_weapons_update_hndlr(self, connection, data):
		#if connection.proxy_connection != None:
		#	connection.proxy_connection.SendWeaponData(data)
		return None
	def server_conn_rpc_hndlr(self, connection, rpcid, rpc_data):
		if not "delegator" in connection.context:
			connection.context['delegator'] = connection.context['proxy_client'].getClientDelegator()

		ret = True
		if rpcid in connection.context['delegator']:
			ret = connection.context['delegator'][rpcid](connection, rpcid, rpc_data)

		if ret:
			self.connection_mgr.SendRPC(rpcid, rpc_data)
		
	def __init__(self, server, connection):
		self.connection = connection
		self.connection.context = {'server': server, 'proxy_client': self, 'connection': connection}
		self.connection.rpc_handler = (self.server_conn_rpc_hndlr)
		self.connection.sync_handler = (self.server_conn_sync_hndlr)
		self.connection.stats_update_handler = (self.server_conn_stats_update_hndlr)
		self.connection.weapons_update_handler = (self.server_weapons_update_hndlr)
		self.connection_mgr = OutboundConnectionManager(self)
		



	def handle_clientjoin_rpc(self, connection, rpcid, rpc_data):
		print("Got client join: - {}\n".format(rpc_data))

		init_data = {'unknown': False, 'gravity': 0.00800000037997961, 'disable_enter_exit': False, 'zone_names': False, 
		'num_spawn_classes': 1, 'cj_walk': False, 'firing_sendrate': 40, 'nametag_los': True, 'limit_chat_radius': False, 
		'allow_weapons': False, 'incar_sendrate': 40, 'unknown_2': 0, 'lagcomp': 1, 'lan_mode': False, 'show_nametags': True, 
		'nametag_dist': 70.0, 'server_weather': 1, 'manual_veh_lighting': False, 'unknown_3': 0, 'onfoot_sendrate': 40, 'stunt_bonus': True, 
		'hostname': 'Proxy Server (New)', 'server_hour': 12, 'send_multiplier': 2, 
		'preloaded_models': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], 
		'playerid': 0, 'chat_radius': 10000.0, 'show_player_markers': 1, 'drop_money_on_death': 0, 'unknown_4': 0}
		connection.SendRPC(SAMP.RPC_InitGame, init_data)
		
		self.dialog_handler = self.connection_mgr.showConnectionMenu()

	def handle_dialog_response(self, connection, rpcid, rpc_data):
		if self.dialog_handler:
			if self.dialog_handler(rpc_data["response"], rpc_data["selected_item"]):
				self.dialog_handler = None
			return False
		return True

	def handle_command(self, connection, rpcid, rpc_data):
		split_cmds = re.findall(r'\S+', rpc_data["Command"])
		cmd = split_cmds[0][1:]
		if cmd == "toolmenu":
			self.dialog_handler = self.connection_mgr.showToolMenu()
			return False
		return True
	#NON-PROXY MODE DELEGATORS
	#ONLY ACTIVE WHEN NOT IN PROXY MODE(INITIAL CONNECT/ON DISCONNECT)
	def getClientDelegator(self):
		ret = {}
		ret[SAMP.RPC_ClientJoin] = self.handle_clientjoin_rpc
		ret[SAMP.RPC_DialogResponse] = self.handle_dialog_response
		ret[SAMP.RPC_ClientCmd] = self.handle_command
		return ret



def server_new_conn_hndlr(server, connection, password):
	print("New conn, clients is: {}\n".format(server))
	server.context['self'].clients.append(ProxyClient(server.context['self'], connection))

#	if password == "123321":
	return SAMP.CONN_RESPONSE_REASON_ACCEPTED
#	else:
#		return SAMP.CONN_RESPONSE_REASON_INVALID_PASS

class ProxyServer(): #TODO BASE SERVER
	def __init__(self, listen_address):
		self.clients = []
		self.server = SAMP.Server()
		self.server.context = {'self': self}
		print("Server: {}\n".format(self.server))
		self.server.SetNewConnectionHandler(server_new_conn_hndlr)
		self.server.Listen(listen_address)
		