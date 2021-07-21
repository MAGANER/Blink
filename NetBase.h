/*
	The base class for Client/Server.
	it contains common methods to receive/send messages,
	also it takes control over keyboard input handling.
*/
#ifndef NET_BASE_H
#define NET_BASE_H
#include<string>
#include<iostream>
#include"SFML/Network.hpp"
#include"Interface.h"
#include"sql/Functools.hpp"
#include"MessageCreator.h"
#include"encryption/Encryption.h"
namespace Blink
{
using namespace std;
using namespace sf;
namespace fp = Functools;
namespace encr = Encryption;

class NetBase
{
protected:
	encr::key_iv key_iv;
private:
	function<void(string)> input_callback;

	AsyncInput input;
	string input_buffer;
	bool ready_to_send_message = false;
	bool dollar_printed = false;

	command_hash commands;
	
	string user_name;
protected:
	NetBase(const command_hash& commands,
			const string& user_name)
	{
		input_callback = [&](const string& str)
		{
			input_buffer = str;
			ready_to_send_message = true;
		};
		this->commands  = commands;
		this->user_name = user_name;
	}
	~NetBase(){}
	
	virtual void send_message(TcpSocket& socket,
							  const string& message)
	{
		Packet pack;
		string jmessage = convert_message_to_json(message, user_name, MessageType::Text);
		pack << encr::encrypt(key_iv,jmessage);
		socket.send(pack);
	}
	void get_and_show_message(TcpSocket& socket)
	{
		string data = NetBase::get_message(socket);
		if (data.size() > 0)
		{
			//move it down, print received message and return
			cout << endl;
			json parsed = json::parse(data);
			auto cut = [&](const string& str) { return fp::slice(str, 0, str.size()); };
			cout << cut(parsed["name"])<<'|'<<cut(parsed["data"]) << endl;
			dollar_printed = false;
		}
	}
	void receive_input_and_send_message(TcpSocket& socket)
	{
		process_input(input, dollar_printed, input_callback);
		if (input_buffer[0] == '$')
		{
			//process command
			string command = fp::slice(input_buffer, 1, input_buffer.size());
			if (commands.find(command) != commands.end())
				commands[command]();
			else
				cout <<"command "<< "`" + command + "`" << " doesn't exist!";
			cout << endl;
		}
		else if (ready_to_send_message && !dollar_printed)send_message(socket, input_buffer);
		input_buffer.clear();
	}

	string get_message(TcpSocket& socket)
	{
		Packet pack;
		socket.receive(pack);
		string data;
		pack >> data;
		return encr::decrypt(key_iv,data);
	}
	string get_raw_message(TcpSocket& socket)
	{
		Packet pack;
		socket.receive(pack);
		string data;
		pack >> data;
		return data;
	}
};
};
#endif //NET_BASE_H

