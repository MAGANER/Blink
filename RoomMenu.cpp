#include "RoomMenu.h"
using namespace Blink;

RoomMenu::RoomMenu()
{
	commands["exit"] = function<void()>
		(
			[&]() { exit = true; }
		);
	commands["status"] = function<void()>
		(
			[&]() { print_state(); }
		);
	commands["send"] = function<void()>
		(
			[&]() 
			{ 
				read_sending_input = true;
				send(); 
			}
		);
}
void RoomMenu::start_as_master()
{
	listner = new TcpListener();
	listner->setBlocking(false);

	current_connection = new TcpSocket();
	current_connection->setBlocking(false);


	started = true;
	exit = false;
}
void RoomMenu::start_as_slave()
{
	socket = new TcpSocket();
	started = true;
	exit    = false;
}
void RoomMenu::process_as_master(unsigned int port)
{
	listner->listen(port);
	listner->accept(*current_connection);


	char buffer[1024];
	std::size_t received = 0;
	if (current_connection->receive(buffer, sizeof(buffer), received) == sf::TcpSocket::Done)
	{
		string message = buffer;
		json result = json::parse(message);
		cout << result["name"] << ":" << result["text"]<<endl;
	}
}
void RoomMenu::print_state()
{
	cout << "name:" << data["name"] << endl
		<< "password:" << data["password"] << endl
		<< "port:" << data["port"] << endl;
	cout << endl;

	auto ip = current_connection->getRemoteAddress();
	if (ip.toInteger() == 0) cout << "no connection to room..." << endl;
	else cout << "connected ip:" << ip.toString() << endl;
	
}
void RoomMenu::set_room_data(const string& name,
							 const string& password,
							 const string& port)
{
	data["name"]     = name;
	data["password"] = password;
	data["port"]     = port;
}
void RoomMenu::send_message(const string& message, TcpSocket* target)
{
	//create json file
	json message_json;
	message_json["name"] = "master";//master of room, sure
	message_json["text"] = message;

	//save it as string
	string json_file = message_json.dump();
	
	//send it
	current_connection->send(json_file.c_str(), json_file.size() + 1);
}
void RoomMenu::send()
{
	//just say add message to type message
	cout << "type message:";
}
void RoomMenu::run()
{
	//if it's type to get message, then it can't read commands
	//but still  it's not blocking the main thread
	if (!read_sending_input) Interface::run();
	else
	{
		sending_input.read();
		if (sending_input.can_get_result())
		{
			send_message(sending_input.get_result(), current_connection);
			read_sending_input = false;
		}
	}
}