#include "BaseGraphicalMenu.h"
using namespace GraphicalBlink;

BaseGraphicalMenu::BaseGraphicalMenu()
{
	window = new RenderWindow(VideoMode(720, 640), "Blink");
	gui = new GuiSFML(*window);
}
BaseGraphicalMenu::~BaseGraphicalMenu()
{
	delete window;
	delete gui;
}

void BaseGraphicalMenu::run()
{
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            gui->handleEvent(event);

            if (event.type == sf::Event::Closed)
                window->close();
        }

        window->clear(back_color);
        gui->draw();
        window->display();
    }
}
void BaseGraphicalMenu::updateTextSize()
{
    const float windowHeight = gui->getView().getRect().height;
    gui->setTextSize(static_cast<unsigned int>(0.04f * windowHeight));
}