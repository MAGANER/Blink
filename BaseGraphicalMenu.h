/*
	this class contains and process every menu
*/

#ifndef BASE_GRAPHICAL_MENU_H
#define BASE_GRAPHICAL_MENU_H
#include"TGUI/Core.hpp"
#include"TGUI/Backends/SFML.hpp"
#include<vector>
#include<iostream>
#include"ConfigLoader.h"
namespace GraphicalBlink
{
using namespace sf;
using namespace tgui;
class BaseGraphicalMenu
{
public:
	enum class CurrentMenu
	{
		EnterMenu,
		MainMenu,
		MakeFullscreen,
		BackToWindow,
		Nothing,
	};
private:
	RenderWindow* window;
	sf::Color back_color;
protected:
	GuiSFML* gui;
	bool should_break = false;
	CurrentMenu menu_to_run;
public:
	BaseGraphicalMenu(bool fullscreen);
	virtual ~BaseGraphicalMenu();

	CurrentMenu run(bool& make_fullscreen);
	void updateTextSize();
	void set_background_color(sf::Color& color)
	{
		back_color = color;
	}
};
}
#endif //BASE_GRAPHICAL_MENU_H
