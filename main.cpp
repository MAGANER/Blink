/*
* BlinkConsole
* stage:pre-alpha

	$main.cpp is entry point to application. that's obvious
	
	(Simple and not complicated)
	Console Mode:
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

	Graphical Mode:
	Graphical Mode is splitted into 2 global menus. Enter one and Main one.
	Main one contains of different sub menus.
	Chat,Room Creator, Room Entering.
*/

#include"RoomMenu.h"
#include"EnterMenu.h"
#include"MainMenu.h"
#include"BaseGraphicalMenu.h"
#include"GraphicalEnterMenu.hpp"
#include"GraphicalMainMenu.h"
#include"ConfigLoader.h"


//These variables make easy to work
//don't touch em!
static bool currently_fullscreen = false;
static sf::Vector2u win_size(720,640);//default value
static string user_name; //should be passed to MainMenu

void run_graphical_mode(Blink::ConfigLoader& loader);
void run_console_mode();

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

	//you can run console mode with direct command
	//also you can set console/graphical flag at data/config.json
	//true is console, false is graphical
	bool console_mode1 = argc == 2 && string(argv[1]) == "-t";
	bool console_mode2 = config_loader.get_mode();
	if (console_mode1 || console_mode2)
		run_console_mode();
	else
		run_graphical_mode(config_loader);

	return 0;
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
		enter_menu->prepare_menu(loader);

		auto change = process_menu_running(enter_menu, fullscreen,loader);

		key = enter_menu->get_user_password();
		db_name = enter_menu->get_db_name();
		user_name = enter_menu->get_user_name();
		if(enter_menu != nullptr)delete enter_menu;
		return change;
	}
	if (val == BaseGraphicalMenu::CurrentMenu::MainMenu)
	{
		GraphicalMainMenu* main_menu = new GraphicalMainMenu(fullscreen,key, db_name, win_size,user_name);
		main_menu->create(loader);
		if (currently_fullscreen)
		{
			main_menu->set_fullscreen(true, loader);
		}
		main_menu->prepare_menu(loader);


		auto change = process_menu_running(main_menu, fullscreen, loader);

		if(main_menu != nullptr)delete main_menu;

		return change;
	}
}
void process_fullscreen(GraphicalBlink::BaseGraphicalMenu* menu,
						GraphicalBlink::BaseGraphicalMenu::CurrentMenu val,
						Blink::ConfigLoader& loader)
{
	//set fullscreen or set Windowed mode
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
	//endlessly run menu, if  it's changed than start it again
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

		auto start = BaseGraphicalMenu::CurrentMenu::EnterMenu;;
		auto result = run_menu(start, key, db_name, loader, loader.is_fullscreen());
		while (true)
		{
			result = run_menu(result, key, db_name, loader, loader.is_fullscreen());
		}
}

void run_console_mode()
{
	//prepare data
	//init all stuff
	//clear console
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
		//match current menu and run required
		if (current == state::ENTER)
		{
			enter_menu->run();
			if (enter_menu->change())
			{
				//after enter menu user gets to main menu
				//so we get data about user name and encryption key
				//also we compute the path to data base name
				//it's user_name.db
				current = state::MAIN;
				current_user_name = enter_menu->get_user_name();
				key = enter_menu->get_correct_user_password();
				db_name = enter_menu->get_db_name();
				delete enter_menu;
				main_menu = new MainMenu(key, db_name, current_user_name);
			}
		}
		if (current == state::MAIN)
		{
			main_menu->run();
			if (main_menu->exit())
			{
				//if user gets out, then delete current menu and create previous one
				cout << "farewell!" << endl;
				current = state::ENTER;
				delete main_menu;
				enter_menu = new EnterMenu(key, db_name);
			}
			else if (main_menu->enter_room() ||
					 main_menu->start_room())
			{
				/*
					User enters room if it's already created.
					If user starts, then this room is just created.

					So get all data required to connect.
				*/

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
				main_menu = new MainMenu(key, db_name,current_user_name);
				delete room_menu;
				connecting_with_conflink_command = false;
				starting_room = false;
			}
		}
	}



	//delete everything before exit
	if (enter_menu != nullptr) delete enter_menu;
	if (main_menu != nullptr) delete main_menu;
	if (room_menu != nullptr) delete room_menu;
	if (keys != nullptr) delete keys;
}