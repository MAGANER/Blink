/*
	This class reads common config from data directory.
	This .json file contains data about style.
	Also about console or window mode.
*/

#ifndef CONFIG_LOADER
#define CONFIG_LOADER
#include"json.hpp"
#include"SFML/System/Vector2.hpp"
#include"SFML/Graphics/Color.hpp"
#include<fstream>
#include<string>
#include<iostream>
namespace Blink
{
using namespace nlohmann;
using namespace std;
using namespace sf;
class ConfigLoader
{
public:
	enum class TextAlignment
	{
		Left = 1,
		Center,
		Right
	};
private:
	json config, theme;

	//default parameters
	bool terminal_mode = false;
	int height = 640;
	int width  = 720;

	bool theme_is_loaded = false;
	Color background_win_color, enter_menu_label_color, message_background;
	TextAlignment default_text_align = TextAlignment::Center;
	bool fullscreen = false;
public:
	ConfigLoader();
	~ConfigLoader();

	auto get_background_win_color()  { return background_win_color; }
	auto get_enter_menu_label_color(){ return enter_menu_label_color; }
	auto get_message_background_color() { return message_background; }
	auto get_text_align() { return default_text_align; }
	bool is_theme_loaded() { return theme_is_loaded; }
	bool get_mode() { return terminal_mode; }
	auto get_win_size() { return sf::Vector2u(width, height); }
	bool is_fullscreen() { return fullscreen; }
private:
	void load_color(Color& color,const string& field_name);
};
};
#endif 

