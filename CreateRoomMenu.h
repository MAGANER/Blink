#ifndef CREATE_ROOM_MENU_H
#define CREATE_ROOM_MENU_H
#ifndef COMMON_HEADERS_H
#define COMMON_HEADERS_H
#include"DataBaseProcessor.h"
#include"BaseGraphicalMenu.h"
#include"ConfigLoader.h"
#endif
#include"Constants.hpp"
#include"TGUI/Widgets/EditBox.hpp"
#include"TGUI/Widgets/Label.hpp"
#include"TGUI/Widgets/Button.hpp"
#include"TGUI/Widgets/ListBox.hpp"
#include"NetRandom.hpp"
namespace GraphicalBlink
{
using namespace Blink;
class CreateRoomMenu: public DataBaseProcessor
{
private:
	EditBox::Ptr name_ptr, password_ptr;
	Label::Ptr result_ptr, no_rooms_ptr;
	Button::Ptr create_ptr;

	ListBox::Ptr rooms_ptr;

	void create_room();
	tgui::GuiBase* gui = nullptr;

	void create(Blink::ConfigLoader& loader);

	bool leave = false;
	string room_name;
public:
	CreateRoomMenu(const string& encr_key,
				   const string& db_name);
	~CreateRoomMenu();

	void clear();
	bool can_leave() { return leave; }

	void init(tgui::GuiBase* gui, 
			  Blink::ConfigLoader& loader);

	string get_room_name() { return room_name; }
};
};
#endif
