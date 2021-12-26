#ifndef GRAPHICAL_ENTER_MENU_H
#define GRAPHICAL_ENTER_MENU_H
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#include"TGUI/Widgets/EditBox.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"Constants.hpp"
#include"DataBaseProcessor.h"
namespace GraphicalBlink
{
using namespace Blink;
class GraphicalEnterMenu :public DataBaseProcessor,
						  public BaseGraphicalMenu
{
private:
	bool _can_login = false;
	string name, password;

	EditBox::Ptr name_ptr, password_ptr;
public:
	GraphicalEnterMenu(bool fullscreen,
					   const string & encr_key,
					   const string & db_name,
					   const sf::Vector2u& win_size) :
		DataBaseProcessor(encr_key,db_name),BaseGraphicalMenu(fullscreen,win_size)
	{
	}
	~GraphicalEnterMenu() {}

	void create(Blink::ConfigLoader & loader)
		{
			updateTextSize();
			gui->removeAllWidgets();

			auto user_name = EditBox::create();
			user_name->setSize({ "60.00%", "6.0%" });
			user_name->setPosition({ "16.67%", "26.67%" });
			user_name->setDefaultText("Username");
			user_name->setAlignment(EditBox::Alignment((int)loader.get_text_align() - 1));
			user_name->setMaximumCharacters(Blink::MAX_NAME_LEN);
			user_name->getSharedRenderer()->setBackgroundColor(loader.get_input_field_background_color());
			user_name->getSharedRenderer()->setBackgroundColorHover(loader.get_input_field_hover_color());
			user_name->getSharedRenderer()->setTextColor(loader.get_input_field_text_color());
			user_name->getSharedRenderer()->setSelectedTextBackgroundColor(loader.get_selection_color());
			user_name->getSharedRenderer()->setSelectedTextColor(loader.get_selected_text_color());
			name_ptr = user_name;
			gui->add(user_name);

			auto password = tgui::EditBox::copy(user_name);
			password->setPosition({ "16.67%", "35.6%" });
			password->setPasswordCharacter('*');
			password->setDefaultText("Password");
			password->getSharedRenderer()->setDefaultTextColor(loader.get_enter_menu_label_color());
			password->setMaximumCharacters(Blink::MAX_PASSWORD_LEN);
			password_ptr = password;
			gui->add(password);

			auto label = tgui::Label::create("Who you are?");
			label->setPosition({ "16.67%","20.00%" });
			label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(label);

			auto result_label = tgui::Label::create("");
			result_label->setPosition({ "16.67%","54.00%" });
			result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(result_label);

			auto login_button = tgui::Button::create("Login");
			login_button->setSize({ "26%", "7.0%" });
			login_button->setPosition({ "16.7%", "45%" });
			login_button->getSharedRenderer()->setBackgroundColor(loader.get_input_field_background_color());
			login_button->getSharedRenderer()->setBackgroundColorHover(loader.get_input_field_hover_color());
			login_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(login_button);


			//set_text doesn't  work, so repeat all code about for New button
			auto new_button = tgui::Button::create("New");
			new_button->setSize({ "26%", "7.0%" });
			new_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			new_button->setPosition({ "50.5%","45%" });
			gui->add(new_button);


			auto create_lambda = [&](EditBox::Ptr username,
									 EditBox::Ptr password,
									 Label::Ptr result)
			{
				create_new_user(username, password, result);
			};
			new_button->onPress(create_lambda ,user_name,password,result_label);
			

			auto login_lambda = [&](EditBox::Ptr username,
									EditBox::Ptr password,
									Label::Ptr result)
			{
				login(username, password, result);
			};
			login_button->onPress(login_lambda, user_name, password, result_label);

		}
	string get_user_name() { return name; }
	string get_user_password() { return password; }
private:

	void create_new_user(EditBox::Ptr username,
						 EditBox::Ptr password,
						 Label::Ptr result)
	{
		string name = username->getText().toStdString();
		string _password = password->getText().toStdString();

		bool ok = true;

		bool name_is_more_or_less = name.size() < MIN_NAME_LEN ||
									name.size() > MAX_NAME_LEN;
		if (name_is_more_or_less && ok)
		{
			name_ptr->setText("");
			password_ptr->setText("");
			result->setText(":name is too short/long!");
			//create error window
			ok = false;
		}

		bool password_is_more_or_less = _password.size() < MIN_PASSWORD_LEN ||
										_password.size() > MAX_PASSWORD_LEN;
		if (password_is_more_or_less && ok)
		{
			name_ptr->setText("");
			password_ptr->setText("");
			result->setText(":password is too short/long!");
			//create error window
			ok = false;
		}

		if (!does_user_exist(name) && ok)
		{
			name_ptr->setText("");
			password_ptr->setText("");
			DataBaseProcessor::create_new_user(name, _password);
			result->setText(":user is created!");

			this->name = name;
			this->password = _password;
			menu_to_return = CurrentMenu::MainMenu;
		}
		else
		{
			name_ptr->setText("");
			password_ptr->setText("");
			if (ok)result->setText(":can not create user!");
		}
	}
	void login(EditBox::Ptr username,
			   EditBox::Ptr password,
		 	   Label::Ptr result)
	{
		auto name = username->getText().toStdString();
		auto passw = password->getText().toStdString();

		if (can_login(name, passw))
		{
			this->name = name;
			this->password = passw;
			result->setText("");

			menu_to_return = CurrentMenu::MainMenu;
		}
		else
		{
			name_ptr->setText("");
			password_ptr->setText("");
			result->setText("can not login!");
		}
	}

};
};
#endif