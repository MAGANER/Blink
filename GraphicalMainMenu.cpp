#include "GraphicalMainMenu.h"
using namespace GraphicalBlink;

GraphicalMainMenu::GraphicalMainMenu(bool fullscreen,
									 const string& encr_key,
									 const string& db_name,
									 const sf::Vector2u& win_size):
	BaseGraphicalMenu(fullscreen,win_size),
	DataBaseProcessor(encr_key,db_name)
{
	rooms = get_rooms(encr_key);
	if (rooms.empty())no_rooms = true;
}
GraphicalMainMenu::~GraphicalMainMenu()
{

}
void GraphicalMainMenu::create(Blink::ConfigLoader& loader)
{
	auto room_box = ListBox::create();
	room_box->setUserData(room_list_id);
	set_room_box_pos_and_size(room_box);
	gui->add(room_box);
	echo_functions.push_back([&](sf::Event::EventType type)
		{resize_room_list_box(type); });


	auto create_room_button = Button::create("create room");
	create_room_button->setUserData(default_id);

	auto creating_lambda = [&]() {run_create_room_menu(); };
	create_room_button->onPress(creating_lambda);

	gui->add(create_room_button);

	if (no_rooms)
	{
		auto no_rooms_label = Label::create("no rooms");
		no_rooms_label->setUserData(no_rooms_id);
		no_rooms_label->setPosition({ "50%" },{ "50%" });
		gui->add(no_rooms_label);
		echo_functions.push_back([&](sf::Event::EventType type)
			{set_no_rooms_label_to_center(type); });
	}
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
			if (wid->getUserData<int>() == room_list_id)
			{
				tgui::Layout2d size = wid->getSizeLayout();
				size.y = get_window_size().y;
				wid->setSize(size);
			}
		}
	}
}
void GraphicalMainMenu::set_no_rooms_label_to_center(sf::Event::EventType type)
{
	if (type == sf::Event::EventType::Resized)
	{
		auto widgets = gui->getWidgets();
		for (auto& wid : widgets)
		{
			if (wid->getUserData<int>() == no_rooms_id)
			{
				wid->setPosition({ "50%" }, { "50%" });
			}
		}
	}
}
void GraphicalMainMenu::run_create_room_menu()
{
	CreateRoomMenu* menu = new CreateRoomMenu(get_encr_key(),
											  get_db_name());
	
	menu->run();
	delete menu;
}