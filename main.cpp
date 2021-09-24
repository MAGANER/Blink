/*
* BlinkConsole
* stage:pre-alpha

	$main.cpp is entry point to application. that's obvious
	
		How does it work?
	If application is executed first time, then before all
	there will be created data base encrypted with special key.

	When you start new Blink session you should enter this key.

	On top there are 3 menu: Room, Enter and Main
	EnterMenu provides an abilities to create new user/login.

	If you get access you get to MainMenu.

	MainMenu provides abilities to enter room(start your own server)
	or connect to the room manually or with special invite link.
	Invite link is created when you enter room.

	And there is third menu. RoomMenu. Users moves to this one after
	he/she/it enters room or connect to the one.
	
*/

#include"RoomMenu.h"
#include"EnterMenu.h"
#include"MainMenu.h"
using namespace Blink;

int main()
{
	system("cls");
	string key,db_name;

	EnterMenu* enter_menu= new EnterMenu(key,db_name);
	MainMenu*  main_menu = nullptr;
	RoomMenu* room_menu = nullptr;

	enum class state{ ENTER,MAIN,ROOM };

	state current = state::ENTER;
	RoomMenu::mode app_mode;
	
	string current_user_name;
	encr::AES::key_iv* keys = nullptr;

	
	//if u use conflink command, then next special
	//port will be generated. I mean port for your server
	//If you enter rooms, then old port is used
	//see: MainMenu.h 58
	bool connecting_with_conflink_command = false;

	bool starting_room = false;
	while (true)
	{
		if (current == state::ENTER)
		{
			enter_menu->run();
			if (enter_menu->change())
			{
				current = state::MAIN;
				current_user_name = enter_menu->get_user_name();
				key = enter_menu->get_correct_user_password();
				db_name = enter_menu->get_db_name();
				delete enter_menu;
				main_menu = new MainMenu(key,db_name);
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
				enter_menu = new EnterMenu(key, db_name);
			}
			else if (main_menu->enter_room() || 
					 main_menu->start_room())
			{
				room_menu = new RoomMenu(key, db_name);

				ConnectionData data = main_menu->get_connection_data();
				if (main_menu->is_decentralysed()) data.mode = RoomNetworkMode::Decentralysed;

				room_menu->set_room_data(data.port,
										 data.ip,
										 current_user_name,
										 data.room,
										 data.password,
										 data.mode);

				if (main_menu->start_room())starting_room = true;

				delete main_menu;

				app_mode = RoomMenu::mode::SERVER;
				current = state::ROOM;	
			}
			else if (main_menu->_connect())
			{
				room_menu = new RoomMenu(key, db_name);
				
				ConnectionData data = main_menu->get_connection_data();
				if (main_menu->is_decentralysed()) data.mode = RoomNetworkMode::Decentralysed;

				room_menu->set_room_data(data.port,
										 data.ip,
										 current_user_name,
										 data.room,
										 data.password,
										 data.mode);


				//init 
				keys = new encr::AES::key_iv();
				keys->first = main_menu->get_encryption_data()->data.first;
				keys->second = main_menu->get_encryption_data()->data.second;
				connecting_with_conflink_command = main_menu->is_connecting_with_conflink_command();

				delete main_menu;

				app_mode = RoomMenu::mode::CLIENT;
				current = state::ROOM;		
			}
		}
		if (current == state::ROOM)
		{
			if (keys != nullptr)
				room_menu->run(app_mode, *keys, connecting_with_conflink_command,starting_room);
			else
				room_menu->run(app_mode, connecting_with_conflink_command,starting_room);

			if (room_menu->should_exit())
			{
				current = state::MAIN;
				main_menu = new MainMenu(key, db_name);
				delete room_menu;
				connecting_with_conflink_command = false;
				starting_room = false;
			}
		}
	}



	if (enter_menu != nullptr) delete enter_menu;
	if (main_menu  != nullptr) delete main_menu;
	if (room_menu  != nullptr) delete room_menu;
	if (keys != nullptr) delete keys;

	return 0;
}