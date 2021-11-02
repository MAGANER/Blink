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
	bool should_return_to_prev_menu = false;
	bool back = false;

	Label::Ptr result_label_ptr;
	EditBox::Ptr inv_link_ptr;

public:
	GraphicalConnectingSubMenu()
	{
	}
	~GraphicalConnectingSubMenu()
	{
	}
	void init_menu(GuiBase* gui, Blink::ConfigLoader& loader,bool& init_chat)
	{
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
		gui->add(conn_button);


		auto back_button = Button::create(" back ");
		back_button->setUserData(-1);
		back_button->setSize({ "18%","5%" });
		back_button->setPosition({ "40%","55%" });
		back_button->onPress([&]() {back = true; });
		gui->add(back_button);
	}
	bool can_connect() { return can_connect_flag; }
	bool return_to_prev_menu() { return should_return_to_prev_menu; }
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

		if (ConnectionChecker::connect_with_filelink(conn_data, encr_data, path))
		{
			can_connect_flag = true;
			init_chat = true;
			result_label_ptr->setText("link is accepted! will be connected in 2 seconds!");
		}
		else
		{
			should_return_to_prev_menu = true;
			result_label_ptr->setText("link is denyied! will be leaving in 2 seconds!");
		}
	}
};
};
#endif