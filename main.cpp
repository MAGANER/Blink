#include"RoomMenu.h"
#include"EnterMenu.h"
#include"MainMenu.h"
using namespace Blink;

int main()
{
	string key = create_database(); //if it doesn't exist

	EnterMenu* enter_menu= new EnterMenu(key);
	MainMenu*  main_menu = nullptr;
	RoomMenu* room_menu = nullptr;

	enum class state{ ENTER,MAIN,ROOM };

	state current = state::ENTER;
	RoomMenu::mode app_mode;
	
	string* current_user_name = nullptr;
	while (true)
	{
		if (current == state::ENTER)
		{
			enter_menu->run();
			if (enter_menu->change())
			{
				current = state::MAIN;
				current_user_name = new string(enter_menu->get_user_name());
				delete enter_menu;
				main_menu = new MainMenu(key);
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
				enter_menu = new EnterMenu(key);
			}
			else if (main_menu->enter_room())
			{
				room_menu = new RoomMenu(key);
				room_menu->set_room_data(main_menu->get_port(),
										 main_menu->get_room_ip(),
										 *current_user_name,
										 main_menu->get_room_to_enter(),
										 main_menu->get_password_to_enter());

				delete current_user_name;
				delete main_menu;

				app_mode = RoomMenu::mode::SERVER;
				current = state::ROOM;	
			}
			else if (main_menu->_connect())
			{
				room_menu = new RoomMenu(key);
				room_menu->set_room_data(main_menu->get_port(),
										 main_menu->get_room_ip(),
										 *current_user_name,
										 main_menu->get_room_to_enter(),
										 main_menu->get_password_to_enter());

				delete current_user_name;
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
	if (room_menu  != nullptr) delete room_menu;

	return 0;
}