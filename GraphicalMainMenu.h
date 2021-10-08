#ifndef GRAPHICAL_MAIN_MENU_H
#define GRAPHICAL_MAIN_MENU_H
#include"DataBaseProcessor.h"
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#include"TGUI/Widgets/ListBox.hpp"
#include"TGUI/Widgets/Button.hpp"
namespace GraphicalBlink
{
using namespace Blink;
class GraphicalMainMenu :public DataBaseProcessor,
						 public BaseGraphicalMenu
{
public:
	GraphicalMainMenu(bool fullscreen,
					  const string& encr_key,
					  const string& db_name);
	~GraphicalMainMenu();

	void create(Blink::ConfigLoader& loader);

private:
	void set_room_box_pos_and_size(ListBox::Ptr ptr);
	void resize_room_list_box(sf::Event::EventType type);
};
};
#endif