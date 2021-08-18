#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, 
			   int port, 
			   const command_hash& commands,
			   const string& user_name,
			   const string& room_name,
			   const string& db_key,
			   const string& db_name):NetBase(commands,user_name,room_name,db_key,db_name)
{
	this->port = port;
	name = user_name;
	room = room_name;

	sender.connect(ip, port);
	sender.setBlocking(false);

	
	string key, iv;
	cout << "enter AES encryption key:";
	cin >> key;

	cout << "enter AES encryption IV:";
	cin >> iv;
	key_iv = make_pair(encr::AES::convert_to_bytes(key), encr::AES::convert_to_bytes(iv));
}
Client::Client(const string& ip,
			   int port,
			   const command_hash& commands,
			   const string& user_name,
			   const string& room_name,
			   const encr::AES::key_iv& keys,
			   const string& db_key,
			   const string& db_name) :NetBase(commands, user_name, room_name, db_key,db_name)
{
	this->port = port;
	name = user_name;
	room = room_name;

	sender.connect(ip, port);
	sender.setBlocking(false);
	key_iv = keys;
}
Client::~Client()
{
}
void Client::run()
{
	receive_input_and_send_message(sender);
	get_and_show_message(sender);
	if (should_disconnect())return;
}