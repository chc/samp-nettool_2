import SAMP

import json
import re
import uuid
import random

import _thread
import time

import decimal

clients = []

def sync_thread():
	while True:
		for client in clients:
			for player in client.players:
				if "incar" in player and player['incar'] > 0:
					rpc_details = {'playerid': player['id'], 'leftright_keys': 0, 'updown_keys': 0, 'keys': 0, 'pos': player['pos'], 'quat': [decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random()))], 'vel': [0.0,0.0,0.0], 'health': 1000.0, 'player_armour': 50, 'player_health': 66, 'weapon': 38, 'specialaction': 0, 'surf_flags': 0, 'surf_offset': [0.0,0.0,0.0], 'anim': 0, 'vehicleid': player['incar'], "angle": 0, "hydra": False, "trailer": False, "train": False, "landinggear_state": True, "train_speed": 100.0}
					client.connection.SendSync(SAMP.PACKET_VEHICLE_SYNC, rpc_details)
				else:
					rpc_details = {'playerid': player['id'], 'leftright_keys': 0, 'updown_keys': 0, 'keys': 0, 'pos': player['pos'], 'quat': [decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random())),decimal.Decimal(str(random.random()))], 'vel': [0.0,0.0,0.0], 'health': 100, 'armour': 50, 'weapon': 38, 'specialaction': 0, 'surf_flags': 0, 'surf_offset': [0.0,0.0,0.0], 'anim': 0}
					client.connection.SendSync(SAMP.PACKET_PLAYER_SYNC, rpc_details)
		time.sleep(16.0 / 1000.0)

#proxy mode handlers
#must live outside of proxyclient due to C code limitations
def proxy_client_rpc_hndlr(connection, rpcid, rpc_data):
	if not "proxy_delegator" in connection.context:
		connection.context['proxy_delegator'] = connection.context['proxy_client'].getProxyDelegator()

	#if rpcid == SAMP.RPC_SendPlayerDialog:
	print("Proxy got: {} {}\n".format(rpcid, rpc_data))
	if connection.source_connection != None:
		connection.source_connection.SendRPC(rpcid, rpc_data)
	
proxy_last_anim_num = 0
last_anim_num = 0
def proxy_client_sync_hndlr(connection, type, sync_data):
	if type == SAMP.PACKET_MARKERS_SYNC:
		print("S->C Got Sync: {} {}\n".format(type,sync_data))
	global proxy_last_anim_num
	if connection.source_connection != None:
		if "quat" in sync_data and connection.source_connection.context['proxy_client'].distort_quat:
			quat = sync_data["quat"]
			sync_data["quat"][0] = quat[2]
			sync_data["quat"][1] = quat[3]
			sync_data["quat"][2] = quat[1]
			sync_data["quat"][3] = quat[0]
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

	RPCData = {'class_id': 0}
	connection.SendRPC(SAMP.RPC_RequestClass, RPCData)




#client mode handlers
def server_conn_sync_hndlr(connection, type, sync_data):
	global last_anim_num
	if connection.proxy_connection != None:
		#print("Send sync: {} {}\n".format(type, sync_data))
		#if "keys" in sync_data and sync_data["keys"] == 0: #horn spam
			#sync_data["keys"] =  2
		if "quat" in sync_data and connection.context['proxy_client'].distort_quat:
			quat = sync_data["quat"]
			sync_data["quat"][0] = quat[2]
			sync_data["quat"][1] = quat[3]
			sync_data["quat"][2] = quat[1]
			sync_data["quat"][3] = quat[0]
		connection.proxy_connection.SendSync(type, sync_data)
		return None

def server_conn_stats_update_hndlr(connection, money, drunk):
	if connection.proxy_connection != None:
		connection.proxy_connection.SendStatsUpdate(money, drunk)

def server_weapons_update_hndlr(connection, data):
	if connection.proxy_connection != None:
		connection.proxy_connection.SendWeaponData(data)
def server_conn_rpc_hndlr(connection, rpcid, rpc_data):
	print("We got RPC: {} {}\n".format(rpcid, rpc_data))
	if connection.proxy_connection != None:
		if not "proxied_delegator" in connection.context:
			connection.context['proxied_delegator'] = connection.context['proxy_client'].getClientProxiedDelegator()
		if rpcid in connection.context['proxied_delegator']:
			if not connection.context['proxied_delegator'][rpcid](connection, rpcid, rpc_data):
				return None
		connection.proxy_connection.SendRPC(rpcid, rpc_data)
		return None
	else:
		if not "delegator" in connection.context:
			connection.context['delegator'] = connection.context['proxy_client'].getClientDelegator()

	if rpcid in connection.context['delegator']:
		connection.context['delegator'][rpcid](connection, rpcid, rpc_data)


