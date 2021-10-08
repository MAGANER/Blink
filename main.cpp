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
#include"BaseGraphicalMenu.h"
#include"GraphicalEnterMenu.hpp"
#include"GraphicalMainMenu.h"
#include"ConfigLoader.h"

static bool currently_fullscreen = false;
static sf::Vector2u win_size(720,640);//default value

void run_graphical_mode(Blink::ConfigLoader& loader);
void run_console_mode();
void prepare_menu(GraphicalBlink::BaseGraphicalMenu* menu, Blink::ConfigLoader& loader);

GraphicalBlink::BaseGraphicalMenu::CurrentMenu
run_menu(GraphicalBlink::BaseGraphicalMenu::CurrentMenu val,
	     string& key,
	     string& db_name,
		 Blink::ConfigLoader& loader,
		 bool fullscreen = false);
void process_fullscreen(GraphicalBlink::BaseGraphicalMenu* menu,
						GraphicalBlink::BaseGraphicalMenu::CurrentMenu val,
						Blink::ConfigLoader& loader);
GraphicalBlink::BaseGraphicalMenu::CurrentMenu 
process_menu_running(GraphicalBlink::BaseGraphicalMenu* menu, 
					 bool fullscreen,
					 Blink::ConfigLoader& loader);
int main(int argc, char** argv)
{
	using namespace Blink;
	ConfigLoader config_loader;
	win_size = config_loader.get_win_size();


	bool console_mode1 = argc == 2 && string(argv[1]) == "-t";
	bool console_mode2 = config_loader.get_mode();
	if (console_mode1 || console_mode2)
		run_console_mode();
	else
		run_graphical_mode(config_loader);

	return 0;
}
void prepare_menu(GraphicalBlink::BaseGraphicalMenu* menu, Blink::ConfigLoader& loader)
{
	if (loader.is_theme_loaded())
	{
		auto color = loader.get_background_win_color();
		menu->set_background_color(color);
	}
}

GraphicalBlink::BaseGraphicalMenu::CurrentMenu 
run_menu(GraphicalBlink::BaseGraphicalMenu::CurrentMenu val,
		 string& key,
		 string& db_name,
		 Blink::ConfigLoader& loader,
		 bool fullscreen)
{
	using namespace GraphicalBlink;
	if (val == BaseGraphicalMenu::CurrentMenu::EnterMenu)
	{
		GraphicalEnterMenu* enter_menu = new GraphicalEnterMenu(fullscreen,key, db_name,win_size);
		enter_menu->create(loader);
		prepare_menu(enter_menu, loader);

		auto change = process_menu_running(enter_menu, fullscreen,loader);

		key = enter_menu->get_user_password();
		db_name = enter_menu->get_db_name();
		if(enter_menu != nullptr)delete enter_menu;
		return change;
	}
	if (val == BaseGraphicalMenu::CurrentMenu::MainMenu)
	{
		GraphicalMainMenu* main_menu = new GraphicalMainMenu(fullscreen,key, db_name, win_size);
		main_menu->create(loader);
		if (currently_fullscreen)
		{
			main_menu->set_fullscreen(true, loader);
		}
		prepare_menu(main_menu, loader);


		auto change = process_menu_running(main_menu, fullscreen, loader);

		if(main_menu != nullptr)delete main_menu;
		return change;
	}
}
void process_fullscreen(GraphicalBlink::BaseGraphicalMenu* menu,
						GraphicalBlink::BaseGraphicalMenu::CurrentMenu val,
						Blink::ConfigLoader& loader)
{
	using namespace GraphicalBlink;
	if (val == BaseGraphicalMenu::CurrentMenu::MakeFullscreen)
	{
		currently_fullscreen = true;
		menu->set_fullscreen(true,loader);
	}
	else if (val == BaseGraphicalMenu::CurrentMenu::BackToWindow)
	{
		currently_fullscreen = false;
		menu->set_fullscreen(false,loader);
	}
}
GraphicalBlink::BaseGraphicalMenu::CurrentMenu 
process_menu_running(GraphicalBlink::BaseGraphicalMenu* menu,
					 bool fullscreen,
					  Blink::ConfigLoader& loader)
{ 
	auto change = GraphicalBlink::BaseGraphicalMenu::CurrentMenu::Nothing;
	while (true)
	{
		change = menu->run(fullscreen,win_size);
		process_fullscreen(menu, change,loader);
		if (menu->is_end()) return change;
	
	}
}
void run_graphical_mode(Blink::ConfigLoader& loader)
{
	using namespace GraphicalBlink;

	string key, db_name;
	while (true)
	{
		auto start = BaseGraphicalMenu::CurrentMenu::EnterMenu;;
		run_menu(run_menu(start,key,db_name,loader, loader.is_fullscreen()),key,db_name,loader, loader.is_fullscreen());
	}
}
void run_console_mode()
{
	using namespace Blink;
	system("cls");
	string key, db_name;

	EnterMenu* enter_menu = new EnterMenu(key, db_name);
	MainMenu* main_menu = nullptr;
	RoomMenu* room_menu = nullptr;

	enum class state { ENTER, MAIN, ROOM };

	state current = state::ENTER;


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
				main_menu = new MainMenu(key, db_name);
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

				room_menu->set_room_data(data.port,
					data.ip,
					current_user_name,
					data.room,
					data.password);

				if (main_menu->start_room())starting_room = true;

				delete main_menu;

				current = state::ROOM;
			}
			else if (main_menu->_connect())
			{
				room_menu = new RoomMenu(key, db_name);

				ConnectionData data = main_menu->get_connection_data();

				room_menu->set_room_data(data.port,
					data.ip,
					current_user_name,
					data.room,
					data.password);


				//init 
				keys = new encr::AES::key_iv();
				keys->first = main_menu->get_encryption_data()->data.first;
				keys->second = main_menu->get_encryption_data()->data.second;
				connecting_with_conflink_command = main_menu->is_connecting_with_conflink_command();

				delete main_menu;

				current = state::ROOM;
			}
		}
		if (current == state::ROOM)
		{
			if (keys != nullptr)
				room_menu->run(*keys, connecting_with_conflink_command, starting_room);
			else
				room_menu->run(connecting_with_conflink_command, starting_room);

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
	if (main_menu != nullptr) delete main_menu;
	if (room_menu != nullptr) delete room_menu;
	if (keys != nullptr) delete keys;
}