/*
	Module provides ability to pack message into json.
*/

#ifndef MESSAGE_CREATOR_H
#define MESSAGE_CREATOR_H
#include"json.hpp"
#include<string>
#include<iostream>
namespace Blink
{
namespace
{
	using namespace std;
	using namespace nlohmann;
	enum class MessageType
	{
		Text,
		ComeInRequest
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
	bool can_come_in(const string& message,
					 const string& password,
					 const string& name)
	{
		json jdata = json::parse(message);
		return string(jdata["data"]) == password &&
			   string(jdata["name"]) == name;
	}
	
};
};
#endif //MESSAGE_CREATOR_H