def id_generator(size, chars):
	return ''.join(random.choice(chars) for _ in range(size))

class ProxyClient():

	def getProxyDelegator(self):
		ret = {}
		
		return ret

		
	def __init__(self, server, connection):
		global clients
		print("New proxyclient\n")
		connection.context = {'server': server, 'proxy_client': self, 'connection': connection}
		connection.rpc_handler = (server_conn_rpc_hndlr)
		connection.sync_handler = (server_conn_sync_hndlr)
		connection.stats_update_handler = (server_conn_stats_update_hndlr)
		connection.weapons_update_handler = (server_weapons_update_hndlr)
		self.connection = connection
		self.distort_quat = False
		self.players = []
		clients.append(self)
		print("Done proxy client\n")



	def get_random_name(self):
		chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
		return id_generator(20, chars)
	#debug stuff
	def init_fake_players(self):
		self.vehicles = [
			{'id': 1, 'modelid': 411, 'x':-2017.87, 'y': 200.0, 'z': 27.54, 'zrot': 0.0, 'col1': 41, 'col2': 11, 'health': 1000.0, 'interior': 0, 'door_damage': 0, 'panel_damage': 0, 'light_damage': 0, 'tire_damage': 0, 'siren': 0, 'comp1': 0,'comp2': 0,'comp3': 0,'comp4': 0,'comp5': 0,'comp6': 0,'comp7': 0,'comp8': 0,'comp9': 0,'comp10': 0,'comp11': 0,'comp12': 0,'comp13': 0,'comp14': 0, 'col32_1': 0, 'col32_2': 0, 'unknown': 0},
		]
		self.players = [
		#-2007.87 174.93 27.54
			{'name': 'Joe_Bob', 'score': 1111, 'ping': 666, 'skin': 200, 'pos': [-2007.87, 174.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255, 'id': 51},
			{'name': 'Jeff_CarDriver', 'score': 2111, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255, 'incar': 1, 'id': 50},
		#	{'name': 'asdasd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'asxasd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'asdasd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'assdfd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'aadfgd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'agcasd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'gfdgsd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'a435sd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'as23sd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': 'asfsdf', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
		#	{'name': '234asd', 'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 134.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255},
			
		]

		template_user = {'score': 2111, 'ping': 1337, 'skin': 122, 'pos': [-2017.87, 144.93, 27.54], 'z_angle': 0.0, 'fightstyle': 0, 'team': 255}
		y_offset = 1.5
		id_offset = 300
		count = 2

		the_players = self.players

		for x in range(0, 50):
			id_offset = id_offset + 1
			template_user['id'] = id_offset
			template_user["skin"] = random.randrange(0,311, count)
			name = "ID_{}".format(id_offset)
			y_offset = y_offset + 1.5
			template_user["pos"][1] = template_user["pos"][1] + y_offset
			template_user["name"] = "ID_{}".format(id_offset)
			dic_copy = template_user.copy()
			dic_copy["pos"] = dic_copy["pos"].copy()
			self.players.append(dic_copy)

		for player in self.players:
			
			
			count = count + 10
			random.seed(count)


				#player["name"] = self.get_random_name()
			player['pos'][1] = player['pos'][1] + y_offset
			rpc_details = {'id': player['id'], 'colour': 0xFFFFFFFF, 'x': player['pos'][0], 'y': player['pos'][1], 'z': player['pos'][2], 'z_angle': player['z_angle'], 'fightstyle': player['fightstyle'], 'skin': player['skin'], 'team': player['team'], 'npc': 0, 'name': player['name']}
			self.connection.SendRPC(SAMP.RPC_ServerJoin, rpc_details)
			self.connection.SendRPC(SAMP.RPC_AddPlayerToWorld, rpc_details)

		for veh in self.vehicles:
			self.connection.SendRPC(SAMP.RPC_VehicleCreate, veh)
	####

	#PROXY MODE C-> DELEGATORS
	#ONLY ACTIVE WHEN IN PROXY NODE
	def getClientProxiedDelegator(self):
		ret = {}
		ret[SAMP.RPC_ClientCmd] = self.handle_clientcmd_rpc_proxied
		ret[SAMP.RPC_ClientCheck] = self.handle_clientcheck_rpc_proxied
		return ret

	def handle_clientcheck_rpc_proxied(self, connection, rpcid, rpc_data):
		connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFF0000FF, 'Message': 'CLIENT CHECK: {}'.format(self.rpc_data)})
		return True
	def handle_clientcmd_rpc_proxied(self, connection, rpcid, rpc_data):
		split_cmds = re.findall(r'\S+', rpc_data["Command"])
		cmd = split_cmds[0][1:]
		if cmd == "distort":
			self.distort_quat = not self.distort_quat
			connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFFFFFFFF, 'Message': 'Distort mode set to: {}'.format(self.distort_quat)})
			return False
		return True

	#NON-PROXY MODE DELEGATORS
	#ONLY ACTIVE WHEN NOT IN PROXY MODE(INITIAL CONNECT/ON DISCONNECT)
	def getClientDelegator(self):
		ret = {}
		ret[SAMP.RPC_ClientJoin] = self.handle_clientjoin_rpc
		ret[SAMP.RPC_ClientCmd] = self.handle_clientcmd_rpc
		ret[SAMP.RPC_SendChatMessage] = self.handle_sendchatmsg_rpc
		ret[SAMP.RPC_RequestClass] = self.handle_requestclass_rpc
		ret[SAMP.RPC_RequestSpawn] = self.handle_requestspawn_rpc
		ret[SAMP.RPC_ScoreboardSelectPlayer] = self.handle_select_scoreboard_player_rpc
		return ret

	
	def handle_select_scoreboard_player_rpc(self, connection, rpcid, rpc_data):
		connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFFFFFFFF, 'Message': 'You selected {}'.format(rpc_data["id"])})
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
		connection.SendRPC(SAMP.RPC_SendClientMessage, {'Colour': 0xFFFFFFFF, 'Message': 'Welcome to the proxy server, do /connect [ip:port] to go somewhere'})

	def handle_requestspawn_rpc(self, connection, rpcid, rpc_data):
		rpc_data = {'unknown': 204}
		connection.SendRPC(SAMP.RPC_RequestSpawn, rpc_data)

		self.init_fake_players()

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
		elif cmd == "test":
			send_rpc = {'id': 4661, 'msg': '/basickeys - keys to get you started\n/savestats - save your stats\n/pickupgun - pickup dropped gun\n/dropgun - drop your current weapon\n/survival - blood, hunger, thirst etc\n/version - SA:MP version\n/getid - Get a persons ID\n/stopambient - turn off ambient\n/firstperson - first person view\n/ask - Use this when you need help\n/r - send a radio message\n/ooc- Global Chat (needs radio device)\n/me - self expression command\n/shout - yell :)\n/local - local chat\n/pm - Private message someone\n/blockpm & /unblockpm - self explained\n/inv - show inventory\n/acmds - Admin Commands\n/rules - List of server rules\n', 'button_2': '', 'button_1': 'Close', 'type': 0, 'title': 'DayZ - General CMDSs'}
			connection.SendRPC(SAMP.RPC_SendPlayerDialog, send_rpc)
	def handle_sendchatmsg_rpc(self, connection, rpcid, rpc_data):
		connection.SendRPC(SAMP.RPC_SendChatMessage, {'playerid': 0, "Message": rpc_data["Message"]})
	def handle_requestclass_rpc(self, connection, rpcid, rpc_data):
		rpc_data = {'unknown': 1, 'team': 255, 'skin': 122,'x':-2000.0,'y':150.0,'z':30.0,'z_angle':0.0,'weapon_1': 24,'weapon_2': 0,'weapon_3': 0, 'ammo_1': 1000, 'ammo_2': 0, 'ammo_3': 0, 'unknown2': 0}
		connection.SendRPC(SAMP.RPC_RequestClass, rpc_data)


#must exist outside ProxyServer due to C implementation limitations
def server_new_conn_hndlr(server, connection, password):

	server.context.clients.append(ProxyClient(server.context, connection))

#	if password == "123321":
	return SAMP.CONN_RESPONSE_REASON_ACCEPTED
#	else:
#		return SAMP.CONN_RESPONSE_REASON_INVALID_PASS

class ProxyServer(): #TODO BASE SERVER

	def __init__(self, listen_address):
		self.clients = []
		self.server = SAMP.Server()
		print("Server: {}\n".format(self.server))
		self.server.context = self
		self.server.SetNewConnectionHandler(server_new_conn_hndlr)
		self.server.Listen(listen_address)
		
#_thread.start_new_thread(sync_thread, ())