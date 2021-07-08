#ifndef SERVER_H
#define SERVER_H
#define _s(str) string(str)

#include<string>
#include<iostream>
#include<future>
#include"NetBase.h"
#include"json.hpp"
#include"sql/Functools.hpp"

namespace Blink
{
	using namespace std;
	using namespace nlohmann;
	using namespace sf;
	namespace fp = Functools;

	class Server:public NetBase
	{
	private:
		string connected_ip;
		int connected_port;

		TcpListener listener;
	public:
		Server();
		~Server();

		bool run(const string& channel_name,
				 int port);
		string get_connected_ip() { return connected_ip; }
		int get_connected_port() { return connected_port; }
	private:
		void process_commands(const string& command);
	};
};
#endif //SERVER_H