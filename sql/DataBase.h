#pragma once
#include"../sqleet/sqleet.h"
#include<map>
#include<vector>
#include<algorithm>
#include<iostream>
#include <iterator>
#define MAP_FUNCTOOLS
#include"Types.h"
namespace SQLite3DataBaseTools
{
using namespace std;
typedef map<string, SQLtype*> chunk;

//overwrapper for C-style connection.
class DataBase
{
private:
	sqlite3* db  = nullptr;
	string error_message;

	int ok; //put here value from sqlite library
	bool correct_password = false;
public:
	DataBase(const string& path, const string& key, bool first_time);
	~DataBase();

	bool is_ok();
	bool is_password_correct() {
		return correct_password;
	};
	string get_error_message();

	bool run_set_request(const string& request);
	vector<chunk> run_get_request(const string& request);
	vector<chunk> run_get_request(const string& request,function<bool(string, SQLtype*)> predicat);
private:
	static int get_request_callback(void* data, int argc, char** argv, char** azColName);
};

//functions to create special requests to database
extern string make_create_request(const map<string, SQLtype*>& data, const string& table_name);
extern string make_insert_request(const map<string, SQLtype*>& data, const string& table_name);
extern string make_select_request(const string& table_name);
extern string make_select_request(const string& table_name,
								  const vector<string>& fields);
extern string make_update_request(const map<string,SQLtype*>& fields_to_update,
								  const string& table_name);
extern string make_delete_request(const string& table_name,
								  const map<string, SQLtype*>& fields_to_update);
};

