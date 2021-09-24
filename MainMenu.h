/*
    Entry point to connect or enter room.
*/
#pragma once
#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include"SFML/Network.hpp"
#include "Interface.h"
#include"DataBaseProcessor.h"
#include"InviteLinkCreator.h"
#include"EncryptionData.h"
#include"ConnectionChecker.hpp"
#include<fstream>
namespace Blink
{
    using namespace sf;
    class MainMenu : public Interface, public DataBaseProcessor
    {
    public:
        MainMenu(const string& encr_key,
                 const string& db_name);
        ~MainMenu()
        {
            if (encr_data != nullptr) delete encr_data;
        }

        bool exit()       { return _exit;          }
        bool enter_room() { return can_enter_room; }
        bool _connect() { return can_connect_to_room; }
        bool is_decentralysed() { return decentralysed; }
        bool is_connecting_with_conflink_command() { return connecting_with_conflink_command; }

        ConnectionData get_connection_data(){ return data; }
        EncryptionData* get_encryption_data() { return encr_data; }
        void finish()
        {
            can_enter_room      = false;
            can_connect_to_room = false;

            data.ip.clear();
            data.port.clear();
            data.password.clear();
            data.room.clear();
        }

    private:
        bool _exit          = false;
        bool can_enter_room = false;
        bool can_connect_to_room = false;
        bool decentralysed = false;

        //if u use conflink command, then next special
        //port will be generated. I mean port for your server
        //If you enter rooms, then old port is used
        bool connecting_with_conflink_command = false;
        
        ConnectionData data;
        EncryptionData* encr_data = nullptr;

        void create();
        void enter();
        void show_rooms();
    };
};

