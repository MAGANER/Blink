#include "DataBaseProcessor.h"
using namespace Blink;


void DataBaseProcessor::create_new_user(const string& name,
										const string& password)
{
	auto curr_path_str = fs::current_path().string() + fs::path{ "/" + name + ".db" }.string();
	auto db_path = fs::path{ curr_path_str };
	if (!fs::exists(db_path))
	{
		this->db_name = name + ".db";
		sql::DataBase db(curr_path_str, password, true);
		table rooms, chats;


		rooms["name"] = new sql::Text("");
		rooms["password"] = new sql::Text("");
		rooms["port"] = new sql::Text("");
		rooms["mode"] = new sql::Integer(0);

		chats["room_name"] = new sql::Text("");
		chats["user_name"] = new sql::Text("");
		chats["message"] = new sql::Text("");

		string req = sql::make_create_request(rooms, "rooms");
		db.run_set_request(req);

		req = sql::make_create_request(chats, "chats");
		db.run_set_request(req);

		table owner;
		owner["owner"] = new sql::Text("");
		owner["password"] = new sql::Text("");
		req = sql::make_create_request(owner, "owner");
		db.run_set_request(req);

		table _owner;
		_owner["owner"] = new sql::Text(name);
		_owner["password"] = new sql::Text(password);
		req = sql::make_insert_request(_owner, "owner");
		db.run_set_request(req);

	}
}
bool DataBaseProcessor::does_user_exist(const string& name,
										const string& password)
{
	auto curr_path_str = fs::current_path().string() + fs::path{ "/" + name + ".db" }.string();
	auto db_path = fs::path{ curr_path_str };
	if (fs::exists(db_path))
	{
		sql::DataBase db(curr_path_str, password, false);
		if (!db.is_password_correct())
		{
			cout << "password is incorrect!"<<endl;
			return false;
		}
		string req = sql::make_select_request("owner");
		auto result = db.run_get_request(req);
		for (auto chunk : result)
		{
			if (sql::type_to_string(chunk["owner"]) == name &&
				sql::type_to_string(chunk["password"]) == password)
			{
				this->db_name = name + ".db";
				return true;
			}

		}
	}
	return false;
}
void DataBaseProcessor::create_new_room(const string& name,
										const string& password,
										const string& port,
										RoomNetworkMode mode)
{
	sql::DataBase db(db_name, encryption_key, false);

	table room;
	room["name"]	 = new sql::Text(name);
	room["password"] = new sql::Text(password);
	room["port"]	 = new sql::Text(port);
	room["mode"]	 = new sql::Integer((int)mode);

	string req = sql::make_insert_request(room, "rooms");
	db.run_set_request(req);
}
bool DataBaseProcessor::does_room_exists(const string& name)
{
	sql::DataBase db(db_name, encryption_key, false);
	if (!db.is_password_correct())
	{
		cout << "password is incorrect!" << endl;
		return false;
	}

	string req = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);
	for (auto chunk : result)
	{
		bool eq_name     = sql::type_to_string(chunk["name"]) == name;
		
		if (eq_name)
		{
			this->db_name = name + ".db";
			return true;
		}
	}

	return false;
}
bool DataBaseProcessor::is_password_correct(const string& room_name,
										    const string& password)
{
	sql::DataBase db(db_name, encryption_key, false);

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
	sql::DataBase db(db_name, encryption_key, false);
	table chats;
	chats["room_name"] = new sql::Text(room_name);
	chats["user_name"] = new sql::Text(user_name);
	chats["message"] = new sql::Text(message);
	string req = sql::make_insert_request(chats, "chats");
	db.run_set_request(req);
}
vector<message> DataBaseProcessor::get_messages(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	string req = sql::make_select_request("chats");
	auto result = db.run_get_request(req);
	vector<message> messages;
	for (auto chunk : result)
	{
		bool eq_room = room_name == sql::type_to_string(chunk["room_name"]);
		if (eq_room)
		{
			string message = sql::type_to_string(chunk["message"]);
			string user_name = sql::type_to_string(chunk["user_name"]);
			messages.push_back(make_pair(user_name, message));
		}
	}

	return messages;
}
int Blink::get_room_port(const string& room_name,
						 const string& db_key,
						 const string& db_name)
{
	sql::DataBase db(db_name,db_key, false);

	string req  = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);

	//if you call this function, then room should exist
	for (auto chunk : result)
	{
		if (sql::type_to_string(chunk["name"]) == room_name)
		{
			auto val = chunk["port"];

			//sqlite3 has dynamic type casting
			//so integer number can become Primary key
			//and when room is created it can not get port as not number
			if (val->type == sql::SQL_TYPES::PRIMARY_KEY)
			{
				auto result = static_cast<sql::PrimaryKey*>(val)->key;
				return result;
			}
			else if (val->type == sql::SQL_TYPES::INTEGER)
			{
				auto result = static_cast<sql::Integer*>(val)->value;
				return result;
			}
		}
	}
}
RoomNetworkMode Blink::get_room_mode(const string& room_name,
									 const string& db_key,
									 const string& db_name)
{
	sql::DataBase db(db_name, db_key, false);

	string req = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);

	for (auto chunk : result)
	{
		if (sql::type_to_string(chunk["name"]) == room_name)
		{
			auto val = chunk["mode"];

			//sqlite3 has dynamic type casting
			//so integer number can become Primary key
			//and when room is created it can not get port as not number
			if (val->type == sql::SQL_TYPES::INTEGER)
			{
				auto result = static_cast<sql::Integer*>(val)->value;
				return (RoomNetworkMode)result;
			}
		}
	}
}
vector<str3> DataBaseProcessor::get_rooms(const string& password)
{
	vector<str3> rooms;

	sql::DataBase db(db_name, encryption_key, false);
	string req = sql::make_select_request("rooms");
	auto result = db.run_get_request(req);

	for (auto& chunk : result)
	{
		auto to_str = [](sql::SQLtype* type) 
		{
			if (type->type == sql::SQL_TYPES::TEXT)
			{
				auto v = static_cast<sql::Text*>(type);
				return v->value;
			}
			else
			{
				auto v = static_cast<sql::Integer*>(type);
				return to_string(v->value);
			}
		};
		
		str3 room = { to_str(chunk["name"]),
					  to_str(chunk["password"]),
					  to_str(chunk["port"]) 
					};
		rooms.push_back(room);
	}
	return rooms;
}