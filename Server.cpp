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

	listener.listen(port);
	sf::TcpSocket socket;
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