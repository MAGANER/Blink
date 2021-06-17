#include "EnterMenu.h"
using namespace Blink;

EnterMenu::EnterMenu()
{
	commands["new"]   = function<void()>([&]() {create_user(); });
	commands["login"] = function<void()>([&]() {login(); });
}

void EnterMenu::create_user()
{
	string name, password;
	cout << "enter name:"; cin >> name;
	cout << "enter password:"; cin >> password;

	if(!does_user_exist(name,password)) create_new_user(name, password);
}
void EnterMenu::login()
{
	string name, password;
	cout << "enter name:"; cin >> name;
	cout << "enter password:"; cin >> password;

	if (does_user_exist(name, password)) change_menu = true;
	else cout << "can not login!" << endl;
}