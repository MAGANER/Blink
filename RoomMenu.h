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

        void print_state();
    public:
        enum class mode { SERVER, CLIENT };

        RoomMenu(const string& encr_key);
        ~RoomMenu()
        {
            if (server != nullptr) delete server;
            if (client != nullptr) delete client;
        }

        void run(mode flag);
        void set_room_data(const string& port,
                           const string& ip,
                           const string& user_name,
                           const string& room_name,
                           const string& room_password);

        bool should_exit() { return exit; }    
    };
};

