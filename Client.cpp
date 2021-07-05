#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, int port, const string& channel_name)
{
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
	auto send = [&]() {manager.socketSend(channel_name, message); };
	run_socket(send);
}