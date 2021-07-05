#include "Server.h"
using namespace Blink;

Server::Server()
{
}
Server::~Server()
{
}

bool Server::run(const string& channel_name,
				 int port)
{
	auto callback = [&](const string& data)
	{
		cout << "#" << data << endl;
	};
	auto listen_to_socket = [&]() {manager.socketListen(channel_name, port, callback); };
	run_socket(listen_to_socket);
	return false;
}