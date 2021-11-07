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
#include"ConnectionChecker.hpp"
#include"getIP.h"

namespace Blink
{
	using namespace std;
	using namespace sf;
	class Server:public NetBase
	{
	protected:
		TcpListener listener;
		string password, room_name;
		int port;
	public:
		Server(command_hash& commands,
			   const string& password,
			   const NetBaseData& data,
			   bool inherited = false);
		virtual ~Server();

	private:

		string get_invite_link_str(int port,
								   const string& room_name,
								   const string& room_password,
								   bool decentralysed,
								   const string& recepient_name);

	protected:
	void create_invite_link(int port,
							const string& room_name,
							const string& room_password,
							bool decentralysed);
	void create_invite_link_to_send(int port,
							const string& room_name,
							const string& room_password,
							bool decentralysed,
							const string& additional_data,
							const string& recepient_name);
	void create_invite_link_to_save(int port,
							const string& room_name,
							const string& room_password,
							bool decentralysed,
							const string& additional_data,
							const string& recepient_name);

	bool is_addres_allowed(vector<IpAddress>& allowed, const IpAddress& ip);


	void check_access(TcpSocket& socket,vector<IpAddress>& allowed);
	void check_access(TcpSocket& socket, 
					  vector<IpAddress>& allowed,
					  vector<int>& port);

	void update_clients(list<RoomClient*>& clients);
	};
};
#endif //SERVER_H
