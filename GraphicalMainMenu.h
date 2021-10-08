#ifndef GRAPHICAL_MAIN_MENU_H
#define GRAPHICAL_MAIN_MENU_H
#include"DataBaseProcessor.h"
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#include"TGUI/Widgets/ListBox.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/ChildWindow.hpp"
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
};
};
#endif