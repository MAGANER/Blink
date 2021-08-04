#pragma once
#include"SFML/Network.hpp"
#include "Interface.h"
#include"DataBaseProcessor.h"
#include"MessageCreator.h"
#include"InviteLinkCreator.h"
#include"encryption/Encryption.h"
#include<fstream>
namespace Blink
{

    struct ConnectionData
    {
        ConnectionData()
        {
        }
        ~ConnectionData(){}
        string room, password, ip, port;
    };
    struct EncryptionData
    {
        EncryptionData(const string& key,
                       const string& iv)
        {
            data = make_pair(encr::AES::convert_to_bytes(key),
                             encr::AES::convert_to_bytes(iv));
        }
        ~EncryptionData(){}
        encr::AES::key_iv data;
    };

    using namespace sf;
    class MainMenu : public Interface, public DataBaseProcessor
    {
    public:
        enum class CONNECTION_REGIME
        {
            WithLink,
            Manually
        };

        MainMenu(const string& encr_key);
        ~MainMenu()
        {
            if (encr_data != nullptr) delete encr_data;
        }

        bool exit()       { return _exit;          }
        bool enter_room() { return can_enter_room; }
        bool _connect() { return can_connect_to_room; };

        ConnectionData get_connection_data(){ return data; }
        CONNECTION_REGIME get_connection_regime() { return con_regime; }
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
        
        ConnectionData data;
        EncryptionData* encr_data = nullptr;
        CONNECTION_REGIME con_regime;

        void create();
        void enter();
        void connect();
        //void connect_with_link();
        void connect_with_filelink();
        bool can_connect(const string& ip,
                         const string& password,
                         const string& name,
                         unsigned int port);
    };
};

