/*
	Base class for menu, which needs to operate with data base.
*/

#pragma once
#include"sql/DataBase.h"
#include<filesystem>
#include<iostream>
#include<string>
#include<vector>
#include<tuple>
#include"Interface.h"
#include"RoomNetworkMode.h"
#include"RoomClient.hpp"
using namespace std;
namespace Blink
{
	namespace sql = SQLite3DataBaseTools;
	namespace fs  = std::filesystem;
	typedef map<string, sql::SQLtype*> table;
	typedef pair<string, string> spair; //pretty abstract
	typedef spair message; //first is user name, second is message
	typedef tuple<string, string, string> str3;
	class DataBaseProcessor
	{
	private:
		string encryption_key;
		string db_name;

		void create_room_connections_info(const string& room_name);
		string replace(const string& str, char old,char _new);
	protected:
		DataBaseProcessor(const string& encr_key,
						  const string& db_name):encryption_key(encr_key),
												 db_name(db_name)
		{
		};
		~DataBaseProcessor() {};

		void create_new_user(const string& name,
							 const string& password);
		bool does_user_exist(const string& name,
							 const string& password);

		void add_connection_info(const string& room_name,
								 const spair& ip_port);
		vector<spair> get_connections_info(const string& room_name);

		void create_new_room(const string& name,
							 const string& password,
							 const string& port,
							 RoomNetworkMode mode);
		bool does_room_exists(const string& name);
		bool is_password_correct(const string& room_name,
								 const string& password);

		vector<str3> get_rooms(const string& password);
		void add_message(const string& room_name,
						 const string& user_name,
						 const string& message);
		vector<message> get_messages(const string& room_name);
		
		string get_encr_key() { return encryption_key; };
	public:
		string get_db_name() { return db_name; }
	};

	extern int get_room_port(const string& room_name,
							 const string& db_key,
							 const string& db_name);
	extern RoomNetworkMode get_room_mode(const string& room_name,
										 const string& db_key,
										 const string& db_name);
};

