#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, 
			   int port, 
			   const command_hash& commands):NetBase(commands)
{
	this->user_name = user_name;
	this->port = port;

	sender.connect(ip, port);
	sender.setBlocking(false);
}
Client::~Client()
{
}

void Client::send_message(const string& message)
{
	NetBase::send_message(sender, message);
}
json Client::send_connection_data()
{
	json data;
	data["ip"]   = get_ip();
	data["port"] = 55002;
	send_message(data.dump());
	return data;
}
string Client::convert_message_to_json(const string& text)
{
	json message;
	message["text"] = text;
	message["name"] = user_name;
	return message.dump();
}

void Client::run()
{
	receive_input_and_send_message(sender);
	get_and_show_message(sender);
}