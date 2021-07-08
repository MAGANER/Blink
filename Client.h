#ifndef CLIENT_H
#define CLIENT_H
#include<string>
#include<iostream>
#include"json.hpp"
#include"getIP.h"
#include"SFML/Network.hpp"
#include<future>

namespace Blink
{
	using namespace std;
	using namespace sf;
	using namespace nlohmann;

	class Client
	{
	private:
		int port;
		TcpSocket sender;

		string user_name;
		string convert_message_to_json(const string& text);
	public:
		Client(const string& ip,
			   int port,
			   const string& channel_name,
			   const string& user_name);
		~Client();

		void send_message(const string& message);
		void get_message();
		json send_connection_data();
		
	};
};
#endif //CLIENT_H
