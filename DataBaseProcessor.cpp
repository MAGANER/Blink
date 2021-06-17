#include "DataBaseProcessor.h"
using namespace Blink;


void Blink::create_database()
{
	//create database if it doesn't exist

	auto curr_path_str = fs::current_path().string() + fs::path{ "/data.db" }.string();
	auto db_path = fs::path{ curr_path_str };
	if (!fs::exists(db_path))
	{
		sql::DataBase db(curr_path_str);

		table users, rooms, chats;

		users["name"]     = new sql::Text("");
		users["password"] = new sql::Text("");

		rooms["name"]     = new sql::Text("");
		rooms["password"] = new sql::Text("");
		rooms["ip"]       = new sql::Text("");
		rooms["port"]     = new sql::Text("");

		chats["room_name"] = new sql::Text("");
		chats["user_name"] = new sql::Text("");
		chats["message"]   = new sql::Text("");

		string req = sql::make_create_request(users, "users");
		db.run_set_request(req);

		req = sql::make_create_request(rooms, "rooms");
		db.run_set_request(req);

		req = sql::make_create_request(chats, "chats");
		db.run_set_request(req);
	}
}

void DataBaseProcessor::create_new_user(const string& name,
										const string& password)
{
	sql::DataBase db("data.db");

	table users;
	users["name"]     = new sql::Text(name);
	users["password"] = new sql::Text(password);

	string req = sql::make_insert_request(users, "users");
	db.run_set_request(req);
}
bool DataBaseProcessor::does_user_exist(const string& name,
										const string& password)
{
	sql::DataBase db("data.db");

	string req = sql::make_select_request("users");
	auto result = db.run_get_request(req);

	for (auto chunk : result)
	{
		bool eq_name     = sql::type_to_string(chunk["name"]) == name;
		bool eq_password = sql::type_to_string(chunk["password"]) == password;
		if (eq_name && eq_password) return true;
	}

	return false;
}
void DataBaseProcessor::create_new_room(const string& name,
										const string& password,
										const string& port)
{
	sql::DataBase db("data.db");

	table room;
	room["name"]	 = new sql::Text(name);
	room["password"] = new sql::Text(password);
	room["port"]	 = new sql::Text(port);

	string req = sql::make_insert_request(room, "rooms");
	db.run_set_request();
}