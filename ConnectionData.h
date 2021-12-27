#pragma once
#include<string>
namespace Blink
{
    struct ConnectionData
    {
        ConnectionData()
        {
        }
        ~ConnectionData() {}
        std::string room, password, ip;
        void clear()
        {
            room.clear();
            password.clear();
            ip.clear();
        }
    };
};