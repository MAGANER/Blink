#include"GraphicalChatMenu.h"
using namespace GraphicalBlink;

void GraphicalChatMenu::init(GuiBase* gui,
							 Blink::ConfigLoader& loader,
							 Blink::GraphicalDecentralysedServerClient* client,
							 int win_width,
							 const string& room_name)
{
	//create basical gui
	message_background_color = loader.get_message_background_color();

	auto input = TextArea::create();
	input->setSize({ "80%","10%" });
	input->setPosition({ "5%","80%" });
	input->setUserData(-1);
	gui->add(input);
	text_input_ptr = input;

	auto send = Button::create("send");
	send->setSize({ "10%","4%" });
	send->setPosition({ "88%","80%" });
	send->setUserData(-1);
	send->onPress([&]() {
		should_send = true;
		});
	send_ptr = send;
	gui->add(send);

	auto exit = Button::create("exit");
	exit->setSize({ "10%","4%" });
	exit->setPosition({ "88%","85%" });
	exit->setUserData(-1);
	exit->onPress([&]() {this->exit = true; });
	exit_ptr = exit;
	gui->add(exit);


	auto make_link = Button::create("make link");
	make_link->setSize({ "10%","4%" });
	make_link->setPosition({ "88%","90%" });
	make_link->setUserData(-1);
	make_link->onPress([&]() {make_additional_link(); });
	make_ptr = make_link;
	gui->add(make_link);

	auto background = Label::create();
	background->getSharedRenderer()->setBackgroundColor(message_background_color);
	background->setUserData(-1);
	background->setMouseCursor(tgui::Cursor::Type::Arrow);
	background->setWidth(win_width);
	background->setHeight(40);
	name_background_ptr = background;
	gui->add(background);

	auto _room_name = Label::create();
	auto name = "`" + room_name + "`" + " room";
	_room_name->setText(name);
	_room_name->setTextSize(24);
	_room_name->setPosition((win_width / 2)-_room_name->getSize().x, 5);
	gui->add(_room_name);
	room_name_ptr = _room_name;

	local_gui_ptr = gui;
	this->loader = &loader;
}
string GraphicalChatMenu::get_text_to_send()
{
	auto text = text_input_ptr->getText().toStdString();
	text_input_ptr->setText("");
	should_send = false;
	return text;
}
void GraphicalChatMenu::add_message(const MessageToShow& msg)
{	
	auto box = make_default_message_box();

	auto name = msg.mine ? "you:" : msg.name + ":";
	box->setText(split_text_if_required(name + msg.text));
	box->setSize(text_input_ptr->getSize().x,box->getSize().y+5);

	MsgAlign align = MsgAlign::Left;

	auto pos  = text_input_ptr->getPosition();
	auto size = box->getSize();
	box->setPosition(get_message_box_x_pos(align, size.x), pos.y - size.y);
	messages.push_back(box);

	move_messages(STEP);

	local_gui_ptr->add(box);	
}
void GraphicalChatMenu::move_messages(float step)
{
	//move messages when new one is added
	for (int i = messages.size() - 1; i > -1; --i)
	{
		auto msg = messages[i];
		if (i + 1 < messages.size())
		{
			auto next = messages[i + 1];
			auto curr = messages[i];

			auto curr_size = curr->getSize().y;
			auto next_size = next->getSize().y;
			auto size = next_size > curr_size ? next_size: curr_size;

			auto y = next->getPosition().y-size -  step;


			messages[i]->setPosition(curr->getPosition().x, y);
		}
		else
		{
			auto y = messages[i]->getPosition().y - messages[i]->getSize().y -step;
			messages[i]->setPosition(messages[i]->getPosition().x, y);
		}
	}
}
string GraphicalChatMenu::split_text_if_required(const string& _text)
{
	//split text into several lines

	string text = _text;
	if (text.size() > 20)
	{
		namespace so = Blink::StringOperations;
		auto words = so::simple_split(text, ' ');
		vector<string> fitable_words;
		string fitable_word;
		for (auto& word : words)
		{
			fitable_word += word + " ";
			if (fitable_word.size() > 20)
			{
				fitable_words.push_back(fitable_word);
				fitable_word.clear();
			}
		}
		fitable_words.push_back(fitable_word);//last one

		string __text;
		for (auto& word : fitable_words)
		{
			__text += word + "\n";
		}

		//erase useless spaces
		reverse(__text.begin(), __text.end());
		string new_text;
		int i = 0;
		for (i; i < __text.size(); i++)
		{
			string test; test += __text[i];
			if (test != " " || test[0] != '\n')
				break;
		}
		
		new_text = Functools::slice(__text, i+1, __text.size());
		
		reverse(new_text.begin(), new_text.end());
		return new_text;
	}
	return text;
}
Label::Ptr GraphicalChatMenu::make_default_message_box()
{
	//prepare message box

	auto box = Label::create();
	box->getRenderer()->setBackgroundColor(message_background_color);
	box->setAutoSize(true);
	box->setUserData(-1);
	box->setMouseCursor(tgui::Cursor::Type::Arrow);

	return box;
}
float GraphicalChatMenu::get_message_box_x_pos(MsgAlign val, float box_size)
{
	//current user messages must be aligned to left
	//received ones must be aligned righter
	if (val == MsgAlign::Left)
		return text_input_ptr->getPosition().x;
	else 
		return text_input_ptr->getSize().x - box_size;
}
void GraphicalChatMenu::process_scroll(int direction)
{
	//move text up/or down

	auto move = [&](float step)
	{
		for (auto& msg : messages)
		{
			//it must be "hidden" behind the input box
			msg->moveToBack();

			auto x = msg->getPosition().x;
			auto y = msg->getPosition().y + step;

			//if message is under text_input, than hide it
			//else show it again
			if (y > text_input_ptr->getPosition().y)
				msg->setVisible(false);
			else
				msg->setVisible(true);

			msg->setPosition(x, y);
		}
	};

	auto input_high_y = text_input_ptr->getPosition().y;
	if (direction > 0)
	{
		//move down
		auto first_msg_high_y = messages[0]->getPosition().y;
		bool can_move = first_msg_high_y  < input_high_y;
		if (can_move)
			move(10.0f);
	}
	else
	{
		//move up
		auto last = *--messages.end();
		auto last_msg_bottom_y = last->getPosition().y + last->getSize().y + STEP;
		if(last_msg_bottom_y > input_high_y)
			move(-10.0f);
	}
}
void GraphicalChatMenu::load_messages(vector<MessageToShow>& messages)
{
	for (auto& msg : messages)
	{
		add_message(msg);
	}
	name_background_ptr->moveToFront();
	room_name_ptr->moveToFront();
}
void GraphicalChatMenu::resize(int win_width)
{
	//move received messages lefter
	for (auto& msg : messages)
	{
		//it message's x == input's x, than it's user's  own messages
		if (msg->getText().toStdString().find("you:") == string::npos)
		{
			auto x= get_message_box_x_pos(MsgAlign::Right, msg->getSize().x);
			msg->setPosition(x, msg->getPosition().y);
		}
		else
		{
			msg->setPosition(text_input_ptr->getPosition().x, msg->getPosition().y);
		}
	}

	name_background_ptr->setSize(win_width, name_background_ptr->getSize().y);
	room_name_ptr->setPosition((win_width / 2) - room_name_ptr->getSize().x, 5);
}
void GraphicalChatMenu::hide(bool flag)
{
	for (auto& wid : local_gui_ptr->getWidgets())
		if (flag)
			wid->setVisible(true);
		else
			wid->setVisible(false);
}
void GraphicalChatMenu::make_additional_link()
{
	hide(false);
	additional_link_creating_menu_is_active = true;
	//make link gui init

	if (!add_link_cr_m_created)
	{
		add_link_cr_m_created = true;
		auto label = Label::create("make additional link");
		label->setUserData(-1);
		label->setPosition({ "30%","35%" });
		label->setTextSize(20);
		label->getSharedRenderer()->setTextColor(loader->get_enter_menu_label_color());
		label->getSharedRenderer()->setBackgroundColor(tgui::Color::Transparent);
		local_gui_ptr->add(label);
		label_ptr = label;

		auto __name = EditBox::create();
		__name->setDefaultText("invite link path/recepient's e-mail");
		__name->setSize({ "40%","5.0%" });
		__name->setPosition({ "30%", "40%" });
		__name->setUserData(-1);
		local_gui_ptr->add(__name);
		name_path_ptr = __name;


		auto nick = EditBox::create();
		nick->setDefaultText("recepient name");
		nick->setSize({ "40%","5.0%" });
		nick->setPosition({ "30%", "48%" });
		nick->setUserData(-1);
		local_gui_ptr->add(nick);
		recepient_name_ptr = nick;

		auto save = Button::create("save");
		save->setUserData(-1);
		save->setSize({ "18%","5%" });
		save->setPosition({ "30%","54%" });
		save->onPress([&]() { save_link(); });
		local_gui_ptr->add(save);
		save_link_ptr = save;

		auto send = Button::create("send");
		send->setUserData(-1);
		send->setSize({ "18%","5%" });
		send->setPosition({ "52%","54%" });
		send->onPress([&]() {send_link(); });
		local_gui_ptr->add(send);
		send_link_ptr = send;
	}
	else
	{
		label_ptr->setVisible(true);
		name_path_ptr->setVisible(true);
		recepient_name_ptr->setVisible(true);
		save_link_ptr->setVisible(true);
		send_link_ptr->setVisible(true);
	}
}
void GraphicalChatMenu::save_link()
{

	auto additional_data = name_path_ptr->getText().toStdString();
	auto recepient_name = recepient_name_ptr->getText().toStdString();
	bool not_empty = !additional_data.empty() and !recepient_name.empty();
	if(not_empty)
		client->create_invite_link_to_save(link_data->port,
								link_data->room_name,
						     	link_data->password,
								true,
								additional_data,
								recepient_name);

	if (not_empty)
	{
		hide_link_creating_menu();
	}
}
void GraphicalChatMenu::send_link()
{
	auto additional_data = name_path_ptr->getText().toStdString();
	auto recepient_name = recepient_name_ptr->getText().toStdString();
	bool not_empty = !additional_data.empty() and !recepient_name.empty();
	if (not_empty)
		client->create_invite_link_to_send(link_data->port,
			link_data->room_name,
			link_data->password,
			true,
			additional_data,
			recepient_name);

	if (not_empty)
	{
		hide_link_creating_menu();
	}
}
void GraphicalChatMenu::hide_link_creating_menu()
{
	if (additional_link_creating_menu_is_active)
	{
		additional_link_creating_menu_is_active = false;

		hide(true);
		label_ptr->setVisible(false);
		name_path_ptr->setText("");
		name_path_ptr->setVisible(false);
		recepient_name_ptr->setText("");
		recepient_name_ptr->setVisible(false);
		send_link_ptr->setVisible(false);
		save_link_ptr->setVisible(false);

		name_background_ptr->getSharedRenderer()->setBackgroundColor(message_background_color);
		name_background_ptr->setVisible(true);
	}
}