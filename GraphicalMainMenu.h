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
namespace GraphicalBlink
{
using namespace Blink;
class GraphicalMainMenu :public DataBaseProcessor,
						 public BaseGraphicalMenu
{
private:
	vector<str3> rooms;
	bool no_rooms = true;


	//user data for widgets
	const int no_rooms_id  = 3;
	const int default_id   = 0;
	const int room_list_id = 1;
	const int enter_room_id = 4;
	const int conn_room_id = 5;

	Blink::ConfigLoader* loader = nullptr;

	ListBox::Ptr rooms_ptr;
	Label::Ptr no_rooms_label_ptr, entering_room_label_ptr;
	EditBox::Ptr room_passw;
	Button::Ptr enter_room_ptr,conn_room_ptr;
public:
	GraphicalMainMenu(bool fullscreen,
					  const string& encr_key,
					  const string& db_name,
					  const sf::Vector2u& win_size);
	~GraphicalMainMenu();

	void create(Blink::ConfigLoader& loader);

private:
	void set_room_box_pos_and_size(ListBox::Ptr ptr);
	void resize_room_list_box(sf::Event::EventType type);
	void set_no_rooms_label_to_center(sf::Event::EventType type);


	void run_create_room_menu(Blink::ConfigLoader& loader);
	void enter_room();
};
};
#endif