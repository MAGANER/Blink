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
namespace Blink
{
using namespace std;
using namespace sf;

class NetBase
{
private:
	function<void(string)> input_callback;

	AsyncInput input;
	string input_buffer;
	bool ready_to_send_message = false;
	bool dollar_printed = false;
protected:
	NetBase()
	{
		input_callback = [&](const string& str)
		{
			input_buffer = str;
			ready_to_send_message = true;
		};
	}
	~NetBase(){}
	
	virtual void send_message(TcpSocket& socket,
							  const string& message)
	{
		Packet pack;
		pack << message;
		socket.send(pack);
	}
	void get_and_show_message(TcpSocket& socket)
	{
		string data = NetBase::get_message(socket);
		if (data.size() > 0)cout << "got:" << data << endl;
	}
	void receive_input_and_send_message(TcpSocket& socket)
	{
		process_input(input, dollar_printed, input_callback);
		if (ready_to_send_message)send_message(socket,input_buffer);
		input_buffer.clear();
	}
private:
	virtual string get_message(TcpSocket& socket)
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

