/*
	This menu is used to enter/start room.
	If user has acess to room, then everything is deleted.
	If user start room, then before delete everything,
	he/she must create/send invite link.
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
#include<utility>
namespace GraphicalBlink
{
using namespace tgui;
using namespace std;
class RoomGateMenu:public Blink::DataBaseProcessor
{
private:
	EditBox::Ptr room_passw, recepient_name;
	Label::Ptr entering_room_label_ptr, result_label_ptr;
	Button::Ptr enter_room_ptr;

	const int conn_room_id = 5;
	const int enter_room_id = 4;

	tgui::String room_password, room_name;

	bool save_link = true;//else send
	string link_creator_additional_data;//recepient name/path to save

	bool starting_room = false;

public:
	RoomGateMenu(const string& encr_key,
				 const string& db_name):Blink::DataBaseProcessor(encr_key,db_name)
	{
	}
	~RoomGateMenu(){}
	void init(GuiBase* gui, Blink::ConfigLoader& loader,bool& init_chat,bool& start)
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


		auto recepient_name_box = EditBox::create();
		recepient_name_box->setDefaultText("recepient name");
		recepient_name_box->setSize({ "40%","5.0%" });
		recepient_name_box->setUserData(-1);
		recepient_name_box->setVisible(false);
		recepient_name = recepient_name_box;
		gui->add(recepient_name_box);

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
		result_label->setVisible(false);
		result_label->getSharedRenderer()->setBackgroundColor(tgui::Color::Transparent);
		result_label_ptr = result_label;
		gui->add(result_label);

		auto enter_room_button = Button::create("enter");
		enter_room_button->setUserData(enter_room_id);
		enter_room_button->setSize({ "18%","5%" });
		enter_room_button->setPosition({ "40%","48%" });
		enter_room_button->setVisible(false);
		enter_room_button->onPress([&]() {can_enter_room(init_chat); });
		enter_room_ptr = enter_room_button;
		gui->add(enter_room_ptr);
	}
	void can_enter_room(bool& init_chat)
	{
		bool active_input_box = enter_room_ptr->isVisible();
		if (active_input_box)
		{
			room_password = room_passw->getText().toStdString();
			cout << room_name.toStdString() << ":" << room_password.toStdString() << endl;
			bool can_enter = is_password_correct(room_name.toStdString(),
												 room_password.toStdString());

			if (can_enter)
			{
				cout << "can enter:"<< is_room_started(room_name.toStdString()) << endl;
				if (!is_room_started(room_name.toStdString()))
				{
					cout << "wtf!" << endl;
					starting_room = true;
				}
				init_chat = true;
			}
			else
			{
				//show label of disability
				result_label_ptr->setVisible(true);
				result_label_ptr->setText(":can not enter room!");
				room_passw->setText("");
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

			rooms_ptr->setSelectedItem("");
		}
	}

	bool _save_link() { return save_link; }
	bool is_starting_room() { return starting_room; }
	void stop_starting() { starting_room = false; }

	pair<string, string> get_room_name_password()
	{
		return make_pair(room_name.toStdString(), room_password.toStdString());
	}

	void clear(GuiBase* gui)
	{
		room_passw->setVisible(false);
		recepient_name->setVisible(false);
		entering_room_label_ptr->setVisible(false);
		result_label_ptr->setVisible(false);
		enter_room_ptr->setVisible(false);

	}
};
};
#endif