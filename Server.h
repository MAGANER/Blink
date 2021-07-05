#ifndef SERVER_H
#define SERVER_H
#include"SocketRunner.h"
#include"Interface.h" // for process_input procedure
#include<string>
#include<iostream>
#include<future>

namespace Blink
{
	using namespace std;
	using namespace masesk;
	class Server
	{
	private:
		EasySocket manager;
	public:
		Server();
		~Server();

		bool run(const string& channel_name,
				 int port);
	private:
		AsyncInput input;
		bool dollar_printed = false;
		void process_commands(const string& command);
	};
};
#endif //SERVER_H
