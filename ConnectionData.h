#pragma once
#include<string>
#include"RoomNetworkMode.h"
namespace Blink
{
    struct ConnectionData
    {
        ConnectionData()
        {
        }
        ~ConnectionData() {}
        std::string room, password, ip, port;
        void clear()
        {
            room.clear();
            password.clear();
            ip.clear();
            port.clear();
        }
    };
};