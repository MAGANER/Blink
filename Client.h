#ifndef CLIENT_H
#define CLIENT_H
#include<string>
#include<iostream>
#include<future>
#include"getIP.h"
#include"NetBase.h"
#include"Interface.h" // process_input

namespace Blink
{
	using namespace std;
	using namespace sf;

	class Client:public NetBase
	{
	private:
		int port;
		string user_name;

		TcpSocket sender;
	public:
		Client(const string& ip,
			   int port,
			   const command_hash& commands);
		~Client();


		void run();
	private:
		void send_message(const string& message);
	};
};
#endif //CLIENT_H
