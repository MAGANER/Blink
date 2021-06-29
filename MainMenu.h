#pragma once
#include "Interface.h"
#include"DataBaseProcessor.h"
#include "SFML/Network.hpp"
#include"json.hpp"
using namespace sf;
using namespace nlohmann;

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
        string get_port() { return port; }

        void finish()
        {
            can_enter_room = false;
            room_to_enter.clear();
            password.clear();
            port.clear();
        }

    private:
        bool _exit          = false;
        bool can_enter_room = false;
        string room_to_enter;
        string password;
        string port;

        void create();
        void enter();
        void connect();
        bool can_connect(const string& ip,
                         const string& password,
                         const string& name,
                         unsigned int port);


    };
};

