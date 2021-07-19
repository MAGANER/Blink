#ifndef SERVER_H
#define SERVER_H
#define _s(str) string(str)

#include<string>
#include<iostream>
#include<future>
#include"NetBase.h"
#include"sql/Functools.hpp"
#include"MessageCreator.h"

namespace Blink
{
	using namespace std;
	using namespace sf;
	namespace fp = Functools;

	class Server:public NetBase
	{
	private:
		TcpListener listener;
		string password, room_name;
	public:
		Server(const command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name);
		~Server();

		bool run(const string& channel_name,
				 int port);
	private:
		void process_commands(const string& command);
	};
};
#endif //SERVER_H
