#include"EnterMenu.h"
using namespace Blink;
int main()
{
	create_database(); //if it doesn't exist

	EnterMenu enter_menu;
	enter_menu.run();

	return 0;
}