#pragma once
#include"Server.h"
#include"Client.h"
#include "Interface.h"
#include"DataBaseProcessor.h"
#include<future>

namespace Blink
{
    class RoomMenu : public Interface, public DataBaseProcessor
    {
    private:
        bool exit = false;
        map<string, string> data;

        Server* server = nullptr;
        Client* client = nullptr;
    public:
        enum class mode { SERVER, CLIENT };

        RoomMenu();
        ~RoomMenu()
        {
            if (server != nullptr) delete server;
            if (client != nullptr) delete client;
        }

        void run(mode flag);
        void set_room_data(const string& port,
                           const string& ip,
                           const string& user_name);

        bool should_exit() { return exit; }    
    };
};

