#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu(const string& encr_key,
				   const string& db_name,
				   const string& user_name):DataBaseProcessor(encr_key,db_name)
{
	commands["exit"]    = function<void()>([&]() { _exit = true; });
	commands["create"]  = function<void()>([&]() { create();     });
	commands["enter"]   = function<void()>([&]() { enter();      });
	commands["start"]   = function<void()>([&]() { _start_room();      });
	commands["conflink"] = function<void()>([&]()
		{
			EncryptionData _data;

			string path;
			cout << "enter path to link:";
			cin >> path;
			if (ConnectionChecker::connect_with_filelink(data, _data,path,user_name))
			{
				can_connect_to_room = true;
				encr_data = new EncryptionData();
				encr_data->data = _data.data;
				connecting_with_conflink_command = true;
			}
		});
	commands["rooms"] = function<void()>([&]() {show_rooms(); });
}
void MainMenu::create()
{
	auto input = get_login_password();

	string name = input.first;
	string password = input.second;
	string port = to_string(NetRandom::get_random_port());

	if (is_name_passw_correct(name, password))
	{
		if (!does_room_exists(name))create_new_room(name, password, port);
		else cout << "room" << " `" + name + "`" << " already exists!" << endl;
	}
	else
	{
		cout << "name/password is too short/long!" << endl;
	}

}
void MainMenu::_start_room()
{
	auto input = get_login_password();
	string name = input.first;
	string password = input.second;
	if (!is_password_correct(name, password)) cout << "can not enter to " << "`" + name + "`" << " room";
	else
	{
		init_room_data(name, password);
		can_start_room = true;
		system("cls");
		cout << "welcome to " << name << " room!" << endl;
	}
}
void MainMenu::enter()
{
	auto input = get_login_password();
	string name = input.first;
	string password = input.second;

	if (!is_password_correct(name,password)) cout << "can not enter to " << "`" + name + "`" << " room";
	else
	{
		init_room_data(name, password);
		can_enter_room = true;
		system("cls");
		cout << "welcome to " << name << " room!" << endl;
	}
}
void MainMenu::init_room_data(const string& name, const string& password)
{
	data.room = name;
	data.password = password;
	data.port = to_string(get_room_port(name, get_encr_key(), get_db_name()));
}
void MainMenu::show_rooms()
{
	//enter password
	//if it's right
	//show list of rooms

	cout << "password:";
	string password = get_secret_data();

	if (password == get_encr_key())
	{
		auto rooms = get_rooms(password);
		for (auto& r : rooms)
		{
			cout << "name:"     << get<0>(r)<<endl;
			cout << "password:" << get<1>(r)<<endl;
			cout << "port:"     << get<2>(r)<<endl;
			cout << "----"<<endl;
		}
	}
	else
	{
		cout << "inccorect password!" << endl;
	}
}
pair<string, string> MainMenu::get_login_password()
{
	string name, password;
	cout << "enter room name:";      cin >> name;
	cout << "enter room password:";
	password = get_secret_data(); //don't show entered symbols, while getting input
	return make_pair(name, password);
}