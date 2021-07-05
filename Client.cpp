#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, 
			   int port, 
			   const string& channel_name,
			   const string& user_name)
{
	local_ip        = get_ip();
	this->user_name = user_name;

	this->ip   = ip;
	this->port = port;
	this->channel_name = channel_name;

	auto connect = [&]() {manager.socketConnect(channel_name, ip, port);};
	run_socket(connect);
}
Client::~Client()
{
	auto close = [&]() {manager.closeConnection(channel_name); };
	run_socket(close);
}

void Client::send_message(const string& message)
{
	auto send = [&]() { manager.socketSend(channel_name, 
					    convert_message_to_json(message)); 
					  };
	run_socket(send);
}
string Client::convert_message_to_json(const string& text)
{
	json message;
	message["text"] = text;
	message["ip"]   = local_ip;
	message["name"] = user_name;
	return message.dump();
}