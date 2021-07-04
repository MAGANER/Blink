#ifndef SERVER_H
#define SERVER_H
#include"masesk/EasySocket.hpp"
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

		void run(const string& channel_name,
				 int port);
	};
};
#endif //SERVER_H
