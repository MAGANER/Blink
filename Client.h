/*
	class that has ability to connect to another server.
*/
#ifndef CLIENT_H
#define CLIENT_H
#include<string>
#include<iostream>
#include"getIP.h"
#include"NetBase.h"
#include"Interface.h" // process_input
#include"encryption/Encryption.h"
#include"DataBaseProcessor.h"
namespace Blink
{
	using namespace std;
	using namespace sf;

	class Client:public NetBase
	{
	private:
		int port;
		TcpSocket sender;

		string name, room;
	public:
		Client(const string& ip,
			   int port,
			   const command_hash& commands,
			   const string& user_name,
			   const string& room_name,
			   const string& db_key,
			   const string& db_name);
		Client(const string& ip,
			int port,
			const command_hash& commands,
			const string& user_name,
			const string& room_name,
			const encr::AES::key_iv& keys,
			const string& db_key,
			const string& db_name);
		~Client();

		void run();
	};
};
#endif //CLIENT_H
