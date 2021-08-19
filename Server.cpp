#include "Server.h"
using namespace Blink;

Server::Server(const command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name,
			   const string& db_key,
			   const string& db_name):NetBase(commands,user_name, room_name, db_key,db_name)
{
	this->password  = password;
	this->room_name = room_name;

	key_iv = encr::AES::get_random_key();
}
Server::~Server()
{
}

void Server::create_invite_link(int port,
								const string& room_name,
								const string& room_password)
{
	cout << "before server starts...";
	string inv_link;
	while (true)
	{
		int mode = -1;
		cout << "save invite link or send it to special e-mail?(1,2):";
		cin >> mode;
		
		auto write_link_to_file = [&](const string& path)
		{
			ofstream file;
			file.open(path, ios::binary);
			file << inv_link;
			file.close();
		};
		if (mode != -1)
		{
			string key = encr::AES::convert_bytes(key_iv.first);
			string iv = encr::AES::convert_bytes(key_iv.second);
			inv_link = ::create_invite_link(get_ip(),
											to_string(port),
											room_name,
											iv,
											key,
											room_password);
			inv_link = ::encrypt_invite_link(inv_link);
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
bool Server::run(const string& room_name,
				 const string& room_password,
				 int port,
				 RoomNetworkMode mode)
{

	//show_key_iv();
	create_invite_link(port,room_name,room_password);

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
	if (listener.accept(socket) == TcpSocket::Done)
	{
		cout << "someone tries to connect.." << endl;
	}
	//first we should accept ability to connect
	check_access(socket);

	listener.listen(port);
	listener.accept(socket);
	listener.close();

	socket.setBlocking(false);
	while (true)
	{
		get_and_show_message(socket);
		receive_input_and_send_message(socket);
	}
	return;
}
void Server::run_one2ones_mode(const string& room_name,
							   const string& room_password,
							   int port)
{

	listener.setBlocking(false);
	listener.listen(port);

	list<TcpSocket*> clients;
	vector<IpAddress> allowed;

	TcpSocket* entering_socket = new TcpSocket;
	while (true)
	{
		receive_input_and_send_message_to_all(clients);

		// The listener is ready: there is a pending connection
		if (listener.accept(*entering_socket) == sf::Socket::Done)
		{
			bool check1 = is_addres_allowed(allowed, entering_socket->getRemoteAddress());
			if (check1 && true)
			{
				TcpSocket* client = move(entering_socket);
				client->setBlocking(false);

				clients.push_back(client);
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

		for (auto& client : clients)
			NetBase::return_and_show_message(*client, clients);
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
	if (can_come_in(check, encr::SHA::sha256(password), encr::SHA::sha256(room_name)))
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
void Server::check_access(TcpSocket& socket)
{
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
}