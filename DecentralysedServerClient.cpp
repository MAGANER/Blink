#include "DecentralysedServerClient.h"
using namespace Blink;

DecentralysedServerClient::DecentralysedServerClient(command_hash& commands,
	const string& password,
	const string& room_name,
	const string& user_name,
	const string& db_key,
	const string& db_name,
	bool inherited) :Server(commands,password,room_name,user_name,db_key,db_name,inherited)
{
	while (true)
	{
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
			port = get_room_port(room_name, db_key, db_name);
			create_invite_link(port, room_name, password,true);
			break;
		}
		else if (mode == "2")
		{
			cout << "enter port:";
			cin >> port;
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
	list<RoomClient*> clients;
	vector<IpAddress> allowed;

	int client_counter = 0;

	
	//connect again if you are not "holder"
	if (connecting)
	{
		socket = new TcpSocket();
		auto address = IpAddress(conn_ip.c_str());
		auto port = atoi(conn_port.c_str());
		socket->connect(address, port);

		//send your listner port
		Packet pack;
		pack << to_string(this->port);
		socket->send(pack);

		//save connected as client
		make_client(clients, client_counter, socket, port);
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
				//retrieve port, cos it's needed to create clients' info
				int listner_port = -1;
				string some_shit = get_raw_message(*entering_socket);
				if (some_shit == "motherfucker") should_resend_clients_info = false;
				else listner_port = atoi(some_shit.c_str());

				

				if(should_resend_clients_info)send_clients_info(clients, entering_socket);
				make_client(clients, client_counter, entering_socket,listner_port);
		
				//clear it, because there is no need to keep this data anymore
				for (size_t i = 0; i < allowed.size(); i++)
				{
					auto curr = allowed[i];
					if (curr.toString() == entering_socket->getRemoteAddress().toString())
					{
						allowed.erase(allowed.begin() + i);
					}
				}
				entering_socket = new TcpSocket();
			}
			else
			{
				check_access(*entering_socket, allowed);
			}
		}

		//receive message, show it and resend
		for (auto& client : clients)
			NetBase::return_and_show_message(client, clients);

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
}
void DecentralysedServerClient::send_clients_info(list<RoomClient*>& clients,
												  TcpSocket* socket)
{
	string message = create_clients_info_message(clients, room_name, password);
	socket->setBlocking(true);
	send_jmessage(*socket, message);
	socket->setBlocking(false);
}