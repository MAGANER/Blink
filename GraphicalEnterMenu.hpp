#ifndef GRAPHICAL_ENTER_MENU_H
#define GRAPHICAL_ENTER_MENU_H
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#include"TGUI/Widgets/EditBox.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"Constants.hpp"
namespace GraphicalBlink
{
namespace
{
	void create_enter_menu(GuiSFML& gui,Blink::ConfigLoader& loader)
	{
		gui.removeAllWidgets();

		auto user_name = EditBox::create();
		user_name->setSize({ "60.00%", "10.0%" });
		user_name->setPosition({ "16.67%", "16.67%" });
		user_name->setDefaultText("Username");
		user_name->setAlignment(EditBox::Alignment((int)loader.get_text_align() - 1));
		user_name->setMaximumCharacters(Blink::MAX_NAME_LEN);
		gui.add(user_name);

		auto password = tgui::EditBox::copy(user_name);
		password->setPosition({ "16.67%", "35.6%" });
		password->setPasswordCharacter('*');
		password->setDefaultText("Password");
		password->getSharedRenderer()->setDefaultTextColor(loader.get_enter_menu_label_color());
		password->setMaximumCharacters(Blink::MAX_PASSWORD_LEN);
		gui.add(password);

		auto label = tgui::Label::create("Who you are?");
		label->setPosition({"34.67%","10.00%"});
		label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		gui.add(label);

		auto login_button = tgui::Button::create("Login");
		login_button->setSize({ "26%", "10.0%" });
		login_button->setPosition({ "16.7%", "50%" });
		login_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		gui.add(login_button);


		//set_text doesn't  work
		auto new_button = tgui::Button::create("New");
		new_button->setSize({ "26%", "10.0%" });
		new_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
		new_button->setPosition({ "50.5%","50%" });
		gui.add(new_button);

	}
}
};
#endif

