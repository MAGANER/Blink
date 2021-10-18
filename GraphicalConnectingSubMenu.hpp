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
	Blink::EncryptionData data;
	bool can_connect_flag = false;

	Label::Ptr result_label_ptr;
public:
	GraphicalConnectingSubMenu()
	{
	}
	~GraphicalConnectingSubMenu()
	{
	}
	void init_menu(GuiBase* gui, Blink::ConfigLoader& loader)
	{
		auto inv_link = EditBox::create();
		inv_link->setDefaultText("inviting link path");
		inv_link->setSize({ "40.00%", "5.0%" });
		inv_link->setPosition({ "30%", "40%" });
		inv_link->setUserData(-1);//default id is -1 for this menu
		inv_link->setPasswordCharacter('*');
		gui->add(inv_link);

		auto result_label = Label::create();
		result_label->setUserData(-1);
		result_label->setPosition({ "30%","55%" });
		result_label->setTextSize(20);
		result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		result_label_ptr = result_label;
		gui->add(result_label);

		auto conn_button = Button::create("connect");
		conn_button->setUserData(-1);
		conn_button->setSize({ "18%","5%" });
		conn_button->setPosition({ "40%","48%" });
		conn_button->onPress([&]() {check_can_connect(); });
		gui->add(conn_button);
	}
	bool can_connect() { return can_connect_flag; }
	Blink::EncryptionData& get_encrpyption_data()
	{
		return data;
	}

	void check_can_connect()
	{

	}
};
};
#endif