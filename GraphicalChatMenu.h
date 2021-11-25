/*
  This class process input, received messages.
  Scroll them, add new ones e.t.c
*/

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
#include"TGUI/Widgets/EditBox.hpp"
#include<string>
#include"GraphicalDecentralysedServerClient.h"
#include"StringOperations.hpp"
#include"SFML/Window/Mouse.hpp"
#define STEP 10.0f //number of pixels between messages P.S. constant just doesn't work(wtf)
namespace GraphicalBlink
{
using namespace tgui;
using namespace std;

struct MessageToShow
{
	MessageToShow(const string& text,
				  const string& name,
				  bool mine) :text(text), mine(mine),name(name)
	{
	}
	~MessageToShow(){}
	string text;
	string name;
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
	Button::Ptr send_ptr, exit_ptr, make_ptr;

	bool should_send = false;

	vector<Label::Ptr> messages;

	GuiBase* local_gui_ptr      = nullptr;
	Blink::ConfigLoader* loader = nullptr;

	sf::Color message_background_color;

	Label::Ptr room_name_ptr, name_background_ptr;

	//making additional link gui
	Label::Ptr label_ptr;
	EditBox::Ptr name_path_ptr, recepient_name_ptr;
	Button::Ptr send_link_ptr, save_link_ptr;
	//

	bool exit = false;
	bool additional_link_creating_menu_is_active = false;
	bool add_link_cr_m_created = false;

	Blink::LinkData* link_data = nullptr;
	Blink::GraphicalDecentralysedServerClient* client = nullptr;//now owner
public:
	GraphicalChatMenu()
	{
	}
	~GraphicalChatMenu()
	{
	}

	void init(GuiBase* gui,
			  Blink::ConfigLoader& loader,
			  Blink::GraphicalDecentralysedServerClient* client,
			  int win_width,
			  const string& room_name);
	void set_link_data(Blink::LinkData* link_data)
	{
		this->link_data = link_data;
	}
	void set_client_ptr(Blink::GraphicalDecentralysedServerClient* client)
	{
		this->client = client;
	}

	void add_message(const MessageToShow& msg);

	bool _should_send() { return should_send; }
	void _should_send(bool flag) { should_send = flag; }
	string get_text_to_send();
	void load_messages(vector<MessageToShow>& messages);
	void process_scroll(int direction);

	bool should_exit() { return exit; }

	void set_room_name(const string& room_name)
	{
		room_name_ptr->setText("`" + room_name + "` room");
	}
	void resize(int win_width);

	void hide_additional_link_creating_menu()
	{
		if(additional_link_creating_menu_is_active)
			hide_link_creating_menu();
	}
private:
	Label::Ptr make_default_message_box();
	float get_message_box_x_pos(MsgAlign val, float box_size);
	string split_text_if_required(const string& _text);
	void move_messages(float step);	

	
	void hide(bool flag);
	void make_additional_link();

	void send_link();
	void save_link();
	void hide_link_creating_menu();
};
};
#endif