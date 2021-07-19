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