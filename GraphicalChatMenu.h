#ifndef GRAPHICAL_CHAT_MENU_H
#define GRAPHICAL_CHAT_MENU_H
#ifndef COMMON_HEADERS_H
#define COMMON_HEADERS_H
#include"ConfigLoader.h"
#endif
#include"TGUI/Core.hpp"
#include"TGUI/Widgets/TextArea.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/Button.hpp"
#include<iostream>
#include<string>
#include"GraphicalDecentralysedServerClient.h"
#include"StringOperations.hpp"
#define step 15.0f //number of puxels between messages P.S. constant just doesn't work(wtf)
namespace GraphicalBlink
{
using namespace tgui;
using namespace std;

struct MessageToShow
{
	MessageToShow(const string& text, bool mine) :text(text), mine(mine)
	{
	}
	~MessageToShow(){}
	string text;
	bool mine;//if true than left align, else right
};
class GraphicalChatMenu
{
public:
	enum class MsgAlign
	{
		Left, Right
	};
private:
	TextArea::Ptr text_input_ptr;
	bool should_send = false;

	vector<Label::Ptr> my_messages;//left align
	vector<Label::Ptr> another_messages;//right align

	GuiBase* local_gui_ptr = nullptr;
	sf::Color message_background_color;

public:
	GraphicalChatMenu()
	{
	}
	~GraphicalChatMenu()
	{
	}

	void init(GuiBase* gui,
			  Blink::ConfigLoader& loader,
			  Blink::GraphicalDecentralysedServerClient* client);

	void add_message(const MessageToShow& msg);

	bool _should_send() { return should_send; }
	void _should_send(bool flag) { should_send = flag; }
	string get_text_to_send();
private:
	Label::Ptr make_default_message_box();
	Label::Ptr get_last_message_box(vector<Label::Ptr>& messages) { return *(--messages.end()); }
	void move_messages_up(vector<Label::Ptr>& messages,MsgAlign align);

	string split_text_if_required(const string& _text);
	float get_message_box_x_pos(MsgAlign val, float box_size);
};
};
#endif