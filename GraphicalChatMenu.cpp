#include"GraphicalChatMenu.h"
using namespace GraphicalBlink;

void GraphicalChatMenu::init(GuiBase* gui,
							 Blink::ConfigLoader& loader,
							 Blink::GraphicalDecentralysedServerClient* client)
{
	local_gui_ptr = gui;

	message_background_color = loader.get_message_background_color();

	auto input = TextArea::create();
	input->setSize({ "70%","10%" });
	input->setPosition({ "3%","80%" });
	input->setUserData(-1);
	gui->add(input);
	text_input_ptr = input;

	auto send = Button::create("send");
	send->setSize({ "20%","5%" });
	send->setPosition({ "75%","80%" });
	send->setUserData(-1);
	send->onPress([&]() {
		should_send = true;
		});
	gui->add(send);

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
	cout << box->getText() << ":" << endl;
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
	for (int i = messages.size() - 1; i > -1; --i)
	{
		auto msg = messages[i];
		if (i + 1 < messages.size())
		{
			auto next = messages[i + 1];
			
			auto size = next->getSize().y > messages[i]->getSize().y ? next->getSize().y :
											messages[i]->getSize().y;

			auto y = next->getPosition().y-size -  step;


			messages[i]->setPosition(messages[i]->getPosition().x, y);
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
	auto box = Label::create();
	box->getSharedRenderer()->setBackgroundColor(message_background_color);
	box->setAutoSize(true);
	box->setUserData(-1);
	box->setMouseCursor(tgui::Cursor::Type::Text);

	return box;
}
float GraphicalChatMenu::get_message_box_x_pos(MsgAlign val, float box_size)
{
	if (val == MsgAlign::Left)
	{
		return text_input_ptr->getPosition().x;
	}
	else 
	{
		return text_input_ptr->getSize().x - box_size;
	}
}
void GraphicalChatMenu::process_scroll(int direction)
{

	auto move = [&](float step)
	{
		for (auto& msg : messages)
		{
			msg->moveToBack();
			auto x = msg->getPosition().x;
			auto y = msg->getPosition().y + step;

			if (y > text_input_ptr->getPosition().y)
				msg->setVisible(false);
			else
				msg->setVisible(true);

			msg->setPosition(x, y);
		}
	};
	if (direction > 0)
	{
		//move down
		if (messages[0]->getPosition().y < text_input_ptr->getPosition().y)
			move(10.0f);
	}
	else
	{
		//move up
		auto last = *--messages.end();
		if(last->getPosition().y+last->getSize().y+STEP > text_input_ptr->getPosition().y)
			move(-10.0f);
	}
}