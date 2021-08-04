#include "Server.h"
using namespace Blink;

Server::Server(const command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name):NetBase(commands,user_name)
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
			cout << inv_link.size() << endl;
		}
		if (mode == 1)
		{
			ofstream file;
			string path;
			cout << "enter path to save link:";
			cin >> path;
			file.open(path,ios::binary);
			file << inv_link;
			file.close();
			cout << "link saved!";
			break;
		}
		else if (mode == 2)
		{

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
	if (can_come_in(check, password, room_name))
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
	}
	return true;
}
void Server::process_commands(const string& command)
{
}