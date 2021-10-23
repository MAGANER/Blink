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
namespace GraphicalBlink
{
using namespace tgui;
using namespace std;
class GraphicalChatMenu
{
private:
	TextArea::Ptr text_input_ptr;
	bool should_send = false;

	vector<Label::Ptr> messages;

	GuiBase* local_gui_ptr = nullptr;
	tgui::Color message_background_color;
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

	void add_message(const string& text);

	bool _should_send() { return should_send; }
	void _should_send(bool flag) { should_send = flag; }
	string get_text_to_send();
private:
	Label::Ptr make_default_message_box();
	Label::Ptr get_last_message_box() { return *(--messages.end()); }

	string split_text_if_required(const string& _text);
};
};
#endif