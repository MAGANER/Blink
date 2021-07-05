#ifndef SERVER_H
#define SERVER_H
#include"SocketRunner.h"
#include<string>
#include<iostream>

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
	};
};
#endif //SERVER_H
