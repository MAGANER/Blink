#include "CreateRoomMenu.h"
using namespace GraphicalBlink;

CreateRoomMenu::CreateRoomMenu(const string& encr_key,
							   const string& db_name) :
	BaseGraphicalMenu(false, sf::Vector2u(320, 140)),
	DataBaseProcessor(encr_key, db_name)
{

}
CreateRoomMenu::~CreateRoomMenu()
{

}
void CreateRoomMenu::create(Blink::ConfigLoader& loader)
{
	auto user_name = EditBox::create();
	user_name->setSize({ "80.00%", "20.0%" });
	user_name->setPosition({ "10.00%", "16.67%" });
	user_name->setDefaultText("Room name");
	user_name->setAlignment(EditBox::Alignment((int)loader.get_text_align() - 1));
	user_name->setMaximumCharacters(Blink::MAX_NAME_LEN);
	gui->add(user_name);
	name_ptr = user_name;

	auto password = tgui::EditBox::copy(user_name);
	password->setPosition({ "10.00%", "35.6%" });
	password->setPasswordCharacter('*');
	password->setDefaultText("Room password");
	password->getSharedRenderer()->setDefaultTextColor(loader.get_enter_menu_label_color());
	password->setMaximumCharacters(Blink::MAX_PASSWORD_LEN);
	gui->add(password);
	password_ptr = password;

	auto result_label = tgui::Label::create("");
	result_label->setPosition({ "10.00%","54.00%" });
	result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
	gui->add(result_label);
	result_ptr = result_label;

	auto fn = function<void()>([&] { create_room(); });
	paramless_echo_functions.push_back(fn);
}
void CreateRoomMenu::create_room()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !enter_pressed)
	{
		auto name  = name_ptr->getText().toStdString();
		auto passw = password_ptr->getText().toStdString();

		if (name.size() < MIN_NAME_LEN)
		{
			result_ptr->setText("name is too short!");
			return;
		}
		if (passw.size() < MIN_PASSWORD_LEN)
		{
			result_ptr->setText("password is too short!");
			return;
		}

		if (does_room_exists(name))
		{
			result_ptr->setText("can not create room!");
			return;
		}
		else
		{
			string port = to_string(NetRandom::get_random_port());
			create_new_room(name, passw, port);
			should_break = true;
			update = true;
		}

		enter_pressed = true;
	}

	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		enter_pressed = false;
	}
}