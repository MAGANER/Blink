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
							 const string& room_password,
							 RoomNetworkMode mode)
{
	data["port"] = port;
	data["ip"]   = ip;
	data["user_name"] = user_name;
	data["room_name"] = room_name;
	data["room_password"] = room_password;
	room_mode = mode;
}
void RoomMenu::run(mode flag, bool connecting_with_file_link)
{
	auto port = atoi(data["port"].c_str());
	if (room_mode == RoomNetworkMode::Decentralysed)
	{
		DecentralysedServerClient client_server(commands,
								  data["room_password"],
							      data["room_name"],
								  data["user_name"],
								  db_key,
								  get_db_name(),
								  connecting_with_file_link);
		if (client_server.cant_connect())
		{
			exit = true;
		}
		else
		{
			while (true)
			{
				client_server._run();
				if (client_server.should_disconnect())
				{
					exit = true;
					break;
				}
			}	
		}
	}
	else
	{
		if (flag == mode::SERVER)
			server = new Server(commands, data["room_password"],
				data["room_name"],
				data["user_name"],
				db_key,
				get_db_name());
		else
		{
			client = new Client(data["ip"], port, commands, data["user_name"], data["room_name"], db_key, get_db_name());
		}

		execute(port);
	}
}
void RoomMenu::run(mode flag, const encr::AES::key_iv& key, bool connecting_with_file_link)
{
	auto port = atoi(data["port"].c_str());
	if (room_mode == RoomNetworkMode::Decentralysed)
	{
		DecentralysedServerClient client_server(commands,
												data["room_password"],
												data["room_name"],
												data["user_name"],
												db_key,
												get_db_name(),
												connecting_with_file_link,
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
				client_server._run();
				if (client_server.should_disconnect())
				{
					exit = true;
					break;
				}
			}
		}
	}
	else
	{
		if (flag == mode::SERVER)
			server = new Server(commands, data["room_password"],
				data["room_name"],
				data["user_name"],
				db_key,
				get_db_name());
		else
		{
			client = new Client(data["ip"], port, commands, data["user_name"], data["room_name"], key, db_key, get_db_name());
		}

		execute(port);
	}
}
void RoomMenu::execute(int port)
{
	bool run_server = true;

	auto run = [&]()
	{
		server->run(data["room_name"], data["room_password"], port,room_mode);
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