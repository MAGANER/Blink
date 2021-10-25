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
	
	//user data for widgets
	const int no_rooms_id  = 3;
	const int default_id   = 0;
	const int room_list_id = 1;

	Blink::ConfigLoader* loader = nullptr;

	ListBox::Ptr rooms_ptr;
	Label::Ptr no_rooms_label_ptr;
	EditBox::Ptr room_passw;

	RoomGateMenu* room_gate_menu = nullptr;
	GraphicalConnectingSubMenu* conn_menu = nullptr;
	GraphicalChatMenu* chat_menu = nullptr;

	string user_name;

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
	void run_create_room_menu(Blink::ConfigLoader& loader);
	void create_link();
	void connect_link(Blink::ConfigLoader& loader);
	void recreate_this_menu(Blink::ConfigLoader& loader);

	void process_chat();
	void _main_echo_function();
};
};
#endif