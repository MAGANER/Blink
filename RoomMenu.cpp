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
							 const string& ip)
{
	data["port"] = port;
	data["ip"]   = ip;
}
void RoomMenu::run(mode flag)
{
	auto port = atoi(data["port"].c_str());
	if (flag == mode::SERVER) server = new Server();
	else client = new Client(data["ip"], port,"test");

	while (true)
	{
		if (server != nullptr)
		{
			server->run("test",port);
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