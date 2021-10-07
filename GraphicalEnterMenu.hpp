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
public:
	GraphicalEnterMenu(bool fullscreen,
					   const string & encr_key,
					   const string & db_name) :
		DataBaseProcessor(encr_key,db_name),BaseGraphicalMenu(fullscreen)
	{
	}
	~GraphicalEnterMenu() {}

	void create_enter_menu(Blink::ConfigLoader & loader)
		{
			gui->removeAllWidgets();

			auto user_name = EditBox::create();
			user_name->setSize({ "60.00%", "10.0%" });
			user_name->setPosition({ "16.67%", "16.67%" });
			user_name->setDefaultText("Username");
			user_name->setAlignment(EditBox::Alignment((int)loader.get_text_align() - 1));
			user_name->setMaximumCharacters(Blink::MAX_NAME_LEN);
			gui->add(user_name);

			auto password = tgui::EditBox::copy(user_name);
			password->setPosition({ "16.67%", "35.6%" });
			password->setPasswordCharacter('*');
			password->setDefaultText("Password");
			password->getSharedRenderer()->setDefaultTextColor(loader.get_enter_menu_label_color());
			password->setMaximumCharacters(Blink::MAX_PASSWORD_LEN);
			gui->add(password);

			auto label = tgui::Label::create("Who you are?");
			label->setPosition({ "16.67%","10.00%" });
			label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(label);

			auto result_label = tgui::Label::create("");
			result_label->setPosition({ "16.67%","60.00%" });
			result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(result_label);

			auto login_button = tgui::Button::create("Login");
			login_button->setSize({ "26%", "10.0%" });
			login_button->setPosition({ "16.7%", "50%" });
			login_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			gui->add(login_button);


			//set_text doesn't  work, so repeat all code about for New button
			auto new_button = tgui::Button::create("New");
			new_button->setSize({ "26%", "10.0%" });
			new_button->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
			new_button->setPosition({ "50.5%","50%" });
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
	bool __can_login() { return _can_login; }
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
			result->setText(":name is too short/long!");
			//create error window
			ok = false;
		}

		bool password_is_more_or_less = _password.size() < MIN_PASSWORD_LEN ||
										_password.size() > MAX_PASSWORD_LEN;
		if (password_is_more_or_less && ok)
		{
			result->setText(":password is too short/long!");
			//create error window
			ok = false;
		}

		if (!does_user_exist(name) && ok)
		{
			DataBaseProcessor::create_new_user(name, _password);
			result->setText(":user is created!");
		}
		else
		{
			if (ok)result->setText(":can not create user!");
			//create error window
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
			_can_login = true;
			should_break = true;
			menu_to_run = CurrentMenu::MainMenu;
			result->setText("");
		}
		else
		{
			result->setText("can not login!");
		}
	}

};
};
#endif