#include "EnterMenu.h"
using namespace Blink;

EnterMenu::EnterMenu(const string& encr_key,
					 const string& db_name):DataBaseProcessor(encr_key,db_name)
{
	commands["new"]   = function<void()>([&]() {create_user(); });
	commands["login"] = function<void()>([&]() {login(); });
}

void EnterMenu::create_user()
{
	string name, password;
	cout << "enter name:"; cin >> name;
	cout << "enter password:"; 
	password = get_secret_data();

	if (is_name_passw_correct(name,password))
	{
		if (!does_user_exist(name)) create_new_user(name, password);
		else cout << "can not create user!" << endl;
	}
	else
	{
		cout << "name/password is too short/long!"<<endl;
	}
}
void EnterMenu::login()
{
	string name, password;
	cout << "enter name:"; cin >> name;
	cout << "enter password:"; 
	password = get_secret_data();

	if (can_login(name, password))
	{
		system("cls");
		cout << "welcome " << name << " to Blink!" << endl;
		change_menu = true;
		user_name = name;
		this->password = password;
	}
	else cout << "can not login!" << endl;
}