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
	else no_rooms = false;
}
GraphicalMainMenu::~GraphicalMainMenu()
{

}
void GraphicalMainMenu::create(Blink::ConfigLoader& loader)
{
	this->loader = &loader;
	auto room_box = ListBox::create();
	room_box->setUserData(room_list_id);
	set_room_box_pos_and_size(room_box);
	gui->add(room_box);
	echo_functions.push_back([&](sf::Event::EventType type)
		{resize_room_list_box(type); });

	
	int counter = 0;
	for (auto& room : rooms)
	{
		tgui::String name = get<0>(room);
		tgui::String id(counter);
		room_box->addItem(name,id);
		counter++;
	}
	rooms_ptr = room_box;

	auto create_room_button = Button::create("create room");
	create_room_button->setUserData(default_id);
	auto creating_lambda = [&]() { run_create_room_menu(loader); };
	create_room_button->onClick(creating_lambda);
	gui->add(create_room_button);

	auto connect_button = Button::create(" connect ");
	connect_button->setUserData(default_id);

	auto create_button_y_pos = create_room_button->getPositionLayout().y +
							   create_room_button->getSizeLayout().y;
	connect_button->setPosition({"0"},create_button_y_pos);
	connect_button->setSize(create_room_button->getSizeLayout());
	gui->add(connect_button);

	if (no_rooms)
	{
		auto no_rooms_label = Label::create("no rooms");
		no_rooms_label->setUserData(no_rooms_id);
		no_rooms_label->setPosition({ "50%" },{ "50%" });
		gui->add(no_rooms_label);
		echo_functions.push_back([&](sf::Event::EventType type)
			{set_no_rooms_label_to_center(type); });
		no_rooms_label_ptr = no_rooms_label;
	}
	
	paramless_echo_functions.push_back([&]() {enter_room(); });

	auto enter_room_password = EditBox::create();
	enter_room_password->setDefaultText("room password");
	enter_room_password->setSize({ "40.00%", "5.0%" });
	enter_room_password->setPosition({ "30%", "40%" });
	enter_room_password->setUserData(default_id);
	enter_room_password->setVisible(false);
	room_passw = enter_room_password;
	gui->add(enter_room_password);

	auto entering_room_label = Label::create();
	entering_room_label->setUserData(default_id);
	entering_room_label->setPosition({"30%","35%"});
	entering_room_label->setTextSize(22);
	entering_room_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
	entering_room_label->setVisible(false);
	entering_room_label_ptr = entering_room_label;
	gui->add(entering_room_label);

	auto enter_room_button = Button::create("enter");
	enter_room_button->setUserData(enter_room_id);
	enter_room_button->setSize({ "18%","5%" });
	enter_room_button->setPosition({ "30%","48%" });
	enter_room_button->setVisible(false);
	enter_room_ptr = enter_room_button;
	gui->add(enter_room_ptr);

	auto conn_room_button = Button::create("connect");
	conn_room_button->setUserData(conn_room_id);
	conn_room_button->setSize({ "18%","5%" });
	conn_room_button->setPosition({ "52%","48%" });
	conn_room_button->setVisible(false);
	conn_room_ptr = conn_room_button;
	gui->add(conn_room_button);
}
void GraphicalMainMenu::set_room_box_pos_and_size(ListBox::Ptr ptr)
{
	tgui::Layout2d size;
	size.x = 96;
	size.y = get_window_size().y;
	ptr->setSize(size);
	tgui::Layout2d pos;
	pos.x = 0;
	pos.y = 42;
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
void GraphicalMainMenu::run_create_room_menu(Blink::ConfigLoader& loader)
{
	CreateRoomMenu* menu = new CreateRoomMenu(get_encr_key(),
											  get_db_name());
	menu->prepare_menu(loader);
	menu->create(loader);
	menu->run();
	if (menu->should_update())
	{
		auto rooms = get_rooms(get_encr_key());
		int counter = this->rooms.size() + 1;
		for (auto& room : rooms)
		{
			bool added = find(this->rooms.begin(), this->rooms.end(), room) != this->rooms.end();
			if (!added)
			{
				no_rooms_label_ptr->setText("");
				rooms_ptr->addItem(get<0>(room), tgui::String(counter));
				this->rooms.push_back(room);
				counter++;
			}
		}
	}
	delete menu;
}
void GraphicalMainMenu::enter_room()
{
	if (rooms_ptr->getSelectedItem() != "")
	{
		room_passw->setVisible(true);
		auto room_name = rooms_ptr->getSelectedItem();
		entering_room_label_ptr->setText("entering `" + room_name +"` room");
		entering_room_label_ptr->setVisible(true);
		enter_room_ptr->setVisible(true);
		conn_room_ptr->setVisible(true);

		rooms_ptr->setSelectedItem("");
		
	}

}