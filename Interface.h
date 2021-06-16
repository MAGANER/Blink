/*
* Base class for all console menu interfaces
*/
#pragma once
#include<functional>
#include<map>
#include<string>
#include<iostream>
namespace Blink
{
	using namespace std;

	class Interface
	{
	protected:
		Interface(){}
		~Interface(){}

		map<string, function<void()>> commands;
		void run_command(const string& command)
		{
			if (commands.find(command) == commands.end())
			{
				cout << "command " << command << " doesn't exist!" << endl;
			}
			else
			{
				commands[command]();
			}
		}
	};
};

