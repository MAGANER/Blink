#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, 
			   int port, 
			   const command_hash& commands,
			   const string& user_name):NetBase(commands,user_name)
{
	this->port = port;

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
			   const encr::AES::key_iv& keys) :NetBase(commands, user_name)
{
	this->port = port;

	sender.connect(ip, port);
	sender.setBlocking(false);
	key_iv = keys;
}
Client::~Client()
{
}

void Client::send_message(const string& message)
{
	NetBase::send_message(sender, message);
}

void Client::run()
{
	receive_input_and_send_message(sender);
	get_and_show_message(sender);
}