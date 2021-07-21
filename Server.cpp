#include "Server.h"
using namespace Blink;

Server::Server(const command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name):NetBase(commands,user_name)
{
	this->password  = password;
	this->room_name = room_name;

	key_iv = encr::get_random_key();
}
Server::~Server()
{
}
void Server::show_key_iv()
{
	string key = encr::convert_bytes(key_iv.first);
	string iv  = encr::convert_bytes(key_iv.second);
	cout << "Next key and IV will be used to encrypt data with AES:";
	cout << endl;
	cout << key << endl << iv << endl;
	cout << ".";
}
bool Server::run(const string& channel_name,
				 int port)
{

	show_key_iv();
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