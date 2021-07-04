#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, int port, const string& channel_name)
{
	this->ip   = ip;
	this->port = port;
	this->channel_name = channel_name;
	manager.socketConnect(channel_name, ip, port);
}
Client::~Client()
{
	manager.closeConnection(channel_name);
}

void Client::send_message(const string& message)
{
	manager.socketSend(channel_name, message);
}