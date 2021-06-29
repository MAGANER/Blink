#pragma once
#include "Interface.h"
#include "DataBaseProcessor.h"
#include "SFML/Network.hpp"
#include"json.hpp"
using namespace sf;
using namespace nlohmann;
namespace Blink
{
    class RoomMenu : public Interface, public DataBaseProcessor
    {
    private:
        TcpListener* listner          = nullptr;
        TcpSocket* current_connection = nullptr;
        
        TcpSocket* socket    = nullptr;

        bool started = false;
        bool exit    = false;

        map<string, string> data;
    public:
        RoomMenu();
        ~RoomMenu()
        {
            disconnect();
        }

        void set_room_data(const string& name,
                           const string& password,
                           const string& port);

        void start_as_master();
        void start_as_slave();
        bool is_started() { return started; }
        bool should_exit() { return exit; }

        void process_as_master(unsigned int port);

        void disconnect()
        {
            if (listner != nullptr) delete listner;
            if (socket != nullptr) delete socket;
            if (current_connection != nullptr) delete current_connection;
            data.clear();

            started = false;
        }
        void run();
    private:
        void print_state();
        void send_message(const string& message, TcpSocket* target);

        AsyncInput sending_input;
        bool read_sending_input = false;
        void send();
    };
};

