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
		json message = json::parse(data);
		if (connected_ip.empty()) connected_ip = message["ip"];
		cout << message["name"] << ":" << message["text"]<<endl;
	};
	
	auto listen_to_socket = [&]() {manager.socketListen(channel_name, port, callback); };
	auto run_server = [&]()
	{
		run_socket(listen_to_socket);
		return true;
	};
	auto process = [&](const string& command) {process_commands(command); };
	auto result = async(std::launch::async, run_server);
	
	//process input 
	while (true)
	{
	}
	result.get(); //don't need it
	return false;
}
void Server::process_commands(const string& command)
{
}