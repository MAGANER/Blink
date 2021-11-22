/*
	From this menu user has ability to enter/start/connect/create room.
	So it creates every sub menu
*/

#ifndef GRAPHICAL_MAIN_MENU_H
#define GRAPHICAL_MAIN_MENU_H
#ifndef COMMON_HEADERS_H
#define COMMON_HEADERS_H
#include"DataBaseProcessor.h"
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#endif
#include"CreateRoomMenu.h"
#include"TGUI/Widgets/ListBox.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/EditBox.hpp"
#include"GraphicalRoomGateMenuCreator.hpp"
#include"GraphicalDecentralysedServerClient.h"
#include"GraphicalConnectingSubMenu.hpp"
#include"GraphicalChatMenu.h"
namespace GraphicalBlink
{
using namespace Blink;
class GraphicalMainMenu :public DataBaseProcessor,
						 public BaseGraphicalMenu
{
private:
	vector<str3> rooms;


	bool no_rooms = true;
	bool init_chat = false;
	bool start_room = false;
	bool should_exit = false;
	
	//user data for widgets
	const int no_rooms_id  = 3;
	const int default_id   = 0;
	const int room_list_id = 1;

	ListBox::Ptr rooms_ptr;
	Label::Ptr no_rooms_label_ptr;
	EditBox::Ptr room_passw;


	//local ptr, it will be needed to recreate menu again
	Blink::ConfigLoader* loader = nullptr;

	//* every sub menu
	RoomGateMenu* room_gate_menu				= nullptr;
	GraphicalConnectingSubMenu* conn_menu		= nullptr;
	GraphicalChatMenu* chat_menu				= nullptr;
	CreateRoomMenu* create_room_menu			= nullptr;
	//**

	//This enum is needed to has ability to return from menu
	//if it is not required with pressing ESC key
	enum class ActiveSubMenu { creating, entering, connecting, none };
	ActiveSubMenu curr_sub_menu = ActiveSubMenu::none;

	string user_name;
	string encr_key;

	GraphicalDecentralysedServerClient* client = nullptr;
public:
	GraphicalMainMenu(bool fullscreen,
					  const string& encr_key,
					  const string& db_name,
					  const sf::Vector2u& win_size,
					  const string& user_name);
	~GraphicalMainMenu();

	void create(Blink::ConfigLoader& loader);
private:
	void set_room_box_pos_and_size(ListBox::Ptr ptr);
	void resize_room_list_box(sf::Event::EventType type);
	void set_no_rooms_label_to_center(sf::Event::EventType type);
	void resize_chat_widgets(sf::Event::EventType type);
	void run_create_room_menu(Blink::ConfigLoader& loader);
	void create_link();
	void connect_link(Blink::ConfigLoader& loader);
	void leave_creating_room_menu();
	void enter_room();

	void process_chat();
	void _main_echo_function();

	void exit() { menu_to_return = CurrentMenu::EnterMenu; };
	vector<MessageToShow> get_saved_messages();

	void clear_sub_menu();
};
};
#endif