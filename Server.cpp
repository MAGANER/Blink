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


	while (true)
	{
		Packet pack;
		string data;
		socket.receive(pack);
		/*if (!is_first_message_received)
		{
			string connection_data;
			pack >> connection_data;
			json data = json::parse(connection_data);
			
			connected_ip = fp::slice(_s(data["ip"]), 1, _s((data["ip"])).size());
			connected_port = data["port"];

			is_first_message_received = true;
		}
		else
		{*/
		pack >> data;
		cout << "got:" << data << endl;
		//}

		data.clear();
		cout << "enter:";
		cin >> data;
		pack.clear();
		pack << data;
		socket.send(pack);
	}
	return true;
}
void Server::process_commands(const string& command)
{
}