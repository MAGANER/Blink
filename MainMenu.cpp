#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu()
{
	commands["exit"]   = function<void()>([&]() { _exit = true; });
	commands["create"] = function<void()>([&]() { create();     });
}
void MainMenu::create()
{
	string name, password, port;
	cout << "enter room name:";      cin >> name;
	cout << "enter room passworod:"; cin >> password;
	cout << "enter room port:";      cin >> port;

	if (!does_room_exists(name, password))create_new_room(name, password, port);
	else cout << "room" << " `" + name + "`" << " already exists!" << endl;
}