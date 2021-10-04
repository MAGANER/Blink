/*
	this class contains and process every menu
*/

#ifndef BASE_GRAPHICAL_MENU_H
#define BASE_GRAPHICAL_MENU_H
#include"TGUI/Core.hpp"
#include"TGUI/Backends/SFML.hpp"
#include<vector>
namespace GraphicalBlink
{
using namespace sf;
using namespace tgui;
class BaseGraphicalMenu
{
private:
	RenderWindow* window;
	GuiSFML* gui;

	sf::Color back_color;
public:
	BaseGraphicalMenu();
	~BaseGraphicalMenu();

	void run();
	GuiSFML* get_gui() { return gui; }
	void updateTextSize();
	void set_background_color(sf::Color& color)
	{
		back_color = color;
	}

};
}
#endif //BASE_GRAPHICAL_MENU_H
