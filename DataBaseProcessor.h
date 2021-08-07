/*
	Base class for menu, which needs to operate with data base.
*/

#pragma once
#include"sql/DataBase.h"
#include<filesystem>
#include<iostream>
#include<string>
#include<vector>
#include<variant>
using namespace std;
namespace Blink
{
	namespace sql = SQLite3DataBaseTools;
	namespace fs  = std::filesystem;
	typedef map<string, sql::SQLtype*> table;
	typedef pair<string, string> message; //first is user name, second is message
	class DataBaseProcessor
	{
	private:
		string encryption_key;
	protected:
		DataBaseProcessor(const string& encr_key):encryption_key(encr_key) {};
		~DataBaseProcessor() {};

		void create_new_user(const string& name,
							 const string& password);
		bool does_user_exist(const string& name,
							 const string& password);

		void create_new_room(const string& name,
							 const string& password,
							 const string& port);
		bool does_room_exists(const string& name);
		bool is_password_correct(const string& room_name,
								 const string& password);

		void add_message(const string& room_name,
						 const string& user_name,
						 const string& message);
		vector<message> get_messages(const string& room_name,
									 const string& user_name);
		
		string get_encr_key() { return encryption_key; };
	};

	extern int get_room_port(const string& room_name,
							 const string& db_key);
	extern string create_database();
};

