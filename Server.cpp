#include "Server.h"
using namespace Blink;

Server::Server(command_hash& commands,
			   const string& password,
			  const NetBaseData& data,
			   bool inherited):NetBase(commands,data)
{
	this->password  = password;
	this->room_name = data.room_name;

	this->commands["makelink"] = function<void()>([&]()
		{
			//if inherited, than decentralysed
			create_invite_link(PORT, data.room_name, password, inherited);
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
	string nickname;
	cout << "before we start...";
	cout << "say recepient's nickname:";
	cin >> nickname;
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
			inv_link = get_invite_link_str(port, room_name, room_password,decentralysed,nickname);
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
			cout << "enter recepient's e-mail:";
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
void Server::create_invite_link_to_send(int port,
								const string& room_name,
								const string& room_password,
								bool decentralysed,
								const string& additional_data,
								const string& recepient_name)
{
	string inv_link = get_invite_link_str(port, 
						room_name, 
						room_password, 
						decentralysed,
						recepient_name);
	auto write_link_to_file = [&](const string& path)
	{
		ofstream file;
		file.open(path, ios::binary);
		file << inv_link;
		file.close();
	};

	write_link_to_file("link");
	string command = "sender.exe " + additional_data;
	system(command.c_str());
	system("@echo off\n erase link");
}
void Server::create_invite_link_to_save(int port,
								const string& room_name,
								const string& room_password,
								bool decentralysed,
								const string& additional_data,
								const string& recepient_name)
{
	string inv_link = get_invite_link_str(port, 
								room_name, 
								room_password, 
								decentralysed,
								recepient_name);

	auto write_link_to_file = [&](const string& path)
	{
		ofstream file;
		file.open(path, ios::binary);
		file << inv_link;
		file.close();
	};
	write_link_to_file(additional_data);
}


string Server::get_invite_link_str(int port,
								   const string& room_name,
								   const string& room_password,
								   bool decentralysed,
								   const string& recepient_name)
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
	
	inv_link = ::encrypt_invite_link(inv_link, recepient_name);
	return inv_link;
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
	string _password = password.size()!= 64? encr::SHA::sha256(password):password;
	string _room_name= room_name;
	if (ConnectionChecker::can_come_in(check, _password,_room_name))
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
	auto should_be_erased = [&](RoomClient* client)
	{
		string message = get_message(*client->socket);
		if (socket_dissconnected)
		{
			cout << "\n#client with id " << client->id << " has been disconnected." << endl;
			update_input();
			update = true;
			socket_dissconnected = false;
			return true;
		}
		else
		{
			if(message.size() > 0) show_message(message);
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