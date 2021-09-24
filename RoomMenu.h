/*
    Menu can be running in server or client mode.
*/

#pragma once
#include"Server.h"
#include "Interface.h"
#include"DataBaseProcessor.h"
#include"DecentralysedServerClient.h"

namespace Blink
{
    class RoomMenu : public Interface, public DataBaseProcessor
    {
    private:
        bool exit = false;
        bool decentralysed = false;

        map<string, string> data;

        Server* server = nullptr;

        void print_state();
        void list_all_messages();

        string db_key;
    public:
        RoomMenu(const string& encr_key,
                 const string& db_name);
        ~RoomMenu()
        {
            if (server != nullptr) delete server;
        }

        void run(bool connecting_with_file_link, bool starting_room);
        void run(const encr::AES::key_iv& key, bool connecting_with_file_link, bool starting_room);
        void set_room_data(const string& port,
                           const string& ip,
                           const string& user_name,
                           const string& room_name,
                           const string& room_password);

        bool should_exit() { return exit; }   
    };
};

