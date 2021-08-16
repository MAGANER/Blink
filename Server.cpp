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
			cout << "link saved!";
			break;
		}
		else if (mode == 2)
		{
			string recepient;
			cout << "enter recepient:";
			cin >> recepient;
			write_link_to_file("link");
			string command = "@echo off\n sender.exe " + recepient;
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
				 int port)
{

	//show_key_iv();
	create_invite_link(port,room_name,room_password);
	cout << "waiting for connection..." << endl;
	listener.listen(port);
	sf::TcpSocket socket;
	listener.accept(socket);
	//first we should accept ability to connect

	string check = get_raw_message(socket);
	Packet p;
	if (can_come_in(check, encr::SHA::sha256(password), encr::SHA::sha256(room_name)))
	{
		p << "1";
		socket.send(p);
	}
	{
		p << "0";
		socket.send(p);
	}
	socket.disconnect();

	listener.listen(port);
	listener.accept(socket);

	socket.setBlocking(false);
	while (true)
	{
		get_and_show_message(socket);
		receive_input_and_send_message(socket);
		if (should_disconnect())return false;
	}
	return true;
}
void Server::process_commands(const string& command)
{
}