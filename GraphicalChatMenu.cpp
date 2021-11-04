#include"GraphicalChatMenu.h"
using namespace GraphicalBlink;

void GraphicalChatMenu::init(GuiBase* gui,
							 Blink::ConfigLoader& loader,
							 Blink::GraphicalDecentralysedServerClient* client,
							 int win_width,
							 const string& room_name)
{
	//create basical gui
	local_gui_ptr = gui;

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
	gui->add(send);

	auto exit = Button::create("exit");
	exit->setSize({ "10%","4%" });
	exit->setPosition({ "88%","85%" });
	exit->setUserData(-1);
	exit->onPress([&]() {this->exit = true; });
	gui->add(exit);



	auto background = Label::create();
	background->getSharedRenderer()->setBackgroundColor(message_background_color);
	background->setUserData(-1);
	background->setMouseCursor(tgui::Cursor::Type::Arrow);
	background->setWidth(win_width);
	background->setHeight(40);
	gui->add(background);

	auto _room_name = Label::create();
	auto name = "`" + room_name + "`" + " room";
	_room_name->setText(name);
	_room_name->setTextSize(24);
	_room_name->setPosition((win_width / 2)-_room_name->getSize().x, 5);
	gui->add(_room_name);
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

	auto name = msg.mine ? "you:\n " : msg.name + ":\n";
	box->setText(split_text_if_required(name + msg.text));

	MsgAlign align = msg.mine ? MsgAlign::Left : MsgAlign::Right;

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
}