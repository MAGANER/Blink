#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu(const string& encr_key):DataBaseProcessor(encr_key)
{
	commands["exit"]    = function<void()>([&]() { _exit = true; });
	commands["create"]  = function<void()>([&]() { create();     });
	commands["enter"]   = function<void()>([&]() { enter();      });
	commands["connect"] = function<void()>([&]() { connect();   });
	commands["conflink"] = function<void()>([&]() { connect_with_filelink();   });
	//commands["conflink"] = function<void()>([&]() { connect_with_link();   });
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
		data.room     = name;
		data.password = password;
		data.port = to_string(get_room_port(name, get_encr_key()));
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
		data.ip        = ip;
		data.password  = password;
		data.port      = to_string(port);
		data.room      = room_name;
		can_connect_to_room = true;
		con_regime = CONNECTION_REGIME::Manually;
	}
}
void MainMenu::connect_with_filelink()
{
	string path;
	ifstream link_file;
	while (true)
	{
		cout << "enter path to link:";
		cin >> path;
		link_file.open(path, ios::binary);
		if (!link_file)
		{
			cout << "can not open " << path << "!" << endl;
			path.clear();
		}
		else break;
	}

	
	string link;
	istreambuf_iterator<char> inputIt(link_file), emptyInputIt;
	back_insert_iterator<string> stringInsert(link);

	copy(inputIt, emptyInputIt, stringInsert);

	link = decrypt_invite_link(link);

	json jlink = json::parse(link);
	data.ip   = jlink["ip"];
	data.port = jlink["port"];
	data.room = jlink["room"];
	data.password = jlink["password"];
	
	encr_data = new EncryptionData(jlink["key"],jlink["iv"]);
}
