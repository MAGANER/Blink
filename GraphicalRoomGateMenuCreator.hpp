/*
	This class is used to init/delete
	sub menu to enter or start room
*/
#ifndef GRAPHICAL_ROOM_GATE_MENU_CREATOR_HPP
#define GRAPHICAL_ROOM_GATE_MENU_CREATOR_HPP
#include"TGUI/Core.hpp"
#include"TGUI/Widgets/EditBox.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"ConfigLoader.h"
#include"DataBaseProcessor.h"
#include<string>
namespace GraphicalBlink
{
using namespace tgui;
using namespace std;
class RoomGateMenu:public Blink::DataBaseProcessor
{
private:
	EditBox::Ptr room_passw;
	Label::Ptr entering_room_label_ptr, result_label_ptr;
	Button::Ptr enter_room_ptr, start_room_ptr;

	const int conn_room_id = 5;
	const int enter_room_id = 4;

	tgui::String room_password, room_name;
public:
	RoomGateMenu(const string& encr_key,
				 const string& db_name):Blink::DataBaseProcessor(encr_key,db_name)
	{
	}
	~RoomGateMenu(){}
	void init(GuiBase* gui, Blink::ConfigLoader& loader,bool& init_chat)
	{
		auto enter_room_password = EditBox::create();
		enter_room_password->setDefaultText("room password");
		enter_room_password->setSize({ "40.00%", "5.0%" });
		enter_room_password->setPosition({ "30%", "40%" });
		enter_room_password->setUserData(-1);//default id is -1 for this menu
		enter_room_password->setVisible(false);
		enter_room_password->setPasswordCharacter('*');
		room_passw = enter_room_password;
		gui->add(enter_room_password);

		auto entering_room_label = Label::create();
		entering_room_label->setUserData(-1);
		entering_room_label->setPosition({ "30%","35%" });
		entering_room_label->setTextSize(22);
		entering_room_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		entering_room_label->setVisible(false);
		entering_room_label_ptr = entering_room_label;
		gui->add(entering_room_label);

		auto result_label = Label::create();
		result_label->setUserData(-1);
		result_label->setPosition({ "30%","55%" });
		result_label->setTextSize(20);
		result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		result_label_ptr = result_label;
		gui->add(result_label);

		auto enter_room_button = Button::create("enter");
		enter_room_button->setUserData(enter_room_id);
		enter_room_button->setSize({ "18%","5%" });
		enter_room_button->setPosition({ "30%","48%" });
		enter_room_button->setVisible(false);
		enter_room_button->onPress([&]() {can_enter_room(init_chat); });
		enter_room_ptr = enter_room_button;
		gui->add(enter_room_ptr);

		auto start_room_button = Button::create("start");
		start_room_button->setUserData(conn_room_id);
		start_room_button->setSize({ "18%","5%" });
		start_room_button->setPosition({ "52%","48%" });
		start_room_button->setVisible(false);
		start_room_button->onPress([&]() {can_start_room(init_chat); });
		start_room_ptr = start_room_button;
		gui->add(start_room_button);
	}
	void can_enter_room(bool& init_chat)
	{
		bool active_input_box = enter_room_ptr->isVisible();
		if (active_input_box)
		{
			room_password = room_passw->getText();
			bool can_enter = is_password_correct(room_name.toStdString(),
												 room_password.toStdString());

			if (can_enter)
			{
				init_chat = true;
			}
			else
			{
				//show label of disability
				result_label_ptr->setText(":can not enter room!");
			}
		}
	}
	void can_start_room(bool& init_chat)
	{
		bool active_input_box = enter_room_ptr->isVisible();
		if (active_input_box)
		{
			room_password = room_passw->getText();
			bool can_enter = is_password_correct(room_name.toStdString(),
												 room_password.toStdString());

			if (can_enter)
			{
				init_chat = true;
			}
			else
			{
				//show label of disability
				result_label_ptr->setText(":can not start room!");
			}
		}
	}
	void enter_room(ListBox::Ptr rooms_ptr)
	{
		if (rooms_ptr->getSelectedItem() != "")
		{
			room_passw->setVisible(true);
			room_name = rooms_ptr->getSelectedItem();
			entering_room_label_ptr->setText("entering `" + room_name + "` room");
			entering_room_label_ptr->setVisible(true);
			enter_room_ptr->setVisible(true);
			start_room_ptr->setVisible(true);

			rooms_ptr->setSelectedItem("");
		}
	}
};
};
#endif