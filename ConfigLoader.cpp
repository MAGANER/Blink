#include "ConfigLoader.h"
using namespace Blink;

ConfigLoader::ConfigLoader()
{
	fstream file("data/config.json");
	bool can_load = !!file;
	if(can_load)file >> config;
	file.close();

	if (can_load)
	{
		width  = config["width"];
		height = config["height"];
		terminal_mode = config["mode"];
		fullscreen = config["fullscreen"];
		public_connection = config["net"];

		string theme_path = config["theme"];
		file.open("data/" + theme_path);
		can_load = !!file;
		if (can_load)
		{
			theme_is_loaded = true;
			file >> theme;
		}
		else theme_is_loaded = false;
		file.close();
	}

	if (theme_is_loaded)
	{
		load_color(background_win_color, "background_win_color");
		load_color(enter_menu_label_color, "enter_menu_label_color");
		load_color(message_background, "message_background_color");
		load_color(input_field_background_color, "input_background_color");
		load_color(input_field_hover_color, "input_hover_back_color");
		load_color(input_field_text_color, "input_field_text_color");
		load_color(selected_text_color, "selected_text_color");
		load_color(selection_color, "selection_color");
		int text_align = theme["default_text_align"];
		default_text_align = TextAlignment(text_align);


	}

}
ConfigLoader::~ConfigLoader()
{

}
void ConfigLoader::load_color(Color& color, const string& field_name)
{
	int r = theme[field_name][0];
	int g = theme[field_name][1];
	int b = theme[field_name][2];
	int a = theme[field_name][3];
	color = Color(r, g, b, a);
}