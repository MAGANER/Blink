#pragma once
#include"SFML/Network.hpp"
#include"ConnectionData.h"
#include"MessageCreator.h"
#include"EncryptionData.h"
#include"StringOperations.hpp"
#include<iostream>
#include<fstream>
namespace Blink
{
namespace {
namespace inner
	{
		using namespace StringOperations;
		string convert_time_string_to_seconds(const string& str)
		{
			int h, m, s = 0;
			if (sscanf(str.c_str(), "%d:%d:%d", &h, &m, &s) >= 2)
			{
				int secs = h * 3600 + m * 60 + s;
				return to_string(secs);
			}
			return "";
		}
		string convert_secs_to_regular_time_view(int seconds)
		{
			auto min = (int)seconds / 60;
			auto hour = (int)min / 60;
			auto secs = (int)seconds % 60;
			return to_string(hour) + ":" + to_string(min) + ":" + to_string(secs);
		}
		vector<string> parse_time(const string& time)
		{
			auto curr_time = split(time, ' ');
			curr_time = Functools::slice(curr_time, 0, 5);
			return curr_time;
		}
		int get_seconds(const string& time)
		{
			string seconds_str = convert_time_string_to_seconds(time);
			if (seconds_str.empty())
			{
				cout << "can not read link!";
				exit(1);
			}
			return atoi(seconds_str.c_str());
		}
	};
namespace ConnectionChecker 
{
	using namespace sf;
	using namespace std;

	bool can_come_in(const string& message,
					 const string& password,
					 const string& name)
	{

		json jdata = json::parse(message);
		if (jdata["type"] == 4) return true;//come in as friend

		return string(jdata["data"]) == password &&
			string(jdata["name"]) == name;
	}

	bool can_connect(const ConnectionData& data)
	{
		//connect to server
		TcpSocket socket;
		if (socket.connect(IpAddress(data.ip), atoi(data.port.c_str())) != TcpSocket::Done)
		{
			return false;
		}

		//send data to get right to be connected
		string message = convert_message_to_json(data.password,
												 data.room,
												 MessageType::ComeInRequest);
		Packet p; p << message;
		if (socket.send(p) != TcpSocket::Done)
		{
			return false;
		}
		else
		p.clear();

		//get the answer
		socket.receive(p);
		string received_result;
		p >> received_result;
		bool ability = false;
		if (received_result == "1") ability = true;

		if (!ability)cout << "can not come in!" << endl;
		return ability;
	}
	bool connect_with_filelink(ConnectionData& data,
							   EncryptionData& encr_data,
							   const string& path)
	{
		using namespace inner;

		ifstream link_file;
		while (true)
		{
			link_file.open(path, ios::binary);
			if (!link_file)
			{
				cout << "can not open " << path << "!" << endl;
			}
			else break;
		}


		string link;
		istreambuf_iterator<char> inputIt(link_file), emptyInputIt;
		back_insert_iterator<string> stringInsert(link);

		copy(inputIt, emptyInputIt, stringInsert);


		link = decrypt_invite_link(link);

		json jlink = json::parse(link);
		data.ip	  = jlink["ip"];
		data.port = jlink["port"];
		data.room = jlink["room"];
		data.password = jlink["password"];

	
		auto new_encr_data = new EncryptionData(jlink["key"], jlink["iv"]);
		encr_data = *new_encr_data;

		auto curr_time = parse_time(get_curr_time() + " d");
		int curr_seconds = get_seconds(curr_time[3]);

		string _time = jlink["time"];
		string time = Functools::slice(_time, 0, _time.size() - 1);
		auto creation_time = parse_time(time + " d");
		int creation_seconds = get_seconds(creation_time[3]);

		int eq_counter = 0;
		for (size_t i = 0; i < curr_time.size(); i++)
		{
			if (i != 3)
			{
				if (curr_time[i] == creation_time[i])eq_counter++;
			}
		}
		bool date_is_correct = eq_counter == 4;
		bool time_diff_is_correct = abs(curr_seconds - creation_seconds) < (5 * 60);
		if (date_is_correct && time_diff_is_correct)
		{
			if (ConnectionChecker::can_connect(data))
			{
				cout << "link is accepted!" << endl;
				return true;
			}
			else cout << "link is denyied!" << endl;
		}
		else
		{
			cout << "link is denyied!" << endl;
		}

		return false;
	}

};
};
};