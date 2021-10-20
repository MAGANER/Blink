#include "RoomMenu.h"
using namespace Blink;

RoomMenu::RoomMenu(const string& encr_key,
				   const string& db_name):DataBaseProcessor(encr_key,db_name)
{
	db_key = encr_key;
	commands["status"] = function<void()>
		(
			[&]() { print_state(); return; }
		);
	commands["ls"] = function<void()>
		(
			[&]() {list_all_messages(); return; }
	    );
}
void RoomMenu::list_all_messages()
{
	auto messages = get_messages(data["room_name"]);

	int counter = 0;
	for (auto& m : messages)
	{
		cout << m.first << ":" << m.second << endl;
		if (counter == 4)
		{
			char ch = _getch();
			if (ch == 'q')break;

			counter = 0;
		}
		counter++;
	}
}
void RoomMenu::print_state()
{
	cout << "port:" << data["port"]<<endl;
	cout << "ip:"   << data["ip"]<<endl;
	cout << "room name:" << data["room_name"]<<endl;
	cout << "room password:" << data["room_password"]<<endl;
}
void RoomMenu::set_room_data(const string& port,
							 const string& ip,
							 const string& user_name,
							 const string& room_name,
							 const string& room_password)
{
	data["port"] = port;
	data["ip"]   = ip;
	data["user_name"] = user_name;
	data["room_name"] = room_name;
	data["room_password"] = room_password;
}
void RoomMenu::run(bool connecting_with_file_link,
				   bool starting_room)
{
	auto port = atoi(data["port"].c_str());

	NetBaseData _data(data["user_name"],
					  data["room_name"],
				      db_key,
				      get_db_name());
	DecentralysedServerClient client_server(commands,
											data["room_password"],
											_data,
											connecting_with_file_link,
											starting_room);
	if (client_server.cant_connect())
	{
		exit = true;
	}
	else
	{
		while (true)
		{
			client_server.prepare();
			client_server.run_in_console();
			if (client_server.should_disconnect())
			{
				exit = true;
				break;
			}
		}	
	}
}
void RoomMenu::run(const encr::AES::key_iv& key, 
				   bool connecting_with_file_link,
				   bool starting_room)
{
	auto port = atoi(data["port"].c_str());

	NetBaseData _data(data["user_name"],
					  data["room_name"],
					  db_key,
					  get_db_name());
	DecentralysedServerClient client_server(commands,
											data["room_password"],
											_data,
											connecting_with_file_link,
											starting_room,
											true);
	if (client_server.cant_connect())
	{
		exit = true;
	}
	else
	{
		client_server.set_ip_and_port_to_connect(data["ip"], data["port"]);
		client_server.is_connecting(true);
		client_server.set_key_iv(key);
		while (true)
		{
			client_server.prepare();
			client_server.run_in_console();
			if (client_server.should_disconnect())
			{
				exit = true;
				break;
			}
		}
	}
}