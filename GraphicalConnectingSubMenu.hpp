#ifndef GRAPHICAL_CONNECTING_SUB_MENU_H
#define GRAPHICAL_CONNECTING_SUB_MENU_H
#include"ConnectionChecker.hpp"
#include"EncryptionData.h"
#include"TGUI/Core.hpp"
#include"TGUI/Widgets/EditBox.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"ConfigLoader.h"
namespace GraphicalBlink
{
using namespace tgui;
class GraphicalConnectingSubMenu
{
private:
	Blink::EncryptionData encr_data;
	Blink::ConnectionData conn_data;

	bool can_connect_flag = false;
	bool back = false;


	/*
	   if this label was visible, so it stay the same
	   after this menu isn't used anymore.
	   Anyway, when this menu is active 
	   label must be not visible
	*/
	bool no_rooms_label_ptr_was_visible = true;
	Label::Ptr result_label_ptr, no_rooms_label_ptr;
	EditBox::Ptr inv_link_ptr;
	Button::Ptr conn_button_ptr;

	string user_name;
public:
	GraphicalConnectingSubMenu()
	{
	}
	~GraphicalConnectingSubMenu()
	{
	}
	void clear(GuiBase* gui)
	{
		gui->remove(result_label_ptr);
		gui->remove(inv_link_ptr);
		gui->remove(conn_button_ptr);
		if (no_rooms_label_ptr_was_visible)
			no_rooms_label_ptr->setVisible(true);
	}
	void init_menu(GuiBase* gui, 
				   Blink::ConfigLoader& loader,
				   bool& init_chat,
				   const string& user_name,
				   Label::Ptr no_rooms_label_ptr)
	{
		this->no_rooms_label_ptr = no_rooms_label_ptr;
		no_rooms_label_ptr_was_visible = no_rooms_label_ptr->isVisible();
		no_rooms_label_ptr->setVisible(false);
		this->user_name = user_name;

		auto inv_link = EditBox::create();
		inv_link->setDefaultText("inviting link path");
		inv_link->setSize({ "40.00%", "5.0%" });
		inv_link->setPosition({ "30%", "40%" });
		inv_link->setUserData(-1);//default id is -1 for this menu
		gui->add(inv_link);
		inv_link_ptr = inv_link;

		auto result_label = Label::create();
		result_label->setUserData(-1);
		result_label->setPosition({ "25%","74%" });
		result_label->setTextSize(20);
		result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		result_label_ptr = result_label;
		gui->add(result_label);

		auto conn_button = Button::create("connect");
		conn_button->setUserData(-1);
		conn_button->setSize({ "18%","5%" });
		conn_button->setPosition({ "40%","48%" });
		conn_button->onPress([&]() {check_can_connect(init_chat); });
		conn_button_ptr = conn_button;
		gui->add(conn_button);
	}
	bool can_connect() { return can_connect_flag; }
	bool get_back() { return back; }
	Blink::EncryptionData& get_encrpyption_data()
	{
		return encr_data;
	}
	Blink::ConnectionData& get_connection_data()
	{
		return conn_data;
	}
	void check_can_connect(bool& init_chat)
	{
		string path = inv_link_ptr->getText().toStdString();
		if (ConnectionChecker::connect_with_filelink(conn_data, encr_data, path,user_name))
		{
			can_connect_flag = true;
			init_chat = true;
			result_label_ptr->setText("link is accepted! will be connected in 2 seconds!");
		}
		else
		{
			back = true;
			result_label_ptr->setText("link is denyied! will be leaving in 2 seconds!");
		}
	}
};
};
#endif