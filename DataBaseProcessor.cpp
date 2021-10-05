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

		create_offline_clients_table();

	}
}
bool DataBaseProcessor::does_user_exist(const string& name)
{
	auto curr_path_str = fs::current_path().string() + fs::path{ "/" + name + ".db" }.string();
	auto db_path = fs::path{ curr_path_str };
	if (fs::exists(db_path))
	{
		return true;
	}
	return false;
}
bool DataBaseProcessor::can_login(const string& name,
								  const string& password)
{
	auto curr_path_str = fs::current_path().string() + fs::path{ "/" + name + ".db" }.string();
	auto db_path = fs::path{ curr_path_str };
	if (fs::exists(db_path))
	{
		//set data and it will be used after all
		db_name = db_path.string();
		encryption_key = password;

		sql::DataBase db(db_name, encryption_key, false);
		auto req = sql::make_select_request("owner");
		auto result = db.run_get_request(req);

		for (auto& chunk : result)
		{
			bool check1 = sql::type_to_string(chunk["owner"]) == name;
			bool check2 = sql::type_to_string(chunk["password"]) == password;
			if (check1 && check2)return true;
		}
	}
	return false;
}

void DataBaseProcessor::create_room_connections_info(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	table conn_info;

	conn_info["room_name"] = new sql::Text("");
	conn_info["ip"] = new sql::Text("");
	conn_info["port"] = new sql::Integer(0);
	string req = sql::make_create_request(conn_info, "conn_data");
	db.run_set_request(req);
	conn_info["room_name"] = new sql::Text(room_name);
	req = sql::make_insert_request(conn_info, "conn_data");
	db.run_set_request(req);
}

