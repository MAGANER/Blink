#include "CreateRoomMenu.h"
using namespace GraphicalBlink;

CreateRoomMenu::CreateRoomMenu(const string& encr_key,
							   const string& db_name):
	DataBaseProcessor(encr_key, db_name)
{

}
CreateRoomMenu::~CreateRoomMenu()
{

}
void CreateRoomMenu::init(tgui::GuiBase* gui, Blink::ConfigLoader& loader, ListBox::Ptr rooms_ptr)
{
	this->gui = gui;
	create(loader);

	this->rooms_ptr = rooms_ptr;
}
void CreateRoomMenu::clear()
{
	gui->remove(password_ptr);
	gui->remove(name_ptr);
	gui->remove(result_ptr);
	gui->remove(create_ptr);
}
void CreateRoomMenu::create(Blink::ConfigLoader& loader)
{
	auto user_name = EditBox::create();
	user_name->setSize({ "40.00%", "5.0%" });
	user_name->setPosition({ "30.00%", "30.67%" });
	user_name->setDefaultText("Room name");
	user_name->setAlignment(EditBox::Alignment((int)loader.get_text_align() - 1));
	user_name->setMaximumCharacters(Blink::MAX_NAME_LEN);
	gui->add(user_name);
	name_ptr = user_name;

	auto password = tgui::EditBox::copy(user_name);
	password->setPosition({ "30.00%", "37.6%" });
	password->setPasswordCharacter('*');
	password->setDefaultText("Room password");
	password->getSharedRenderer()->setDefaultTextColor(loader.get_enter_menu_label_color());
	password->setMaximumCharacters(Blink::MAX_PASSWORD_LEN);
	gui->add(password);
	password_ptr = password;

	auto result_label = tgui::Label::create("");
	result_label->setPosition({ "43.00%","54.00%" });
	result_label->getSharedRenderer()->setTextColor(loader.get_enter_menu_label_color());
	gui->add(result_label);
	result_ptr = result_label;

	auto create = tgui::Button::create(" create ");
	create->setSize({ "18%","5%" });
	create->setPosition({ "43%","45%" });
	create->onClick([&]() {create_room(); });
	create_ptr = create;
	gui->add(create);
}
void CreateRoomMenu::create_room()
{
	cout << "gegegeg!" << endl;
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
		rooms_ptr->addItem(name);
		leave = true;
	}
}