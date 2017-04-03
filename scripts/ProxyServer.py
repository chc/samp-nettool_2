import SAMP

import json
import re
import uuid
import random


#proxy mode handlers
#must live outside of proxyclient due to C code limitations
def proxy_client_rpc_hndlr(connection, rpcid, rpc_data):
	if not "proxy_delegator" in connection.context:
		connection.context['proxy_delegator'] = connection.context['proxy_client'].getProxyDelegator()

	print("Proxy got: {}\n".format(rpc_data))
	if connection.source_connection != None:
			connection.source_connection.SendRPC(rpcid, rpc_data)
	
def proxy_client_sync_hndlr(connection, type, sync_data):
	#print("S->C Got Sync: {} {}\n".format(type,sync_data))
	if connection.source_connection != None:
		connection.source_connection.SendSync(type, sync_data)	

#add to client response, check for if banned/wrong pw/server full, etc
def proxy_client_accepted(connection, playerid, challenge):
	print("Proxy Client accepted {} {} {}\n".format(connection, playerid, challenge))
	RPCData = {}
	RPCData["NetCodeVer"] = 4057
	RPCData["Challenge"] = challenge ^ RPCData["NetCodeVer"]
	RPCData["Mod"] = 1
	RPCData["Unknown"] = RPCData["Challenge"]
	RPCData["Version"] = "0.3.7-R2"
	RPCData["Name"] = "sierra"
	RPCData["GPCI"] = SAMP.GenGPCI(1001*4)
	connection.SendRPC(SAMP.RPC_ClientJoin, RPCData)


#client mode handlers
def server_conn_sync_hndlr(connection, type, sync_data):
	if connection.proxy_connection != None:
		connection.proxy_connection.SendSync(type, sync_data)
		return None

def server_conn_stats_update_hndlr(connection, money, drunk):
	if connection.proxy_connection != None:
		connection.proxy_connection.SendStatsUpdate(money, drunk)


def server_conn_rpc_hndlr(connection, rpcid, rpc_data):
	print("We got RPC: {}\n".format(rpcid))
	if connection.proxy_connection != None:
		connection.proxy_connection.SendRPC(rpcid, rpc_data)
		return None
	else:
		if not "delegator" in connection.context:
			connection.context['delegator'] = connection.context['proxy_client'].getClientDelegator()

	if rpcid in connection.context['delegator']:
		connection.context['delegator'][rpcid](connection, rpcid, rpc_data)

class ProxyClient():
	def getProxyDelegator(self):
		ret = {}
		
		return ret

		
	def __init__(self, server, connection):
		print("New proxyclient\n")
		connection.context = {'server': server, 'proxy_client': self, 'connection': connection}
		connection.rpc_handler = (server_conn_rpc_hndlr)
		connection.sync_handler = (server_conn_sync_hndlr)
		connection.stats_update_handler = (server_conn_stats_update_hndlr)
		self.connection = connection
		print("DOne proxy client\n")



	#NON-PROXY MODE DELEGATORS
	#ONLY ACTIVE WHEN NOT IN PROXY MODE(INITIAL CONNECT/ON DISCONNECT)
	def getClientDelegator(self):
		ret = {}
		ret[SAMP.RPC_ClientJoin] = self.handle_clientjoin_rpc
		ret[SAMP.RPC_ClientCmd] = self.handle_clientcmd_rpc
		ret[SAMP.RPC_SendChatMessage] = self.handle_sendchatmsg_rpc
		return ret
	def handle_clientjoin_rpc(self, connection, rpcid, rpc_data):
		print("Got client join: - {}\n".format(rpc_data))

		init_data = {'unknown': False, 'gravity': 0.00800000037997961, 'disable_enter_exit': False, 'zone_names': False, 
		'num_spawn_classes': 1, 'cj_walk': False, 'firing_sendrate': 40, 'nametag_los': True, 'limit_chat_radius': False, 
		'allow_weapons': False, 'incar_sendrate': 40, 'unknown_2': 0, 'lagcomp': 1, 'lan_mode': False, 'show_nametags': True, 
		'nametag_dist': 70.0, 'server_weather': 1, 'manual_veh_lighting': False, 'unknown_3': 0, 'onfoot_sendrate': 40, 'stunt_bonus': True, 
		'hostname': 'SA-MP 0.3 Server', 'server_hour': 12, 'send_multiplier': 2, 
		'preloaded_models': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], 
		'playerid': 0, 'chat_radius': 10000.0, 'show_player_markers': 1, 'drop_money_on_death': 0, 'unknown_4': 0}
		connection.SendRPC(SAMP.RPC_InitGame, init_data)
		connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFFFFFFFF, 'Message': 'Welcome to the proxy server, do /connect [ip:port] to go somewhere'})

	def handle_requestspawn_rpc(self, connection, rpcid, rpc_data):
		rpc_data = {'unknown': 204}
		connection.SendRPC(SAMP.RPC_RequestSpawn, rpc_data)

	def handle_clientcmd_rpc(self, connection, rpcid, rpc_data):
		print("Got command: {}\n".format(rpc_data))
		split_cmds = re.findall(r'\S+', rpc_data["Command"])
		cmd = split_cmds[0][1:]
		if cmd == "connect":
			ip_str = split_cmds[1]
			connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFFFFFFFF, 'Message': 'Connect to: {}'.format(ip_str)})
			connection.proxy_connection = SAMP.Client()
			connection.proxy_connection.source_connection = connection
			connection.proxy_connection.conn_accepted_handler = (proxy_client_accepted)
			connection.proxy_connection.rpc_handler = (proxy_client_rpc_hndlr)
			connection.proxy_connection.sync_handler = (proxy_client_sync_hndlr)
			connection.proxy_connection.context = {'proxy_client': self}
			connection.proxy_connection.Connect(ip_str)
	def handle_sendchatmsg_rpc(self, connection, rpcid, rpc_data):
		connection.SendRPC(SAMP.RPC_SendChatMessage, {'playerid': 0, "Message": rpc_data["Message"]})
	def handle_requestclass_rpc(self, connection, rpcid, rpc_data):
		rpc_data = {'unknown': 1, 'team': 255, 'skin': 122,'x':-2000.0,'y':150.0,'z':0.0,'z_angle':0.0,'weapon_1': 24,'weapon_2': 0,'weapon_3': 0, 'ammo_1': 1000, 'ammo_2': 0, 'ammo_3': 0, 'unknown2': 0}
		connection.SendRPC(SAMP.RPC_RequestClass, rpc_data)


#must exist outside ProxyServer due to C implementation limitations
def server_new_conn_hndlr(server, connection):
	server.context.clients.append(ProxyClient(server.context, connection))

class ProxyServer(): #TODO BASE SERVER

	def __init__(self, listen_address):
		self.clients = []
		self.server = SAMP.Server()
		print("Server: {}\n".format(self.server))
		self.server.context = self
		self.server.SetNewConnectionHandler(server_new_conn_hndlr)
		self.server.Listen(listen_address)
		
		











def getClientProxiedDelegator():
	delegator = {}
	return delegator
