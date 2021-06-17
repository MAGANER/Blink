#pragma once
#include "Interface.h"
#include"DataBaseProcessor.h"

namespace Blink
{
    class MainMenu : public Interface, public DataBaseProcessor
    {
    public:
        MainMenu();
        ~MainMenu(){}

        bool exit() { return _exit; }
    private:
        bool _exit = false;

        void create();
        void enter();
        void connect();
    };
};

