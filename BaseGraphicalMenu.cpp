#include "BaseGraphicalMenu.h"
using namespace GraphicalBlink;

BaseGraphicalMenu::BaseGraphicalMenu(bool fullscreen)
{
    auto style = fullscreen ?sf::Style::Fullscreen : sf::Style::Default;
    window = new RenderWindow(VideoMode(720, 640), "Blink", style);
	gui = new GuiSFML(*window);
}
BaseGraphicalMenu::~BaseGraphicalMenu()
{
	delete window;
	delete gui;
}

BaseGraphicalMenu::CurrentMenu BaseGraphicalMenu::run(bool& make_fullscreen)
{
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            gui->handleEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window->close();
                delete window;
                delete gui;
                exit(0);
            }
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == 6)//ctrl+f
                {
                    if (!make_fullscreen)
                    {
                        make_fullscreen = true;
                        return CurrentMenu::MakeFullscreen;
                    }
                    else
                    {
                        make_fullscreen = false;
                        return CurrentMenu::BackToWindow;
                    }
                }
            }

        }

        window->clear(back_color);
        gui->draw();
        window->display();

        if (should_break)
        {
            window->close();
            should_break = false;
            return menu_to_run;
        }
    }
    return CurrentMenu::Nothing;
}
void BaseGraphicalMenu::updateTextSize()
{
    const float windowHeight = gui->getView().getRect().height;
    gui->setTextSize(static_cast<unsigned int>(0.04f * windowHeight));
}