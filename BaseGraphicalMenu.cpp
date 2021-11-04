#include "BaseGraphicalMenu.h"
using namespace GraphicalBlink;

BaseGraphicalMenu::BaseGraphicalMenu(bool fullscreen, const sf::Vector2u& win_size)
{
    auto style = fullscreen ?sf::Style::Fullscreen : sf::Style::Default;
    window = new RenderWindow(VideoMode(win_size.x, win_size.y), "Blink", style);
    window->setPosition(sf::Vector2i(window->getPosition().x, 10));
    window->setFramerateLimit(30);
    start_size = window->getSize();
	gui = new GuiSFML(*window);
}
BaseGraphicalMenu::~BaseGraphicalMenu()
{
	delete window;
	delete gui;
}

BaseGraphicalMenu::CurrentMenu BaseGraphicalMenu::run(bool& make_fullscreen, sf::Vector2u& win_size)
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
            if (event.type == sf::Event::Resized)
            {
                win_size = window->getSize();
            }

            if (process_mouse_wheel)
            {
                if (event.type == sf::Event::MouseWheelMoved)
                {
                    int direction = event.mouseWheel.delta;
                    if(direction != 0)echo_mouse_wheel_function(direction);
                }
                
            }

            //process child event functions
            for (auto& fn : echo_functions)
            {
                fn(event.type);
            }
        }

        if (should_run_paramless_echo_function)
            for (auto& fn : paramless_echo_functions)
            {
                fn();
            }
        else
        {
            if(!paramless_echo_functions.empty())
                paramless_echo_functions.clear();

            main_echo_function();
        }

        window->clear(back_color);
        gui->draw();
        window->display();

        if (should_break)
        {
            window->close();
            return menu_to_run;
        }

        if (menu_to_return != CurrentMenu::Nothing)
        {
            should_break = true;
            window->close();
            menu_to_run = menu_to_return;
            menu_to_return = CurrentMenu::Nothing;
            return menu_to_run;
        }
    }


    return CurrentMenu::Nothing;
}
void BaseGraphicalMenu::run()
{
    while (window->isOpen())
    {
        for (auto& fn : paramless_echo_functions)
        {
            fn();
        }

        sf::Event event;
        while (window->pollEvent(event))
        {
            gui->handleEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window->close();
            }

            //process child event functions
            for (auto& fn : echo_functions)
            {
                fn(event.type);
            }
        }

        update_win();

        if (should_break)
        {
            window->close();
        }
    }
}
void BaseGraphicalMenu::update_win()
{
    window->clear(back_color);
    gui->draw();
    window->display();
}
void BaseGraphicalMenu::updateTextSize()
{
    const float windowHeight = gui->getView().getRect().height;
    gui->setTextSize(static_cast<unsigned int>(0.04f * windowHeight));
}
void BaseGraphicalMenu::set_fullscreen(bool val, Blink::ConfigLoader& loader)
{
    delete gui;
    auto curr = window->getSize();
    if (val)
    {
        window->create(VideoMode(curr.x,curr.y), "Blink", Style::Fullscreen);
    }
    else
    {
        window->create(VideoMode(start_size.x, start_size.y), "Blink", Style::Default);
    }
    gui = new GuiSFML(*window);
    create(loader);
}
void BaseGraphicalMenu::prepare_menu(Blink::ConfigLoader& loader)
{
    if (loader.is_theme_loaded())
    {
        auto color = loader.get_background_win_color();
        set_background_color(color);
    }
}