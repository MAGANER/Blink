/*
	Module provides ability to pack message into json.
*/

#ifndef MESSAGE_CREATOR_H
#define MESSAGE_CREATOR_H
#include"json.hpp"
#include"RoomClient.hpp"
#include<string>
#include<list>
#include<iostream>
#include"SFML/Network/IpAddress.hpp"
namespace Blink
{
namespace
{
	using namespace std;
	using namespace nlohmann;
	enum class MessageType
	{
		Text,
		ComeInRequest,
		ClientsInfo,
		RoomName,
		ComeInAsFriend,
	};

	string convert_message_to_json(const string& data,
								   const string& name,
								   MessageType type)
	{
		json jdata;
		jdata["type"] = (int)type;
		jdata["data"] = data;
		jdata["name"] = name;
		string result = jdata.dump();
		return result;
	}
	string create_clients_info_message(list<RoomClient*>& clients,
									   const string& room_name,
									   const string& room_password)
	{
		/*
			it creates json message, containing data about connected clients,
			also it contains data about room. Use it in decentralysed mode,
			so after one leaves room, another one who got acccess can
			enter room at any time.
		*/

		json jdata;
		jdata["type"] = (int)MessageType::ClientsInfo;
		jdata["room_name"] = room_name;
		jdata["room_password"] = room_password;

		string client_field = "client";
		string port_field = "port";
		int client_counter = 0;
			
		jdata["max_clients"] = clients.size();
		for (auto& client : clients)
		{
			client_field = client_field + to_string(client_counter);
			port_field = port_field + to_string(client_counter);
			auto ip = client->socket->getRemoteAddress();
			if (ip != sf::IpAddress::None)
			{
				jdata[client_field] = ip.toString();
				jdata[port_field] = client->listner_port;
			}
			client_counter++;
		}
		return jdata.dump();
	}	
};
};
#endif //MESSAGE_CREATOR_H
