#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu()
{
	commands["exit"]   = function<void()>([&]() { _exit = true; });
	commands["create"] = function<void()>([&]() { create();     });
	commands["enter"]  = function<void()>([&]() { enter();      });
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
		can_enter_room = true;
		system("cls");
		cout << "welcome to " << name << " room!" << endl;
	}
}