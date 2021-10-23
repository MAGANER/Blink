#include"GraphicalChatMenu.h"
using namespace GraphicalBlink;

void GraphicalChatMenu::init(GuiBase* gui,
							 Blink::ConfigLoader& loader,
							 Blink::GraphicalDecentralysedServerClient* client)
{
	local_gui_ptr = gui;

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
void GraphicalChatMenu::add_message(const string& text)
{	
	auto box = make_default_message_box();
	box->setText(split_text_if_required(text));
	if (!messages.empty())
	{
		for (auto start = messages.begin(); start != (--messages.end());++start)
		{
			auto pos = (*start)->getPosition();
			(*start)->setPosition(pos.x, pos.y - (*start)->getSize().y-30.0f);
		}
		auto last = get_last_message_box();
		auto pos = last->getPosition();

		last->setPosition(pos.x, pos.y - last->getSize().y-30.0f);
		box->setPosition(pos);
	}
	else
	{
		//set first message
		auto pos = text_input_ptr->getPosition();
		box->setPosition(pos.x, pos.y - box->getSize().y-30.0f);
	}
	messages.push_back(box);
	local_gui_ptr->add(box);
	
}
string GraphicalChatMenu::split_text_if_required(const string& _text)
{
	string text = _text;
	if (text.size() > 20)
	{
		namespace so = Blink::StringOperations;
		auto words = so::split(text, ' ');
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
		return __text;
	}
	return text;
}
Label::Ptr GraphicalChatMenu::make_default_message_box()
{
	auto box = Label::create();
	box->getSharedRenderer()->setBackgroundColor(tgui::Color::Magenta);
	box->setAutoSize(true);
	box->setUserData(-1);
	box->setMouseCursor(tgui::Cursor::Type::Text);

	return box;
}