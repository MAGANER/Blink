#ifndef CLIENT_H
#define CLIENT_H
#include<string>
#include<iostream>
#include<future>
#include"json.hpp"
#include"getIP.h"
#include"NetBase.h"
#include"Interface.h" // process_input

namespace Blink
{
	using namespace std;
	using namespace sf;
	using namespace nlohmann;

	class Client:public NetBase
	{
	private:
		int port;
		string user_name;

		TcpSocket sender;
	public:
		Client(const string& ip,
			   int port,
			   const string& channel_name,
			   const string& user_name);
		~Client();


		json send_connection_data();
		void run();
	private:
		void send_message(const string& message);
		string convert_message_to_json(const string& text);
	};
};
#endif //CLIENT_H
