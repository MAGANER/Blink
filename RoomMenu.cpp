#include "RoomMenu.h"
using namespace Blink;

RoomMenu::RoomMenu()
{
	commands["exit"] = function<void()>
		(
			[&]() { exit = true; }
		);
	commands["status"] = function<void()>
		(
			[&]() { /*print_state();*/ return; }
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
void RoomMenu::set_room_data(const string& port,
							 const string& ip,
							 const string& user_name)
{
	data["port"] = port;
	data["ip"]   = ip;
	data["name"] = user_name;
}
void RoomMenu::run(mode flag)
{
	auto port = atoi(data["port"].c_str());
	if (flag == mode::SERVER) server = new Server();
	else client = new Client(data["ip"], port,"test", data["name"]);

	
	bool run_server = true;
	while (true)
	{
		if (server != nullptr)
		{
			if (run_server)
			{
				bool result = server->run("test", port);
				run_server = result;
				cout << "gonna do nothing...";
			}
		}
		else
		{
			string buffer;
			cout << "type text:";
			cin >> buffer;
			client->send_message(buffer);
		}
	}
}