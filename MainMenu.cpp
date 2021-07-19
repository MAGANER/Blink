#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu()
{
	commands["exit"]    = function<void()>([&]() { _exit = true; });
	commands["create"]  = function<void()>([&]() { create();     });
	commands["enter"]   = function<void()>([&]() { enter();      });
	commands["connect"] = function<void()>([&]() { connect();   });
}
void MainMenu::create()
{
	string name, password, port;
	cout << "enter room name:";      cin >> name;
	cout << "enter room password:"; cin >> password;
	cout << "enter room port:";      cin >> port;
	if (!sql::is_integer(port))
	{
		cout << "can not create room! incorrect port >>" << port << endl;
		return;
	}
	if (!does_room_exists(name))create_new_room(name, password, port);
	else cout << "room" << " `" + name + "`" << " already exists!" << endl;
}
void MainMenu::enter()
{
	string name, password;
	cout << "enter room name:";      cin >> name;
	cout << "enter room password:"; cin >> password;

	if (!is_password_correct(name,password)) cout << "can not enter to " << "`" + name + "`" << " room";
	else
	{
		room_to_enter = name;
		this->password = password;
		port = to_string(get_room_port(name));
		can_enter_room = true;
		system("cls");
		cout << "welcome to " << name << " room!" << endl;
	}
}
bool MainMenu::can_connect(const string& ip,
						   const string& password,
						   const string& name,
						   unsigned int port)
{
	//connect to server
	TcpSocket socket;
	if (socket.connect(IpAddress(ip), port) != TcpSocket::Done)
	{
		cout << "can not connect to " << ip << "!" << endl;
		return false;
	}
	
	//send data to get right to be connected
	string message = convert_message_to_json(password, name, MessageType::ComeInRequest);
	Packet p; p << message;
	if (socket.send(p) != TcpSocket::Done)
	{
		cout << "can not send!" << endl;
		return false;
	}
	p.clear();
	
	//get the answer
	socket.receive(p);
	string received_result;
	p >> received_result;
	bool ability = false;
	if (received_result == "1") ability =  true;
	
	if (!ability)cout << "can not come in!" << endl;
	return ability;
}
void MainMenu::connect()
{
	string ip, room_name, password;
	int port;
	cout << "enter ip:";
	cin >> ip;
	cout << "enter port:";
	cin >> port;
	cout << "enter room name:";
	cin >> room_name;
	cout << "enter password:";
	cin >> password;

	if (can_connect(ip, password, room_name, port))
	{
		cout << "connecting to " << room_name << endl;
		room_ip        = ip;
		this->password = password;
		this->port     = to_string(port);
		room_to_enter  = room_name;
		can_connect_to_room = true;
	}


}