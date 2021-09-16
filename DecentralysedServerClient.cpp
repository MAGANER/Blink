#include "DecentralysedServerClient.h"
using namespace Blink;

DecentralysedServerClient::DecentralysedServerClient(command_hash& commands,
	const string& password,
	const string& room_name,
	const string& user_name,
	const string& db_key,
	const string& db_name,
	bool connecting_with_conflink_command,
	bool inherited) :Server(commands,password,room_name,user_name,db_key,db_name,inherited)
{
	while (true)
	{
		if (connecting_with_conflink_command)
		{
			port = get_random_port();
			_cant_connect = false;
			break;
		}

		cout << "if you start decentralysed room, then create link(1).\n" <<
			"if you connect to existing decentralysed room(2)\n";
		cout << ">>";
		string mode;
		cin >> mode;
		if (mode.size() != 1)
		{
			mode.clear();
			continue;
		}
		else if (mode == "1")
		{
			if (!does_conn_info_exist(room_name))
				create_room_connections_info(room_name);

			port = get_room_port(room_name, db_key, db_name);
			save_own_port(room_name, port);
			create_invite_link(port, room_name, password,true);
			
			auto key = encr::AES::convert_bytes(key_iv.first);
			auto iv = encr::AES::convert_bytes(key_iv.second);
			save_room_key(room_name, make_pair(key, iv));

			break;
		}
		else if (mode == "2")
		{
			port = get_own_port(room_name);
			if (port == -1 && !connecting_with_conflink_command)
				_cant_connect = true;

			auto key_iv_val = get_key_iv(room_name);
			key_iv.first  = encr::AES::convert_to_bytes(key_iv_val.first);
			key_iv.second = encr::AES::convert_to_bytes(key_iv_val.second);

			auto saved_clients = get_connections_info(room_name);
			for (auto& client : saved_clients)
			{
				allowed.push_back(client.first);
			}

			connect_to_saved_clients = true;
			_cant_connect = false;
			break;
		}
		else
		{
			mode.clear();
			continue;
		}
	}
}
DecentralysedServerClient::~DecentralysedServerClient()
{
	if (socket != nullptr)delete socket;
}
bool DecentralysedServerClient::_run()
{
	listener.setBlocking(false);
	listener.listen(port);
	cout << "port=" << listener.getLocalPort()<<" "<<port<< endl;

	//connect again if you are not "holder"
	if (connecting && !connect_to_saved_clients)
	{
		socket = new TcpSocket();
		auto address = IpAddress(conn_ip.c_str());
		auto port = atoi(conn_port.c_str());
		socket->connect(address, port);

		auto nothing = get_message(*socket);
		//update correct room name
		if (received_room_name)
		{
			room_name = *correct_room_name;
			received_room_name = false;
			delete correct_room_name;
			save_own_port(room_name, this->port);

			//create it at connected side
			if (!does_conn_info_exist(room_name))
			{
				create_room_connections_info(room_name);
			}

			//save it if it's not saved
			auto key = encr::AES::convert_bytes(key_iv.first);
			auto iv = encr::AES::convert_bytes(key_iv.second);
			save_room_key(room_name,make_pair(key,iv));
		}

		//send your listner port
		Packet pack;
		pack << to_string(this->port);
		socket->send(pack);

		//save connected as client
		make_client(clients, client_counter, socket, port);
	}

	if (connect_to_saved_clients)
	{
		connect_to_known_clients();
		connect_to_saved_clients = false;
	}


	TcpSocket* entering_socket = new TcpSocket;
	while (true)
	{
		receive_input_and_send_message_to_all(clients);
		
		bool should_resend_clients_info = true;
		if (listener.accept(*entering_socket) == sf::Socket::Done)
		{
			bool check1 = is_addres_allowed(allowed, entering_socket->getRemoteAddress());
			if (check1 && true && can_accept_new_connection(client_counter))
			{
				// send room name, because room name from file link is hashed
				send_message(*entering_socket, room_name, MessageType::RoomName);

				//retrieve port, cos it's needed to create clients' info
				int listner_port = -1;
				string some_shit = get_raw_message(*entering_socket);
				if (some_shit == "motherfucker") should_resend_clients_info = false;
				else listner_port = atoi(some_shit.c_str());

				cout << "listner port received:" << listner_port << endl;
				//don't resend if you connect to user
				//who already was connected to another one
				if(should_resend_clients_info)send_clients_info(clients, entering_socket);
				make_client(clients, client_counter, entering_socket,listner_port);
				

				//clear it, because there is no need to keep this data anymore
				//for (size_t i = 0; i < allowed.size(); i++)
				//{
				//	auto curr = allowed[i];
				//	if (curr.toString() == entering_socket->getRemoteAddress().toString())
				//	{
				//		allowed.erase(allowed.begin() + i);
				//	}
				//}
				entering_socket = new TcpSocket();
			}
			else
			{
				check_access(*entering_socket, allowed);
			}
		}

		//receive message, show it and resend
		for (auto& client : clients)
			NetBase::get_and_show_message(*client->socket);

		//when you connect to another client,
		//it resends its data about another connected clients
		if (received_clients_info)
		{
			//connect to existing users
			ConnectionData conn_data;
			//should hash data, cos the same data is hashed in links
			conn_data.password = encr::SHA::sha256(received_info->room_password);
			conn_data.room = encr::SHA::sha256(received_info->room_name);
			for (auto& client : received_info->clients)
			{
				conn_data.ip = client.first.toString();
				conn_data.port = to_string(client.second);

				if(ConnectionChecker::can_connect(conn_data))
				{
					TcpSocket* socket = new TcpSocket;
					socket->setBlocking(false);
					socket->connect(client.first, client.second);
				
					//send the key word to say
					//don't resend client info
					Packet pack;
					pack << "motherfucker";
					socket->send(pack);
														
					int listner_port = atoi(conn_data.port.c_str());
					make_client(clients,client_counter, socket, listner_port);
				}
			}
			//create room
			if (!does_room_exists(received_info->room_name))
			{
				create_new_room(received_info->room_name,
								received_info->room_password,
								to_string(port),
								RoomNetworkMode::Decentralysed);
			}
			
			received_clients_info = false;
		}



		update_clients(clients);
		if (should_disconnect())return true;
	}

	if (entering_socket != nullptr) delete entering_socket;

	return true;
}
void DecentralysedServerClient::set_ip_and_port_to_connect(const string& ip, const string& port)
{
	conn_ip = ip;
	conn_port = port;
}
void DecentralysedServerClient::is_connecting(bool flag)
{
	connecting = flag;
}
void DecentralysedServerClient::set_key_iv(const encr::AES::key_iv& key_iv)
{
	this->key_iv = key_iv;
}
void DecentralysedServerClient::make_client(list<RoomClient*>& clients, 
										    int& client_counter,
											TcpSocket* socket,
											int listner_port)
{
	RoomClient* client = new RoomClient;

	client->socket = move(socket);
	client->socket->setBlocking(false);
	client->listner_port = listner_port;
	client->id = client_counter;

	clients.push_back(client);
	client_counter++;

	auto ip_port = make_pair(client->socket->getRemoteAddress().toString(),
							 to_string(listner_port));

	//save only if it's not saved and port is correct value
	bool are_saved = !are_ip_port_saved(ip_port, room_name);
	bool is_port_correct = listner_port != -1;
	bool is_ip_ok = client->socket->getRemoteAddress().toInteger() != 0;
	if(are_saved && is_port_correct && is_ip_ok)
		add_connection_info(room_name, ip_port);
}
void DecentralysedServerClient::send_clients_info(list<RoomClient*>& clients,
												  TcpSocket* socket)
{
	string message = create_clients_info_message(clients, room_name, password);
	socket->setBlocking(true);
	send_jmessage(*socket, message);
	socket->setBlocking(false);
}
int DecentralysedServerClient::get_random_port()
{
	random_device rd;   
	mt19937 gen(rd());  
	uniform_int_distribution<> dist(1, 9);
						
	string port;
	for (int i = 0; i < 5; ++i) 
	{
		port += to_string(dist(gen));
	}
	return atoi(port.c_str());
}
void DecentralysedServerClient::connect_to_known_clients()
{
	auto info = get_connections_info(room_name);
	if (info.empty())
	{
		return;
		//because there is no one
	}

	for (auto& client_data : info)
	{
		socket = new TcpSocket();
		cout << client_data.first << ":" << atoi(client_data.second.c_str()) << endl;
		auto s = socket->connect(IpAddress(client_data.first), atoi(client_data.second.c_str()));

		Packet pack;
		pack << "motherfucker";
		socket->send(pack);
		make_client(clients, client_counter, socket, atoi(client_data.second.c_str()));
	}
}
