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
#include"StringOperations.hpp"

namespace Blink
{
using namespace std;
using namespace sf;
namespace fp = Functools;
namespace encr = Encryption;


/*
* this data is sent from client you connected,
* it's required with decentralysed mode.
* after one leaves room, another one who got acccess can
  enter room at any time.
  Also it contains data of another connected clients.
*/
struct Received_decentralysed_info
{
	Received_decentralysed_info(const string& room_name,
								const string& room_password,
								const list<pair<IpAddress, int>>& clients)
	{
		this->room_name		= room_name;
		this->room_password = room_password;
		
		//because = operator doesn't work correctly
		for (auto& client : clients)
		{
			this->clients.push_back(client);
		}
	}
	~Received_decentralysed_info()
	{}
	string room_name, room_password;
	list<pair<IpAddress,int>> clients;
};
struct Received_additional_info
{
	Received_additional_info(const list<pair<string, int>>& clients)
	{
		for (auto& client : clients)
		{
			this->clients.push_back(client);
		}
	}
	list<pair<string, int>> clients;
};


//i know it looks weird
#define s string
#define cs const s&

struct NetBaseData
{
	s user_name, room_name, db_key, db_name;

	NetBaseData(cs user_name,
			cs room_name,
			cs db_key,
			cs db_name):user_name(user_name),
						 room_name(room_name),
						 db_key(db_key),
						 db_name(db_name)
	{}
};
struct message_store
{
	message_store(const string& name,
		const string& text) :
		name(name), text(text)
	{}
	~message_store()
	{}
	string name, text;
};
class NetBase :public DataBaseProcessor
{
protected:
	encr::AES::key_iv key_iv;
	bool disconnect = false;
private:
	function<void(string)> input_callback;

	AsyncInput input;
	string input_buffer;
	bool ready_to_send_message = false;
	bool dollar_printed = false;

	bool is_user = false;
	bool can_show = true;

	string user_name, room_name;
protected:
	command_hash commands;

	NetBase(const command_hash& commands,
		const NetBaseData& data,
		bool is_user=false) :DataBaseProcessor(data.db_key, data.db_name)
	{
		input_callback = [&](const string& str)
		{
			input_buffer = str;
			ready_to_send_message = true;
		};
		this->is_user = is_user;
		this->commands = commands;
		this->user_name = data.user_name;
		this->room_name = data.room_name;
	}

	bool received_clients_info = false;
	Received_decentralysed_info* received_info = nullptr;

	bool received_room_name = false;
	string *correct_room_name;
	string *correct_password;

	bool received_additional_info = false;
	Received_additional_info* additional_info = nullptr;

	virtual ~NetBase() {}

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
	void send_auto_message(TcpSocket& socket,
						   const string& message)
	{
		//this is special method to send system messages
		Packet pack;
		string jmessage = convert_message_to_json(message, "Blinking Wizard", MessageType::Text);
		pack << encr::AES::encrypt(key_iv, jmessage);
		socket.send(pack);
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

			if(MessageType::Text == msg)
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


	void get_and_save_message(TcpSocket& socket,
							  list<message_store*>& messages,
							  bool show=false)
	{
		string data = NetBase::get_message(socket);
		if (data.size() > 0 && can_show)
		{
			auto cut = [&](const string& str) { return fp::slice(str, 0, str.size()); };
			json parsed = json::parse(data);

			message_store* msg = new message_store(cut(parsed["name"]), cut(parsed["data"]));
			messages.push_back(msg);
			show_message(data);
		}
	}
	void show_message(const string& message)
	{
		auto cut = [&](const string& str) { return fp::slice(str, 0, str.size()); };

		//move it down, print received message and return
		cout << endl;
		json parsed = json::parse(message);
		add_message(room_name, parsed["name"], cut(parsed["data"]));
		cout << cut(parsed["name"]) << '|' << cut(parsed["data"]) << endl;
		dollar_printed = false;
	}
	void get_and_show_message(TcpSocket& socket)
	{
		//for one2one mode

		string data = NetBase::get_message(socket);

		if (data.size() > 0 && can_show)
		{
			show_message(data);
		}
	}
	void return_and_show_message(RoomClient* client,
								 list<RoomClient*>& clients)
	{
		//for one2many ones mode(also decentralysed)
		string message = NetBase::get_message(*client->socket);
		if (message.size() > 0 && can_show)
		{
			show_message(message);
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


	bool socket_dissconnected = false; //used in Server::update_clients only
	string get_message(TcpSocket& socket)
	{
		Packet pack;
		if (socket.receive(pack) == TcpSocket::Disconnected &&
			socket.getRemoteAddress() == IpAddress::None)
		{
			can_show = false;
			//if (is_user)disconnect = true;
			socket_dissconnected = true;
			return "";
		}
		else
		{
			can_show = true;
			string data;
			pack >> data;
			data = encr::AES::decrypt(key_iv, data);
			if (data.size() > 0)
			{	
				json parsed = json::parse(data);

				auto json_type_to_str = [](const json& js) {return to_string(js["type"]);};
				auto type_to_int = [=](const json& js){
					return atoi(json_type_to_str(js).c_str()); };

				if (type_to_int(parsed) == (int)MessageType::ClientsInfo)
				{
					can_show = false;
					received_clients_info = true;
					string room_name = parsed["room_name"];
					string room_password = parsed["room_password"];
					list<pair<IpAddress, int>> clients;
					int clients_max = (int)parsed["max_clients"];
					for (int counter = 0; counter < clients_max; counter++)
					{
						string cfield = "client" + to_string(counter);
						string pfield = "port"   + to_string(counter);
						string ip = parsed[cfield];
						int port = parsed[pfield];
						IpAddress parsed_ip = IpAddress(ip);
						clients.push_back(make_pair(parsed_ip, port));
					}
					received_info = new Received_decentralysed_info(room_name, room_password, clients);
					return "";
				}
				if(type_to_int(parsed) == (int)MessageType::Text)
				{
					add_message(room_name, parsed["name"], parsed["data"]);
				}
			}
			return data;
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
	
	bool does_connection_exist(const pair<string, int>& conn,
						       const vector<pair<string, int>>& data)
	{
		for (auto& elem : data)
		{
			bool check1 = elem.first  == conn.first;
			bool check2 = elem.second == conn.second;
			if (check1 && check2) return true;
		}
		return false;
	}
	pair<string, int> split_to_ip_port(const string& str)
	{
		auto splitted = StringOperations::split(str, ':');
		return make_pair(splitted[0], atoi(splitted[1].c_str()));
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