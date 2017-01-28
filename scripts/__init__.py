import SAMP

import json
import re
import uuid
import random

server = SAMP.Server()
server.Listen("127.0.0.1:7774")

def proxy_client_accepted(connection, playerid, challenge):
	print("Client accepted {} {} {}\n".format(connection, playerid, challenge))
	RPCData = {}
	RPCData["NetCodeVer"] = 4057
	RPCData["Challenge"] = challenge ^ RPCData["NetCodeVer"]
	RPCData["Mod"] = 1
	RPCData["Unknown"] = RPCData["Challenge"]
	RPCData["Version"] = "0.3.7-R2"
	RPCData["Name"] = "sierra"
	RPCData["GPCI"] = SAMP.GenGPCI(1001*4)
	connection.SendRPC(SAMP.RPC_ClientJoin, RPCData)


	
def proxy_client_sync_hndlr(connection, type, sync_data):
	return None
	#print("Got Sync: {} {}\n".format(type,sync_data))
	#if connection.source_connection != None:
		#connection.source_connection.SendSync(type, sync_data)	
def proxy_client_rpc_hndlr(connection, rpcid, rpc_data):
	print("Got RPC: {} {} || {} {}\n".format(connection.source_connection,SAMP.RPC_InitGame,rpcid,rpc_data))
	if rpcid == SAMP.RPC_InitGame:
		print("Got game init, send req class {}\n".format(connection.proxy_connection))
		connection.SendRPC(SAMP.RPC_RequestClass, {'class_id': 0})
		#connection.SendRPC(SAMP.RPC_SendClientMessage, {'Message': 'Connect to: {}'.format(ip_str)})
	if connection.source_connection != None:
		connection.source_connection.SendRPC(rpcid, rpc_data)

def server_conn_stats_update_hndlr(connection, money, drunk):
	if connection.proxy_connection != None:
		connection.proxy_connection.SendStatsUpdate(money, drunk)

def server_conn_rpc_hndlr(connection, rpcid, rpc_data):
	print("Proxy: {}\n".format(connection.proxy_connection))
	if connection.proxy_connection != None:
		connection.proxy_connection.SendRPC(rpcid, rpc_data)
		return None
	if rpcid == SAMP.RPC_ClientJoin:
		playerid = 1
		#playerid = connection.playerid
		
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
		#connection.SendRPC(SAMP.RPC_ConnectionRejected, {'reason':3})
	elif rpcid == SAMP.RPC_RequestClass:
		print("Got req class: {}\n".format(rpc_data))
		rpc_data = {'unknown': 1, 'team': 255, 'skin': 20,'x':0.0,'y':0.0,'z':0.0,'z_angle':0.0,'weapon_1': 0,'weapon_2': 0,'weapon_3': 0, 'ammo_1': 0, 'ammo_2': 0, 'ammo_3': 0}
		connection.SendRPC(SAMP.RPC_RequestClass, rpc_data)
	elif rpcid == SAMP.RPC_RequestSpawn:
		connection.SendRPC(SAMP.RPC_RequestSpawn, rpc_data)
	elif rpcid == SAMP.RPC_ClientCmd:
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
			connection.proxy_connection.Connect(ip_str)

			
def server_conn_sync_hndlr(connection, type, sync_data):
	if connection.proxy_connection != None:
		print("Got sync: {}\n".format(sync_data))
		connection.proxy_connection.SendSync(type, sync_data)
		return None
	
	
def server_new_conn_hndlr(server, connection):
	connection.rpc_handler = (server_conn_rpc_hndlr)
	connection.sync_handler = (server_conn_sync_hndlr)
	connection.stats_update_handler = (server_conn_stats_update_hndlr)
	
server.SetNewConnectionHandler(server_new_conn_hndlr)


#conn = SAMP.Client()
#conn.conn_accepted_handler = (proxy_client_accepted)
#conn.rpc_handler = (proxy_client_rpc_hndlr)
#conn.sync_handler = (proxy_client_sync_hndlr)
#conn.Connect("127.0.0.1:7777")