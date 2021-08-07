#include "RoomMenu.h"
using namespace Blink;

RoomMenu::RoomMenu(const string& encr_key):DataBaseProcessor(encr_key)
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
	auto messages = get_messages(data["room_name"], data["user_name"]);

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
void RoomMenu::run(mode flag)
{
	auto port = atoi(data["port"].c_str());

	if (flag == mode::SERVER)
		server = new Server(commands,data["room_password"], 
									 data["room_name"],
									 data["user_name"],
								     db_key);
	else
	{
		client = new Client(data["ip"], port, commands, data["user_name"],data["room_name"],db_key);
	}

	execute(port);
}
void RoomMenu::run(mode flag, const encr::AES::key_iv& key)
{
	auto port = atoi(data["port"].c_str());

	if (flag == mode::SERVER)
		server = new Server(commands, data["room_password"],
			data["room_name"],
			data["user_name"],
			db_key);
	else
	{
		client = new Client(data["ip"], port, commands, data["user_name"], data["room_name"],key, db_key);
	}

	execute(port);
}
void RoomMenu::execute(int port)
{
	bool run_server = true;

	auto run = [&]()
	{
		server->run(data["room_name"], data["room_password"], port);
		return true;
	};
	while (true)
	{
		if (server != nullptr)
		{
			if (run_server)
			{
				run();
				if (server->should_disconnect())
				{
					system("cls");
					cout << "you have been disconnected from room..." << endl;
					exit = true;
					break;
				}
			}
		}
		else
		{
			client->run();
			if (client->should_disconnect())
			{
				system("cls");
				cout << "you have been disconnected from room..." << endl;
				exit = true;
				break;
			}
		}
	}
}