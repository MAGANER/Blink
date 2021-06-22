#pragma once
#include"sql/DataBase.h"
#include<filesystem>
#include<iostream>
#include<string>
#include<vector>
using namespace std;
namespace Blink
{
	namespace sql = SQLite3DataBaseTools;
	namespace fs  = std::filesystem;
	typedef map<string, sql::SQLtype*> table;
	typedef pair<string, string> message; //first is user name, second is message
	class DataBaseProcessor
	{
	protected:
		DataBaseProcessor()  {};
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
		
		
	};

	extern int get_room_port(const string& room_name);
	extern void create_database();
};

