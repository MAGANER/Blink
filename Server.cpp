#include "Server.h"
using namespace Blink;

Server::Server(command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name,
			   const string& db_key,
			   const string& db_name,
			   bool inherited):NetBase(commands,user_name, room_name, db_key,db_name)
{
	this->password  = password;
	this->room_name = room_name;

	this->commands["makelink"] = function<void()>([&]()
		{
			//if inherited, than decentralysed
			create_invite_link(port, room_name, password, inherited);

		});

	if(!inherited)key_iv = encr::AES::get_random_key();
}
Server::~Server()
{
}

void Server::create_invite_link(int port,
								const string& room_name,
								const string& room_password,
								bool decentralysed)
{
	string inv_link;
	while (true)
	{
		string _mode;
		cout << "save invite link or send it to special e-mail?(1,2):";
		cin >> _mode;

		int mode = -1;
		if (_mode.size() != 1) mode = 3;
		mode = atoi(_mode.c_str());

		auto write_link_to_file = [&](const string& path)
		{
			ofstream file;
			file.open(path, ios::binary);
			file << inv_link;
			file.close();
		};
		if (mode != -1)
		{
			inv_link = get_invite_link_str(port, room_name, room_password,decentralysed);
		}
		if (mode == 1)
		{
			string path;
			cout << "enter path to save link:";
			cin >> path;
			write_link_to_file(path);
			cout << "link saved!"<<endl;
			break;
		}
		else if (mode == 2)
		{
			string recepient;
			cout << "enter recepient:";
			cin >> recepient;
			write_link_to_file("link");
			string command = "sender.exe " + recepient;
			system(command.c_str());
			system("@echo off\n erase link");
			break;
		}
		else
		{
			cout << "error occured!" << endl;
		}
	}	
}
string Server::get_invite_link_str(int port,
								   const string& room_name,
								   const string& room_password,
								   bool decentralysed)
{
	string key = encr::AES::convert_bytes(key_iv.first);
	string iv = encr::AES::convert_bytes(key_iv.second);
	string inv_link = ::create_invite_link(get_ip(),
		to_string(port),
		room_name,
		iv,
		key,
		room_password,
		decentralysed);
	inv_link = ::encrypt_invite_link(inv_link);
	return inv_link;
}
bool Server::run(const string& room_name,
				 const string& room_password,
				 int port,
				 RoomNetworkMode mode)
{
	this->port = port;
	create_invite_link(port,room_name,room_password,false);

	if (mode == RoomNetworkMode::OneToOne)
	{
		run_one2one_mode(room_name, room_password, port);
	}
	else if (mode == RoomNetworkMode::OneToOnes)
	{
		run_one2ones_mode(room_name, room_password, port);
	}
	return true;
}
void Server::run_one2one_mode(const string& room_name,
							  const string& room_password,
							  int port)
{
	listener.listen(port);

	sf::TcpSocket socket;
	listener.accept(socket);
	string check = get_raw_message(socket);
	Packet p;
	if (can_come_in(check, encr::SHA::sha256(password), encr::SHA::sha256(room_name)))
	{
		p << "1";
		if (socket.send(p) == TcpSocket::Done) socket.disconnect();
	}
	else
	{
		p << "0";
		if (socket.send(p) == TcpSocket::Done) socket.disconnect();
	}

	listener.accept(socket);
	listener.close();
	socket.setBlocking(false);
	while (true)
	{
		get_and_show_message(socket);
		receive_input_and_send_message(socket);
		if (should_disconnect())break;
	}
		
	return;
}
void Server::run_one2ones_mode(const string& room_name,
							   const string& room_password,
							   int port)
{

	listener.setBlocking(false);
	listener.listen(port);

	list<RoomClient*> clients;
	int client_counter = 0;

	vector<IpAddress> allowed;

	TcpSocket* entering_socket = new TcpSocket;
	while (true)
	{
		receive_input_and_send_message_to_all(clients);

		if (listener.accept(*entering_socket) == sf::Socket::Done)
		{
			bool check1 = is_addres_allowed(allowed, entering_socket->getRemoteAddress());
			if (check1 && true && can_accept_new_connection(client_counter))
			{
				RoomClient* client = new RoomClient;
				client->socket = move(entering_socket);
				client->socket->setBlocking(false);

				client->id = client_counter;
				clients.push_back(client);
				client_counter++;

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
			else check_access(*entering_socket, allowed);
		}

		//receive message, show it and resend
		for (auto& client : clients)
			NetBase::return_and_show_message(client, clients);

		update_clients(clients);
		if (should_disconnect())return;
	}

	if (entering_socket != nullptr) delete entering_socket;
}
bool Server::is_addres_allowed(vector<IpAddress>& allowed, const IpAddress& ip)
{
	for (auto& ip : allowed)
	{
		if (ip.toString() == ip.toString()) return true;
	}
	return false;
}
void Server::check_access(TcpSocket& socket, vector<IpAddress>& allowed)
{
	string check = get_raw_message(socket);
	Packet p;
	string _password = password.size() !=64  ? encr::SHA::sha256(password) : password;
	string _room_name= room_name.size()!=64  ? encr::SHA::sha256(room_name):  room_name;
	if (can_come_in(check, _password, _room_name))
	{
		auto address = socket.getRemoteAddress();
		if (address != IpAddress::None)
			allowed.push_back(address);
			
		p << "1";
		if (socket.send(p) == TcpSocket::Done) socket.disconnect();
	}
	else
	{
		p << "0";
		if (socket.send(p) == TcpSocket::Done) socket.disconnect();
	}
}
void Server::update_clients(list<RoomClient*>& clients)
{
	//erase disconnected clients
	//then change ids of the rest
	
	bool update = false;
	Packet pack;
	auto should_be_erased = [&](RoomClient* client)
	{
		if (client->socket->receive(pack) == TcpSocket::Disconnected)
		{
			cout << "\n#client with id " << client->id << " has been disconnected." << endl;
			update_input();
			update = true;
			return true;
		}
		return false;
	};
	clients.remove_if(should_be_erased);

	int counter = 0;
	if (update)
	{
		for (auto& client : clients)
		{
			client->id = counter;
			counter++;
		}
		update = false;
	}
}