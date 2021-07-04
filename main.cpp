#include"RoomMenu.h"
#include"EnterMenu.h"
#include"MainMenu.h"
using namespace Blink;

int main()
{
	system("cls");
	create_database(); //if it doesn't exist

	EnterMenu* enter_menu= new EnterMenu();
	MainMenu*  main_menu = nullptr;
	RoomMenu* room_menu = nullptr;

	AsyncInput input;
	enum class state{ ENTER,MAIN,ROOM };

	state current = state::ENTER;
	RoomMenu::mode app_mode;


	string ip, port;
	while (true)
	{
		if (current == state::ENTER)
		{
			enter_menu->run();
			if (enter_menu->change())
			{
				current = state::MAIN;
				delete enter_menu;
				main_menu = new MainMenu();
			}
		}
		if (current == state::MAIN)
		{
			main_menu->run();
			if (main_menu->exit())
			{
				cout << "farewell!" << endl;
				current = state::ENTER;
				delete main_menu;
				enter_menu = new EnterMenu();
			}
			else if (main_menu->enter_room())
			{
				room_menu = new RoomMenu();
				room_menu->set_room_data(main_menu->get_port(),
										 main_menu->get_room_ip());

				delete main_menu;

				app_mode = RoomMenu::mode::SERVER;
				current = state::ROOM;	
			}
			else if (main_menu->_connect())
			{
				room_menu = new RoomMenu();
				room_menu->set_room_data(main_menu->get_port(),
										 main_menu->get_room_ip());

				delete main_menu;

				app_mode = RoomMenu::mode::CLIENT;
				current = state::ROOM;		
			}
		}
		if (current == state::ROOM)
		{
			room_menu->run(app_mode);
		}
	}



	if (enter_menu != nullptr) delete enter_menu;
	if (main_menu  != nullptr) delete main_menu;

	return 0;
}




/*
Server* server = nullptr;
Client* client = nullptr;
cout << "mode:";
int mode;
cin >> mode;
if (mode == 1)
{
	int port;
	cout << "enter server port:";
	cin >> port;
	server = new Server(port);
	while (true)
	{
		string result = server->get();
		cout << "#" << result << endl;
	}
	//room_menu->set_room_data(to_string(port), "");
	//while (true)room_menu->run(RoomMenu::mode::SERVER);
}
if (mode == 2)
{
	string ip;
	int port;
	cout << "enter port:";
	cin >> port;

	cout << "enter ip:";
	cin >> ip;
	client = new Client(ip, port);
	while (true)
	{
		string message;
		cout << "%";
		cin >> message;
		client->send_message(message);
	}
	//room_menu->set_room_data(to_string(port), ip);
	//while (true)room_menu->run(RoomMenu::mode::CLIENT);
}
*/