#ifndef CLIENT_H
#define CLIENT_H
#include"masesk/EasySocket.hpp"
#include<string>
#include<iostream>
namespace Blink
{
	using namespace std;
	using namespace masesk;

	class Client
	{
	private:
		int port;
		string ip, channel_name;
		EasySocket manager;
	public:
		Client(const string& ip,
			   int port,
			   const string& channel_name);
		~Client();

		void send_message(const string& message);
	};
};
#endif //CLIENT_H
