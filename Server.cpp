#include "Server.h"
using namespace Blink;

Server::Server()
{
}
Server::~Server()
{
}

void Server::run(const string& channel_name,
				 int port)
{
	auto callback = [&](const string& data)
	{
		cout << "#" << data << endl;
	};
	manager.socketListen(channel_name, port, callback);
}