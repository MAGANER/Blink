#include "GraphicalMainMenu.h"
using namespace GraphicalBlink;

GraphicalMainMenu::GraphicalMainMenu(bool fullscreen,
									 const string& encr_key,
									 const string& db_name):
	BaseGraphicalMenu(fullscreen),
	DataBaseProcessor(encr_key,db_name)
{

}
GraphicalMainMenu::~GraphicalMainMenu()
{

}
void GraphicalMainMenu::create(Blink::ConfigLoader& loader)
{
	auto room_box = ListBox::create();
	room_box->setUserData(1);
	set_room_box_pos_and_size(room_box);
	gui->add(room_box);
	echo_functions.push_back([&](sf::Event::EventType type)
		{resize_room_list_box(type); });


	auto create_room_button = Button::create("create room");
	create_room_button->setUserData(0);
	gui->add(create_room_button);
}
void GraphicalMainMenu::set_room_box_pos_and_size(ListBox::Ptr ptr)
{
	tgui::Layout2d size;
	size.x = 96;
	size.y = get_window_size().y;
	ptr->setSize(size);
	tgui::Layout2d pos;
	pos.x = 0;
	pos.y = 20;
	ptr->setPosition(pos);
}
void GraphicalMainMenu::resize_room_list_box(sf::Event::EventType type)
{
	if (type == sf::Event::EventType::Resized)
	{
		auto widgets = gui->getWidgets();
		for (auto& wid : widgets)
		{
			if (wid->getUserData<int>() == 1)
			{
				tgui::Layout2d size = wid->getSizeLayout();
				size.y = get_window_size().y;
				wid->setSize(size);
			}
		}
	}
}