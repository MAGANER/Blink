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
	typedef tuple<string, string, int> friendly_connection;
	class DataBaseProcessor
	{
	private:
		string encryption_key;
		string db_name;

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
		bool are_ip_port_saved(const spair& ip_port,
							   const string& room_name);
		vector<friendly_connection> get_connections_info(const string& room_name);
		bool does_conn_info_exist(const string& room_name);
		void create_room_connections_info(const string& room_name);

		void create_offline_clients_table();
		void add_offline_client(const string& room_name,
								const string& ip, 
								const int& port);
		vector<pair<string, int>> get_offline_clients(const string& room_name);
		void erase_offline_client(const string& room_name,
								  const string& ip,
								  const int& port);

		void save_own_port(const string& room_name,int port);
		int get_own_port(const string& room_name);

		void save_room_key(const string& room_name,
						   const spair& key_iv);
		spair get_key_iv(const string& room_name);
		bool are_key_iv_saved(const string& room_name);

		void create_new_room(const string& name,
							 const string& password,
							 const string& port);
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
};

