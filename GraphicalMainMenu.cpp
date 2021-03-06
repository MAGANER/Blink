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
	//this menu always active, but it must be changed to decrease used RAM
	room_gate_menu = new RoomGateMenu(encr_key, db_name);
	
	this->encr_key = encr_key;
	this->user_name = user_name;
}
GraphicalMainMenu::~GraphicalMainMenu()
{
	delete room_gate_menu;
	if (conn_menu != nullptr) delete conn_menu;
	//if (client != nullptr)    delete client; 
	if (chat_menu != nullptr) delete chat_menu;
	if (create_room_menu != nullptr) delete create_room_menu;
}
void GraphicalMainMenu::create(Blink::ConfigLoader& loader)
{
	this->loader = &loader;
	room_gate_menu->init(gui, *this->loader,init_chat, start_room);


	//init rooms' list widget
	auto room_box = ListBox::create();
	room_box->getSharedRenderer()->setBackgroundColor(loader.get_message_background_color());
	room_box->getSharedRenderer()->setTextColor(loader.get_input_field_text_color());
	room_box->setUserData(room_list_id);
	set_room_box_pos_and_size(room_box);
	gui->add(room_box);


	//load rooms
	rooms = get_rooms(encr_key);
	if (rooms.empty())no_rooms = true;
	else no_rooms = false;
	int counter = 0;
	for (auto& room : rooms)
	{
		tgui::String name = get<0>(room);
		tgui::String id(counter);
		room_box->addItem(name,id);
		counter++;
	}
	rooms_ptr = room_box;
	//**


	//set first button to create room
	auto create_room_button = Button::create("create room");
	create_room_button->getSharedRenderer()->setBackgroundColor(loader.get_input_field_background_color());
	create_room_button->getSharedRenderer()->setTextColor(loader.get_input_field_text_color());
	create_room_button->setUserData(default_id);
	auto creating_lambda = [&]() {
		if (curr_sub_menu == ActiveSubMenu::none)
		{
			run_create_room_menu(loader);
			curr_sub_menu = ActiveSubMenu::creating;
		}
	};
	create_room_button->onClick(creating_lambda);
	gui->add(create_room_button);
	//**


	//connecting button is based on previous button
	auto connect_button = Button::create(" connect ");
	connect_button->setUserData(default_id);
	connect_button->onPress([&]() {
		if (curr_sub_menu == ActiveSubMenu::none)
		{
			connect_link(loader);
			curr_sub_menu = ActiveSubMenu::connecting;
		}
		});

	auto create_button_y_pos = create_room_button->getPositionLayout().y +
							   create_room_button->getSizeLayout().y;
	connect_button->setPosition({"0"},create_button_y_pos);
	connect_button->setSize(create_room_button->getSizeLayout());
	gui->add(connect_button);
	//**


	//third and last button is based on previous connecting button
	auto conn_button_y_pos = connect_button->getPositionLayout().y +
							 connect_button->getSize().y;
	auto exit_button = Button::create(" exit ");
	exit_button->setUserData(default_id);
	exit_button->setPosition(0, conn_button_y_pos);
	exit_button->setSize(connect_button->getSizeLayout());
	exit_button->onPress([&]() {exit(); });
	gui->add(exit_button);


	//set 'no rooms' label, if there is no any rooms
	if (no_rooms)
	{
		auto no_rooms_label = Label::create("no rooms");
		no_rooms_label->setUserData(no_rooms_id);
		no_rooms_label->setPosition({ "48%" },{ "50%" });
		no_rooms_label_ptr = no_rooms_label;
		gui->add(no_rooms_label);

		echo_functions.push_back([&](sf::Event::EventType type)
			{set_no_rooms_label_to_center(type); });
		
	}
	else
	{
		auto no_rooms_label = Label::create("");
		no_rooms_label_ptr = no_rooms_label;
	}
	

	//init every echo functions
	echo_functions.push_back([&](sf::Event::EventType type)
		{resize_room_list_box(type); });
	echo_functions.push_back([&](sf::Event::EventType type)
		{resize_chat_widgets(type); });
	paramless_echo_functions.push_back([&]() {enter_room();   });
	paramless_echo_functions.push_back([&]() {process_chat(); });
	paramless_echo_functions.push_back([&]() {leave_creating_room_menu(); });
	paramless_echo_functions.push_back([&]() {clear_sub_menu(); });
}
void GraphicalMainMenu::resize_chat_widgets(sf::Event::EventType type)
{
	//resize if chat is active
	if (type == sf::Event::Resized && client != nullptr)
	{
		chat_menu->resize(get_window_width());
	}
}
void GraphicalMainMenu::enter_room()
{
	if (curr_sub_menu == ActiveSubMenu::none &&
		rooms_ptr->getSelectedItem() != "")
	{
		room_gate_menu->enter_room(rooms_ptr);
		curr_sub_menu = ActiveSubMenu::entering;
	}
}
void GraphicalMainMenu::leave_creating_room_menu()
{
	if (create_room_menu != nullptr)
	{
		if (create_room_menu->can_leave())
		{
			create_room_menu->clear();
			for (auto& widget : gui->getWidgets())
			{
				if (widget->getUserData<int>() == no_rooms_id)
				{
					widget->setVisible(false);
				}
				if (widget->getUserData<int>() == room_list_id)
				{
					widget->cast<ListBox>()->addItem(create_room_menu->get_room_name());
				}
			}

			curr_sub_menu = ActiveSubMenu::none;
			delete create_room_menu;
			create_room_menu = nullptr;
		}
	}
}
void GraphicalMainMenu::set_room_box_pos_and_size(ListBox::Ptr ptr)
{
	//set default data for room's list widget
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
	//resize if user change the size of window
	//resize it only when it's not active chat
	if (type == sf::Event::EventType::Resized && client == nullptr)
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
	//if user resizes window, then this widget must be changed
	//resize it only when it's not active chat
	if (type == sf::Event::EventType::Resized && client == nullptr)
	{
		//get all widgets and find required one
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
	//create room menu if it's not created yet
	if (create_room_menu == nullptr)
	{
		cout <<"key:"<< get_encr_key() << endl;
		create_room_menu = new CreateRoomMenu(get_encr_key(), get_db_name());
		create_room_menu->init(gui, loader,no_rooms_label_ptr);
		no_rooms_label_ptr->setVisible(false);
	}
}
void GraphicalMainMenu::process_chat()
{
	if (init_chat)
	{
		//remove everything and don't allow to come here again!
		gui->removeAllWidgets();
		init_chat = false;

		string _room_name;
		//check first case if user connects  with link
		bool can_connect_with_link = false;
		if (conn_menu != nullptr)
		{
			can_connect_with_link = conn_menu->can_connect();
		}

		if (!can_connect_with_link)
		{
			if (room_gate_menu->is_starting_room())
			{
				cout << "start room:" << endl;
				auto name_passw = room_gate_menu->get_room_name_password();
				NetBaseData data(user_name, name_passw.first, get_encr_key(), get_db_name());
				auto none = command_hash();
				client =
					new GraphicalDecentralysedServerClient(none,
						name_passw.second,
						data,
						"",
						true,
						room_gate_menu->_save_link(),
						"");

				room_gate_menu->stop_starting();
			}
			else
			{
				
				cout << "enter room:" << endl;
				auto name_passw = room_gate_menu->get_room_name_password();
				NetBaseData data(user_name, name_passw.first, get_encr_key(), get_db_name());

				auto none = command_hash();
				client =
					new GraphicalDecentralysedServerClient(none, name_passw.second, data, "", false, false, "", false);
			}
			
			client->prepare();
		}
		else
		{
			//conn menu is not nullptr
			auto keys = conn_menu->get_encrpyption_data();
			auto conn_data = conn_menu->get_connection_data();
			
			auto name_passw = room_gate_menu->get_room_name_password();
			NetBaseData data(user_name, conn_data.room, get_encr_key(), get_db_name());
			_room_name = conn_data.room;
			auto none = command_hash();
			cout << "i want to get the room name!" <<data.room_name<<" "<< endl;
			client = new GraphicalDecentralysedServerClient(none, name_passw.second, data,"", false,false,"", false);
			client->set_ip_and_port_to_connect(conn_data.ip,to_string(PORT));
			client->is_connecting(true);
			client->set_room_name(conn_data.room);
			client->set_key_iv(keys.data);


			client->prepare();
		}

		//prepare chat menu
		chat_menu = new GraphicalChatMenu();
		chat_menu->init(gui, *loader,client,get_window_width(), room_gate_menu->get_room_name_password().first);

		auto msgs = get_saved_messages();
		chat_menu->load_messages(msgs);
		chat_menu->set_link_data(client->get_link_data());
		chat_menu->set_client_ptr(client);

		if(!_room_name.empty())chat_menu->set_room_name(_room_name);

		//no more sub menu is used
		curr_sub_menu = ActiveSubMenu::none;

		should_run_paramless_echo_function = false;

		//to scroll text
		process_mouse_wheel = true;
		echo_mouse_wheel_function = [&](int direction) {chat_menu->process_scroll(direction); };
		main_echo_function = [&]() { _main_echo_function(); };
	}
}
void GraphicalMainMenu::connect_link(Blink::ConfigLoader& loader)
{
	//call back function for connect button
	if (conn_menu == nullptr)
	{
		conn_menu =  new GraphicalConnectingSubMenu();
		conn_menu->init_menu(gui, loader, init_chat, user_name, no_rooms_label_ptr);
	}
}
void GraphicalMainMenu::_main_echo_function()
{
	if (client != nullptr)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			chat_menu->hide_additional_link_creating_menu();

		if (chat_menu->_should_send())
		{
			auto text_to_send = chat_menu->get_text_to_send();
			MessageToShow msg(text_to_send, user_name, true);
			chat_menu->add_message(msg);
			client->set_text_to_send(text_to_send);
		}
		client->run_in_window();


		if (client->has_message_to_show())
		{
			auto msg = client->get_message_to_show();
			MessageToShow _msg(msg->text, msg->name, false);
			chat_menu->add_message(_msg);
		}

		if (chat_menu->should_exit())
		{
			//this function will not be running
			client->exit();
			client->save_offline_clients();

			paramless_echo_functions.clear();
			echo_functions.clear();
			should_run_paramless_echo_function = true;
			process_mouse_wheel = false;

			
			gui->removeAllWidgets();

			delete chat_menu;
			chat_menu = nullptr;

			delete client;
			client = nullptr;

			create(*loader);
		}
	}
}
vector<MessageToShow> GraphicalMainMenu::get_saved_messages()
{
	//get messages from DB

	vector<MessageToShow> messages;
	auto loaded = get_messages(room_gate_menu->get_room_name_password().first);
	for (auto& msg : loaded)
	{
		auto name = msg.first;
		bool mine = name == user_name ? true : false;
		auto text = msg.second;
		MessageToShow _msg(text, name, mine);
		messages.push_back(_msg);
	}
	return messages;
}
void GraphicalMainMenu::clear_sub_menu()
{
	//just erase this menu
	if (Keyboard::isKeyPressed(Keyboard::Escape))
	{
		if (curr_sub_menu == ActiveSubMenu::entering)
		{
			room_gate_menu->clear(gui);
			curr_sub_menu = ActiveSubMenu::none;
		}
		if (curr_sub_menu == ActiveSubMenu::creating)
		{
			create_room_menu->clear();
			delete create_room_menu;
			create_room_menu = nullptr;
			curr_sub_menu = ActiveSubMenu::none;
		}
		if (curr_sub_menu == ActiveSubMenu::connecting)
		{
			conn_menu->clear(gui);
			delete conn_menu;
			conn_menu = nullptr;
			curr_sub_menu = ActiveSubMenu::none;
		}
	}

	if(create_room_menu != nullptr)
		if (create_room_menu->can_leave())
		{
			delete create_room_menu;
			create_room_menu = nullptr;
		}
}