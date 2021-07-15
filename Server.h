#ifndef SERVER_H
#define SERVER_H
#define _s(str) string(str)

#include<string>
#include<iostream>
#include<future>
#include"NetBase.h"
#include"sql/Functools.hpp"

namespace Blink
{
	using namespace std;
	using namespace sf;
	namespace fp = Functools;

	class Server:public NetBase
	{
	private:
		TcpListener listener;
	public:
		Server(const command_hash& commands);
		~Server();

		bool run(const string& channel_name,
				 int port);
	private:
		void process_commands(const string& command);
	};
};
#endif //SERVER_H
