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
		bool eq_name     = sql::type_to_string(chunk["name"])     == name;
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
	db.run_set_request(req);
}
bool DataBaseProcessor::does_room_exists(const string& name)
{
	sql::DataBase db("data.db");
	
	string req = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);

	for (auto chunk : result)
	{
		bool eq_name     = sql::type_to_string(chunk["name"]) == name;
		
		if (eq_name) return true;
	}

	return false;
}
bool DataBaseProcessor::is_password_correct(const string& room_name,
										    const string& password)
{
	sql::DataBase db("data.db");

	string req = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);

	for (auto chunk : result)
	{
		bool eq_name = sql::type_to_string(chunk["name"])     == room_name;
		bool eq_pass = sql::type_to_string(chunk["password"]) == password;

		if (eq_name && eq_pass) return true;
	}

	return false;
}
void DataBaseProcessor::add_message(const string& room_name,
									const string& user_name,
									const string& message)
{
	sql::DataBase db("data.db");

	table chats;
	chats["room_name"] = new sql::Text(room_name);
	chats["user_name"] = new sql::Text(user_name);
	chats["message"] = new sql::Text(message);

	string req = sql::make_insert_request(chats, "chats");
	db.run_set_request(req);
}
vector<message> DataBaseProcessor::get_messages(const string& room_name,
												const string& user_name)
{
	sql::DataBase db("data.db");

	string req = sql::make_select_request("chats");
	auto result = db.run_get_request(req);

	vector<message> messages;
	for (auto chunk : result)
	{
		bool eq_room = room_name == sql::type_to_string(chunk["room_name"]);
		bool eq_name = user_name == sql::type_to_string(chunk["user_name"]);
		if (eq_room && eq_name)
		{
			string message = sql::type_to_string(chunk["message"]);
			messages.push_back(make_pair(user_name, message));
		}
	}

	return messages;
}