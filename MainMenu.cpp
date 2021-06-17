#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu()
{
	commands["exit"] = function<void()>([&]() {_exit = true; });
}
void MainMenu::create()
{
	string name, password, port;
	cout << "enter room name:";      cin >> name;
	cout << "enter room passworod:"; cin >> password;
	cout << "enter room port:";      cin >> port;

	create_new_room(name, password, port);
}