string DataBaseProcessor::replace(const string& str, char old, char _new)
{
	string result;
	for (auto& symbol : str)
	{
		if (symbol == old) result += _new;
		else result += symbol;
	}
	return result;
}
void DataBaseProcessor::add_connection_info(const string& room_name,
											const spair& ip_port)
{
	sql::DataBase db(db_name, encryption_key, false);

	//first get all data
	auto existing_data = get_connections_info(room_name);

	auto add = [&](friendly_connection& data)
	{
		table chunk;
		chunk["room_name"] = new sql::Text(get<0>(data));
		chunk["ip"] = new sql::Text(get<1>(data));
		chunk["port"] = new sql::Integer(get<2>(data));
		auto req = sql::make_insert_request(chunk, "conn_data");
		db.run_set_request(req);
	};
	for (auto& data : existing_data)
	{
		add(data);
	}

	auto new_one = make_tuple(room_name,
							  ip_port.first,
							  atoi(ip_port.second.c_str()));
	add(new_one);
}
bool DataBaseProcessor::does_conn_info_exist(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	string req = sql::make_select_request("conn_data");
	auto result = db.run_get_request(req);
	for (auto& chunk : result)
	{
		bool match = sql::type_to_string(chunk["room_name"]) == room_name;
		if (match)return true;
	}
	return false;
}
vector<friendly_connection> DataBaseProcessor::get_connections_info(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	string req = sql::make_select_request("conn_data");
	auto result = db.run_get_request(req);
	vector<friendly_connection> info;
	for (auto& chunk : result)
	{
		auto t = make_tuple(type_to_string(chunk["room_name"]), 
							type_to_string(chunk["ip"]), 
							atoi(type_to_string(chunk["port"]).c_str()));
		info.push_back(t);
	}
	//return empty vector
	return info;
}
bool DataBaseProcessor::are_ip_port_saved(const spair& ip_port,
										  const string& room_name)
{
	auto data = get_connections_info(room_name);
	for (auto& chunk : data)
	{
		bool check1 = get<1>(chunk)			   == ip_port.first;
		bool check2 = to_string(get<2>(chunk)) == ip_port.second;
		if (check1 && check2)return true;
	}
	return false;
}
void DataBaseProcessor::create_new_room(const string& name,
										const string& password,
										const string& port)
{
	sql::DataBase db(db_name, encryption_key, false);

	table room;
	room["name"]	 = new sql::Text(name);
	room["password"] = new sql::Text(password);
	room["port"]	 = new sql::Text(port);

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
		bool eq_name  = sql::type_to_string(chunk["name"]) == name;
		
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
bool DataBaseProcessor::are_key_iv_saved(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	string req = sql::make_select_request("room_key_iv");
	auto result = db.run_get_request(req);
	if (!db.is_ok()) return false;

	for (auto& chunk : result)
	{
		bool check = sql::type_to_string(chunk["room_name"]) == room_name;
		if (check)return true;
	}
	return false;
}
void DataBaseProcessor::save_room_key(const string& room_name,
									  const spair& key_iv)
{
	sql::DataBase db(db_name, encryption_key, false);

	if (are_key_iv_saved(room_name)) return;//break, do nothing

	table data;
	data["room_name"] = new sql::Text(room_name);
	data["key"] = new sql::Text(key_iv.first);
	data["iv"]  = new sql::Text(key_iv.second);

	auto req = sql::make_create_request(data, "room_key_iv");
	db.run_set_request(req);

	req = sql::make_insert_request(data, "room_key_iv");
	db.run_set_request(req);
}
spair DataBaseProcessor::get_key_iv(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	auto req = sql::make_select_request("room_key_iv");
	auto result = db.run_get_request(req);

	for (auto& chunk : result)
	{
		bool check = sql::type_to_string(chunk["room_name"]) == room_name;
		if (check)
		{
			string key = sql::type_to_string(chunk["key"]);
			string iv  = sql::type_to_string(chunk["iv"]);
			return make_pair(key, iv);
		}
	}

	//should return anyway
	return make_pair("", "");
}
void DataBaseProcessor::save_own_port(const string& room_name, int port)
{
	sql::DataBase db(db_name, encryption_key, false);

	table own_port;
	own_port["room_name"] = new sql::Text(room_name);
	own_port["port"] = new sql::Integer(port);

	auto req = sql::make_create_request(own_port, "own_port");
	db.run_set_request(req);

	req = sql::make_insert_request(own_port, "own_port");
	db.run_set_request(req);
}
int DataBaseProcessor::get_own_port(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	auto req = sql::make_select_request("own_port");
	auto result = db.run_get_request(req);

	for (auto& chunk : result)
	{
		bool eq = sql::type_to_string(chunk["room_name"]) == room_name;
		if (eq)
		{
			auto val = chunk["port"];
			if (val->type == sql::SQL_TYPES::INTEGER)
			{
				return static_cast<sql::Integer*>(val)->value;
			}
			return -1;
		}
	}
	return -1;
}
void DataBaseProcessor::create_offline_clients_table()
{
	sql::DataBase db(db_name, encryption_key, false);

	table offline_clients;
	offline_clients["room_name"] = new sql::Text("");
	offline_clients["ip"]		 = new sql::Text("");
	offline_clients["port"]		 = new sql::Integer(0);

	auto req = sql::make_create_request(offline_clients, "offline_clients");
	db.run_set_request(req);
}
void DataBaseProcessor::add_offline_client(const string& room_name,
										   const string& ip, 
										   const int& port)
{
	sql::DataBase db(db_name, encryption_key, false);

	table offline_clients;
	offline_clients["room_name"] = new sql::Text(room_name);
	offline_clients["ip"] = new sql::Text("'"+ip+"'");
	offline_clients["port"] = new sql::Integer(port);

	auto req = sql::make_insert_request(offline_clients, "offline_clients");
	db.run_set_request(req);
}
vector<pair<string, int>> DataBaseProcessor::get_offline_clients(const string& room_name)
{
	sql::DataBase db(db_name, encryption_key, false);

	auto req = sql::make_select_request("offline_clients");
	auto result = db.run_get_request(req);

	vector<pair<string, int>> data;
	for (auto& chunk : result)
	{
		bool same_room = sql::type_to_string(chunk["room_name"]) == room_name;
		if (same_room)
		{
			string ip = sql::type_to_string(chunk["ip"]);
			ip = Functools::slice(ip, 1, ip.size() - 1);
			int port  = atoi(sql::type_to_string(chunk["port"]).c_str());
			data.push_back(make_pair(ip, port));
		}
	}

	return data;
}
void DataBaseProcessor::erase_offline_client(const string& room_name,
											 const string& ip,
											 const int& port)
{
	sql::DataBase db(db_name, encryption_key, false);

	table data_to_delete;
	data_to_delete["room_name"] = new sql::Text("'"+room_name+"'");
	data_to_delete["ip"]		= new sql::Text("'"+ip+"'");
	data_to_delete["port"]		= new sql::Integer(port);

	auto req = sql::make_delete_request("offline_clients", data_to_delete);
	db.run_set_request(req); //it can raise error
	//so let me show it's not a problem
	//when user gets offline clients info
	//he doesn't save it, he saves it, when exit
	//so if offline client come online
	//then it tries to erase, butt fail
	//because offline client is not saved to db
	//and should not be saved.
}