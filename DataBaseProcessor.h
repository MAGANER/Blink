#pragma once
#include"sql/DataBase.h"
#include<filesystem>
#include<iostream>
#include<string>
using namespace std;
namespace Blink
{
	namespace sql = SQLite3DataBaseTools;
	namespace fs  = std::filesystem;
	typedef map<string, sql::SQLtype*> table;
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
	};

	extern void create_database();
};

