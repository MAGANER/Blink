/*
	Regular TCP server with ability to get/send messages.
	Also it checks an ability to be connected.
*/
#ifndef SERVER_H
#define SERVER_H
#define _s(str) string(str)
#include<string>
#include<iostream>
#include<fstream>
#include<list>
#include"NetBase.h"
#include"sql/Functools.hpp"
#include"MessageCreator.h"
#include"InviteLinkCreator.h"
#include"getIP.h"


namespace Blink
{
	using namespace std;
	using namespace sf;
	class Server:public NetBase
	{
	private:
		TcpListener listener;
		string password, room_name;
	public:
		Server(const command_hash& commands,
			   const string& password,
			   const string& room_name,
			   const string& user_name,
			   const string& db_key,
			   const string& db_name);
		~Server();

		bool run(const string& room_name,
				 const string& room_password,
				 int port,
				 RoomNetworkMode mode);
	private:
		void create_invite_link(int port,
								const string& room_name,
								const string& room_password);

		void run_one2one_mode(const string& room_name,
							  const string& room_password,
							  int port);
		void run_one2ones_mode(const string& room_name,
							   const string& room_password,
							   int port);

		bool is_addres_allowed(vector<IpAddress>& allowed, const IpAddress& ip);
		void check_access(TcpSocket& socket,vector<IpAddress>& allowed);
		void update_clients(list<RoomClient*>& clients);
	};
};
#endif //SERVER_H
