#include "GraphicalMainMenu.h"
using namespace GraphicalBlink;

GraphicalMainMenu::GraphicalMainMenu(bool fullscreen,
									 const string& encr_key,
									 const string& db_name,
									 const sf::Vector2u& win_size,
									 const string& user_name):
	BaseGraphicalMenu(fullscreen,win_size),
	DataBaseProcessor(encr_key,db_name)
{
	room_gate_menu = new RoomGateMenu(encr_key, db_name);
	
	this->user_name = user_name;

	rooms = get_rooms(encr_key);
	if (rooms.empty())no_rooms = true;
	else no_rooms = false;
}
GraphicalMainMenu::~GraphicalMainMenu()
{
	delete room_gate_menu;
	if (conn_menu != nullptr) delete conn_menu;
	if (client != nullptr)    delete client;
	if (chat_menu != nullptr) delete chat_menu;
}
void GraphicalMainMenu::create(Blink::ConfigLoader& loader)
{
	this->loader = &loader;
	room_gate_menu->init(gui, *this->loader,init_chat, start_room);
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
	connect_button->onPress([&]() {connect_link(loader); });

	auto create_button_y_pos = create_room_button->getPositionLayout().y +
							   create_room_button->getSizeLayout().y;
	connect_button->setPosition({"0"},create_button_y_pos);
	connect_button->setSize(create_room_button->getSizeLayout());
	gui->add(connect_button);


	auto conn_button_y_pos = connect_button->getPositionLayout().y +
							 connect_button->getSize().y;
	auto exit_button = Button::create(" exit ");
	exit_button->setPosition(0, conn_button_y_pos);
	exit_button->setSize(connect_button->getSizeLayout());
	exit_button->onPress([&]() {exit(); });
	gui->add(exit_button);

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
	
	paramless_echo_functions.push_back([&]() {room_gate_menu->enter_room(rooms_ptr); });
	paramless_echo_functions.push_back([&]() {process_chat(); });
	paramless_echo_functions.push_back([&]() {create_link(); });
	paramless_echo_functions.push_back([&]() {recreate_this_menu(loader); });

}
void GraphicalMainMenu::create_link()
{
	//create client and save/send link
	//callback function for button to create link
	if (start_room)
	{
		room_gate_menu->init_inv_link_creating(init_chat);
		start_room = false;
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
	pos.y = 66;
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
void GraphicalMainMenu::process_chat()
{
	if (init_chat)
	{
		gui->removeAllWidgets();
		init_chat = false;

		bool can_connect_with_link = false;
		if (conn_menu != nullptr)
		{
			can_connect_with_link = conn_menu->can_connect();
		}
		if (!can_connect_with_link)
		{
			if (room_gate_menu->is_starting_room())
			{
				auto name_passw = room_gate_menu->get_room_name_password();
				NetBaseData data(user_name, name_passw.first, get_encr_key(), get_db_name());
				auto none = command_hash();
				client =
					new GraphicalDecentralysedServerClient(none,
						name_passw.second,
						data,
						room_gate_menu->get_link_creator_additional_data(),
						true,
						room_gate_menu->_save_link());
			}
			else
			{
				auto name_passw = room_gate_menu->get_room_name_password();
				NetBaseData data(user_name, name_passw.first, get_encr_key(), get_db_name());
				auto none = command_hash();
				client =
					new GraphicalDecentralysedServerClient(none, name_passw.second, data, false, false);
			}
			
			client->prepare();
			main_echo_function = [&]() {_main_echo_function(); };
		}
		else
		{
			//conn menu is not nullptr
			auto keys = conn_menu->get_encrpyption_data();
			auto conn_data = conn_menu->get_connection_data();
			
			auto name_passw = room_gate_menu->get_room_name_password();
			NetBaseData data(user_name, name_passw.first, get_encr_key(), get_db_name());
			auto none = command_hash();

			client = new GraphicalDecentralysedServerClient(none, name_passw.second, data, true, false);
			client->set_ip_and_port_to_connect(conn_data.ip,conn_data.port);
			client->is_connecting(true);
			client->set_key_iv(keys.data);

			client->prepare();
			main_echo_function = [&]() {_main_echo_function(); };
		}

		chat_menu = new GraphicalChatMenu();
		chat_menu->init(gui, *loader,client);

		auto msgs = get_saved_messages();
		chat_menu->load_messages(msgs);


		should_run_paramless_echo_function = false;

		process_mouse_wheel = true;
		echo_mouse_wheel_function = [&](int direction) {chat_menu->process_scroll(direction); };
	}
}
void GraphicalMainMenu::connect_link(Blink::ConfigLoader& loader)
{
	//call back function for connect button
	gui->removeAllWidgets();
	if (conn_menu == nullptr)
	{
		conn_menu =  new GraphicalConnectingSubMenu();
		conn_menu->init_menu(gui, loader,init_chat);
	}
}
void GraphicalMainMenu::recreate_this_menu(Blink::ConfigLoader& loader)
{
	if (conn_menu != nullptr)
	{
		if (conn_menu->return_to_prev_menu())
		{
			update_win();
			sf::sleep(sf::Time(sf::seconds(2.0f))); //wait to show it
			gui->removeAllWidgets();
			delete conn_menu;
			conn_menu = nullptr;
			create(loader);
		}
	}
}
void GraphicalMainMenu::_main_echo_function()
{
	if (chat_menu->_should_send())
	{
		auto text_to_send = chat_menu->get_text_to_send();
		MessageToShow msg(text_to_send,user_name, true);
		chat_menu->add_message(msg);
		client->set_text_to_send(text_to_send);
	}
	client->run_in_window();

	if (client->has_message_to_show())
	{
		auto msg = client->get_message_to_show();
		MessageToShow _msg(msg->text,msg->name, false);
		chat_menu->add_message(_msg);
	}
}
vector<MessageToShow> GraphicalMainMenu::get_saved_messages()
{
	vector<MessageToShow> messages;
	auto loaded = get_messages(room_gate_menu->get_room_name_password().first);
	for (auto& msg : loaded)
	{
		cout << msg.first << ":" << msg.second << endl;
		auto name = msg.first;
		bool mine = name == user_name ? true : false;
		auto text = msg.second;
		MessageToShow _msg(text, name, mine);
		messages.push_back(_msg);
	}
	return messages;
}