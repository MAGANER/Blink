#ifndef CLIENT_H
#define CLIENT_H
#include"SocketRunner.h"
#include<string>
#include<iostream>
#include"json.hpp"
#include"getIP.h"

namespace Blink
{
	using namespace std;
	using namespace masesk;
	using namespace nlohmann;

	class Client
	{
	private:
		int port;
		string ip, channel_name;
		EasySocket manager;

		string user_name;
		string local_ip;
		string convert_message_to_json(const string& text);
	public:
		Client(const string& ip,
			   int port,
			   const string& channel_name,
			   const string& user_name);
		~Client();

		void send_message(const string& message);
	};
};
#endif //CLIENT_H
