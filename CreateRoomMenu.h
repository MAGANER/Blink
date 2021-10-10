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
#include"NetRandom.hpp"
namespace GraphicalBlink
{
using namespace Blink;
class CreateRoomMenu: public BaseGraphicalMenu,
					  public DataBaseProcessor
{
private:
	EditBox::Ptr name_ptr, password_ptr;
	Label::Ptr result_ptr;

	void create_room();

	bool enter_pressed = false;
	bool update = false;
public:
	CreateRoomMenu(const string& encr_key,
				   const string& db_name);
	~CreateRoomMenu();

	void create(Blink::ConfigLoader& loader);
	bool should_update() { return update; }
};
};
#endif
