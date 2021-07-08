#include "Client.h"
using namespace Blink;

Client::Client(const string& ip, 
			   int port, 
			   const string& channel_name,
			   const string& user_name)
{
	this->user_name = user_name;
	this->port = port;

	sender.connect(ip, port);
}
Client::~Client()
{
}

void Client::send_message(const string& message)
{
	Packet pack;
	pack << message;
	sender.send(pack);
}
json Client::send_connection_data()
{
	json data;
	data["ip"]   = get_ip();
	data["port"] = 55002;
	send_message(data.dump());
	return data;
}
string Client::convert_message_to_json(const string& text)
{
	json message;
	message["text"] = text;
	message["name"] = user_name;
	return message.dump();
}
void Client::get_message()
{
	Packet pack;
	sender.receive(pack);
	string data;
	pack >> data;
	cout << "got:" << data << endl;
}