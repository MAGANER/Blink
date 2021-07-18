#pragma once
#include"SFML/Network.hpp"
#include "Interface.h"
#include"DataBaseProcessor.h"
#include"MessageCreator.h"
namespace Blink
{
    using namespace sf;
    class MainMenu : public Interface, public DataBaseProcessor
    {
    public:
        MainMenu();
        ~MainMenu(){}

        bool exit()       { return _exit;          }
        bool enter_room() { return can_enter_room; }
        bool _connect() { return can_connect_to_room; };
        string get_room_to_enter() { return room_to_enter; }
        string get_password_to_enter() { return password; };
        string get_room_ip() { return room_ip; };
        string get_port() { return port; }

        void finish()
        {
            can_enter_room      = false;
            can_connect_to_room = false;
            room_to_enter.clear();
            password.clear();
            port.clear();
            room_ip.clear();
        }

    private:
        bool _exit          = false;
        bool can_enter_room = false;
        bool can_connect_to_room = false;
        string room_to_enter;
        string password;
        string port;
        string room_ip;

        void create();
        void enter();
        void connect();
        bool can_connect(const string& ip,
                         const string& password,
                         const string& name,
                         unsigned int port);


    };
};

