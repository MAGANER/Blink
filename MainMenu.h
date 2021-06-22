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

        bool exit()       { return _exit;          }
        bool enter_room() { return can_enter_room; }
        string get_room_to_enter() { return room_to_enter; }
        string get_password_to_enter() { return password; };
    private:
        bool _exit = false;
        bool can_enter_room = false;
        string room_to_enter;
        string password;

        void create();
        void enter();
        void connect();
    };
};

