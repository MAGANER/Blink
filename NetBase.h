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
#include"DataBaseProcessor.h"
#include"RoomClient.hpp"
namespace Blink
{
using namespace std;
using namespace sf;
namespace fp = Functools;
namespace encr = Encryption;

class NetBase :public DataBaseProcessor
{
protected:
	encr::AES::key_iv key_iv;
private:
	function<void(string)> input_callback;

	AsyncInput input;
	string input_buffer;
	bool ready_to_send_message = false;
	bool dollar_printed = false;
	bool disconnect = false;

	bool is_user = false;
	bool can_show = true;

	string user_name, room_name;
protected:
	command_hash commands;

	NetBase(const command_hash& commands,
		const string& user_name,
		const string& room_name,
		const string& db_key,
		const string& db_name,
		bool is_user=false) :DataBaseProcessor(db_key, db_name)
	{
		input_callback = [&](const string& str)
		{
			input_buffer = str;
			ready_to_send_message = true;
		};
		this->is_user = is_user;
		this->commands = commands;
		this->user_name = user_name;
		this->room_name = room_name;
	}
	~NetBase() {}

	virtual void send_message(TcpSocket& socket,
							  const string& message)
	{
		if (!message.empty())
		{
			Packet pack;
			string jmessage = convert_message_to_json(message, user_name, MessageType::Text);
			pack << encr::AES::encrypt(key_iv, jmessage);
			socket.send(pack);
			add_message(room_name, user_name, message);
		}
	}
	virtual void send_message(TcpSocket& socket,
							  const string& message,
							  MessageType msg)
	{
		if (!message.empty())
		{
			Packet pack;
			string jmessage = convert_message_to_json(message, user_name, msg);
			pack << encr::AES::encrypt(key_iv, jmessage);
			socket.send(pack);
			add_message(room_name, user_name, message);
		}
	}
	void send_jmessage(TcpSocket& socket,
					   const string& message)
	{
		if (!message.empty())
		{
			Packet pack;
			pack << encr::AES::encrypt(key_iv, message);
			socket.send(pack);
		}
	}
	void resend_messages_from_server(list<RoomClient*>& clients,
									 int exlude_id,
									 const string& message)
	{
		for (auto& client : clients)
		{
			if (client->id != exlude_id)
			{
				send_jmessage(*client->socket, message);
			}
		}
	}
	void get_and_show_message(TcpSocket& socket)
	{
		string data = NetBase::get_message(socket);

		if (data.size() > 0 && can_show)
		{
			auto cut = [&](const string& str) { return fp::slice(str, 0, str.size()); };
			

			//move it down, print received message and return
			cout << endl;
			json parsed = json::parse(data);
			add_message(room_name, parsed["name"], cut(parsed["data"]));
			cout << cut(parsed["name"])<<'|'<<cut(parsed["data"]) << endl;
			dollar_printed = false;
		}
	}
	void return_and_show_message(RoomClient* client,
								 list<RoomClient*>& clients)
	{
		string message = NetBase::get_message(*client->socket);
		if (message.size() > 0 && message != "\n\n\n666\m\m\m\m1488\,\,\,")
		{
			//move it down, print received message and return
			cout << endl;
			auto cut = [&](const string& str) { return fp::slice(str, 0, str.size()); };
			

			json parsed = json::parse(message);
			auto name = cut(parsed["name"]);
			auto data = cut(parsed["data"]);
			add_message(room_name, name, data);
			cout <<name << '|' << data << endl;
			dollar_printed = false;
			resend_messages_from_server(clients, client->id, message);
		}
	}

	bool process_command()
	{
		if (input_buffer[0] == '$')
		{
			//process command
			string command = fp::slice(input_buffer, 1, input_buffer.size());
			if (command == "exit") disconnect = true;
			else if (commands.find(command) != commands.end())
				commands[command]();
			else
				cout << "command " << "`" + command + "`" << " doesn't exist!";
			cout << endl;
			return true;
		}
		return false;
	}
	void send_input(TcpSocket& socket)
	{
		 if (ready_to_send_message && !dollar_printed)send_message(socket, input_buffer);		
	}
	void receive_input_and_send_message(TcpSocket& socket)
	{
		process_input(input, dollar_printed, input_callback);
		if(!process_command()) send_input(socket);
		input_buffer.clear();
	}
	void receive_input_and_send_message_to_all(list<RoomClient*>& clients)
	{
		process_input(input, dollar_printed, input_callback);
		if (!process_command()) 
		{
			for (auto& client : clients)
				send_input(*client->socket);
		}
		input_buffer.clear();
	}

	string get_message(TcpSocket& socket)
	{
		Packet pack;
		if (socket.receive(pack) == TcpSocket::Disconnected)
		{
			can_show = false;
			if (is_user)disconnect = true;
			return "\n\n\n666\m\m\m\m1488\,\,\,";
		}
		else
		{
			can_show = true;
			string data;
			pack >> data;
			return encr::AES::decrypt(key_iv, data);
		}
	}
	string get_raw_message(TcpSocket& socket)
	{
		Packet pack;
		socket.receive(pack);
		string data;
		pack >> data;
		return data;
	}
	
	void update_input()
	{
		dollar_printed = false;
	}
public:
	bool should_disconnect() { return disconnect; }
};
};
#endif //NET_BASE_H

