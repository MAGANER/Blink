#include "DecentralysedServerClient.h"
using namespace Blink;

DecentralysedServerClient::DecentralysedServerClient(command_hash& commands,
	const string& password,
	const NetBaseData& data,
	bool connecting_with_conflink_command,
	bool starting_room,
	bool inherited) :Server(commands,password,data,inherited)
{
	while (true)
	{
		if (connecting_with_conflink_command)
		{
			_cant_connect = false;
			break;
		}

		string mode = starting_room?"1":"2";
		if (mode.size() != 1)
		{
			mode.clear();
			continue;
		}
		else if (mode == "1")
		{
			if (!does_conn_info_exist(data.room_name))
				create_room_connections_info(data.room_name);

			create_invite_link(PORT, data.room_name, password,true);
			
			auto key = encr::AES::convert_bytes(key_iv.first);
			auto iv = encr::AES::convert_bytes(key_iv.second);
			save_room_key(data.room_name, make_pair(key, iv));

			break;
		}
		else if (mode == "2")
		{
			//keys are still the same
			auto key_iv_val = get_key_iv(data.room_name);
			key_iv.first  = encr::AES::_convert_to_bytes(key_iv_val.first);
			key_iv.second = encr::AES::_convert_to_bytes(key_iv_val.second);

			//get offline clients
			auto saved_offline_clients = get_offline_clients(data.room_name);
			for (auto& client : saved_offline_clients)
			{
				offline_clients.push_back(client);
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
DecentralysedServerClient::
DecentralysedServerClient(command_hash& commands,
						  const string& password,
						  const NetBaseData& data,
						  const string& link_creator_data,
						  bool starting_room,
						  const string& recepient_name,
						  bool save_link,
						  bool inherited) :Server(commands, password, data, inherited)
{
	this->starting_room = starting_room;

	string mode = starting_room ? "1" : "2";
	if (mode == "1")
	{
		if (!does_conn_info_exist(data.room_name))
			create_room_connections_info(data.room_name);

		if (save_link)
			create_invite_link_to_save(PORT, data.room_name, password, true, link_creator_data, recepient_name);
		else
			create_invite_link_to_send(PORT, data.room_name, password, true, link_creator_data,recepient_name);

		auto key = encr::AES::convert_bytes(key_iv.first);
		auto iv = encr::AES::convert_bytes(key_iv.second);
		save_room_key(data.room_name, make_pair(key, iv));
	}
	else if (mode == "2")
	{
		//keys are still the same
		auto key_iv_val = get_key_iv(data.room_name);
		key_iv.first = encr::AES::_convert_to_bytes(key_iv_val.first);
		key_iv.second = encr::AES::_convert_to_bytes(key_iv_val.second);
		//get offline clients
		auto saved_offline_clients = get_offline_clients(data.room_name);
		for (auto& client : saved_offline_clients)
		{
			offline_clients.push_back(client);
		}

		connect_to_saved_clients = !get_connections_info(room_name).empty();
		_cant_connect = false;
	}
}
DecentralysedServerClient::~DecentralysedServerClient()
{
	if (socket != nullptr)delete socket;
}

void DecentralysedServerClient::prepare()
{
	load_allowed_long_known_clients();

	listener.setBlocking(false);
	listener.listen(PORT);


	if (connect_to_saved_clients)
	{
		connect_to_known_clients();
		connect_to_saved_clients = false;
	}
	else
	{
		if(!starting_room)connnect_finally();
	}
	
}
bool DecentralysedServerClient::run_in_console()
{
	while (true)
	{
		receive_input_and_send_message_to_all(clients);
		
		bool should_resend_clients_info = true;
		if (listener.accept(*entering_socket) == sf::Socket::Done)
		{
			auto check_addres = [&](vector<IpAddress>& addresses)
			{
				return is_addres_allowed(addresses, entering_socket->getRemoteAddress());
			};

			if (check_addres(allowed) &&
				can_accept_new_connection(client_counter))
			{
				//send password because it was hashed
				Packet pack;
				pack << password;
				entering_socket->send(pack);

				string some_shit = get_raw_message(*entering_socket);
				if (some_shit == "motherfucker") should_resend_clients_info = false;

				//don't resend if you connect to user
				//who already was connected to another one
				if (should_resend_clients_info)
					send_clients_info(clients, entering_socket);


				//save address, because client can log out and came back
				//so save it, because ip is added to DB, but not to allowed vector
				allowed.push_back(entering_socket->getRemoteAddress());
				make_client(clients, client_counter, entering_socket, PORT);
				entering_socket = new TcpSocket();
			}
			else
			{
				check_access(*entering_socket, allowed);
			}
		}

		//receive message, show it and resend
		for (auto& client : clients)
			NetBase::get_and_save_message(*client->socket, received_messages,true);

		//info about another clients
		process_received_clients_info();

		if (timer.getElapsedTime().asSeconds() > 10.0f)
		{
			check_offline_clients();
			timer.restart();
		}

		//update_clients(clients);
		if (should_disconnect())
		{
			//save offline clients
			//because if you don't do that you lost all data  about potential
			//clients
			listener.close();
			socket->disconnect();

			return true;
		}
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

	auto ip = client->socket->getRemoteAddress().toString();

	//save only if it's not saved and port is correct value
	bool are_saved = !is_ip_saved(ip, room_name);
	bool is_ip_ok = client->socket->getRemoteAddress().toInteger() != 0;
	if (are_saved &&  is_ip_ok)
	{
		add_connection_info(room_name, ip);
	}
}
void DecentralysedServerClient::send_clients_info(list<RoomClient*>& clients,
												  TcpSocket* socket)
{
	string message = create_clients_info_message(clients, room_name, password);
	socket->setBlocking(true);
	send_jmessage(*socket, message);
	socket->setBlocking(false);
}
void DecentralysedServerClient::connect_to_known_clients()
{
	for (auto& ip : allowed)
	{
		socket = new TcpSocket();

		auto timeout = sf::seconds(2.0f);
		auto conn_result = socket->connect(ip, PORT,timeout);
		if (conn_result == TcpSocket::Done)
		{
			password = get_raw_message(*socket);
			Packet p; p << "motherfucker";
			socket->send(p);
		
			make_client(clients, client_counter, socket, PORT);
		}
		else
		{
			cout << "can not connect again. SFML code:" << conn_result << endl;
		}
	}
}
void DecentralysedServerClient::process_received_clients_info()
{
	//when you connect to another client,
	//it resends its data about another connected clients
	if (received_clients_info)
	{
		//connect to existing users
		ConnectionData conn_data;
		//should hash data, cos the same data is hashed in links
		conn_data.password = encr::SHA::sha256(received_info->room_password);
		conn_data.room = received_info->room_name;
		for (auto& client : received_info->clients)
		{
			conn_data.ip = client.first.toString();

			if (ConnectionChecker::can_connect(conn_data))
			{
				TcpSocket* socket = new TcpSocket;
				socket->setBlocking(false);
				socket->connect(client.first, PORT);

				//send the key word to say
				//don't resend client info
				Packet pack;
				pack << "motherfucker";
				socket->send(pack);

				make_client(clients, client_counter, socket, PORT);
			}
			else
			{
				//save offline client
				offline_clients.push_back(client.first.toString());
			}
		}
		//create room
		if (!does_room_exists(received_info->room_name))
		{
			create_new_room(received_info->room_name,
							received_info->room_password);
		}

		received_clients_info = false;
		delete received_info;
	}
}
void DecentralysedServerClient::check_offline_clients()
{
	//connect to existing users
	ConnectionData conn_data;
	//should hash data, cos the same data is hashed in links
	conn_data.password = encr::SHA::sha256(password);
	conn_data.room =room_name;
	for (size_t i = 0;i<offline_clients.size();++i)
	{
		auto& client = offline_clients[i];
		conn_data.ip = client;

		if (ConnectionChecker::can_connect(conn_data))
		{
			TcpSocket* socket = new TcpSocket;
			socket->setBlocking(false);
			socket->connect(client, PORT);

			//send the key word to say
			//don't resend client info
			Packet pack;
			pack << "motherfucker";
			socket->send(pack);

			make_client(clients, client_counter, socket, PORT);
			offline_clients.erase(offline_clients.begin() + i);
			//also erase them from DB
			erase_offline_client(room_name, client);
		}
	}
}
void DecentralysedServerClient::connnect_finally()
{
	//connect again if you are not "holder"
	if (connecting && !connect_to_saved_clients)
	{
		auto address = IpAddress(conn_ip.c_str());
		socket = new TcpSocket();

		if (socket->connect(address, PORT) == TcpSocket::Done)
		{	
			password = get_raw_message(*socket);

			///shit comment:send your listner port
			Packet pack;
			pack << "motherfucker";
			socket->send(pack);

			if (!does_room_exists(room_name))
			{
				create_new_room(room_name, password);
				create_room_connections_info(room_name);
			}

			allowed.push_back(address);
			//save connected as client
			make_client(clients, client_counter, socket, PORT);
		}
	}
}
void DecentralysedServerClient::load_allowed_long_known_clients()
{
	auto info = get_connections_info(room_name);
	for (auto& elem : info)
	{
		auto ip_str = get<1>(elem);
		IpAddress ip(ip_str);
		if(ip.toInteger() != 0)allowed.push_back(ip);
	}
}