#include "RoomMenu.h"
using namespace Blink;

RoomMenu::RoomMenu()
{
	commands["status"] = function<void()>
		(
			[&]() { print_state(); return; }
		);
	commands["send"] = function<void()>
		(
			[&]() 
			{ 
				/*read_sending_input = true;
				send();*/ 
				return;
			}
		);

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
		server = new Server(commands,data["room_password"], data["room_name"]);
	else
	{
		client = new Client(data["ip"], port, commands);
	}
	

	bool run_server = true;

	auto run = [&]()
	{
		server->run("test", port);
		return true;
	};
	while (true)
	{
		if (server != nullptr)
		{
			if (run_server)
			{
				auto result = run();

				run_server = result;
				cout << "gonna do nothing...";
			}
		}
		else
		{
			client->run();
		}
	}
}