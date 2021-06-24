/*
* Base class for all console menu interfaces
*/
#pragma once
#include<functional>
#include<map>
#include<string>
#include<iostream>
#include"AsyncInput.hpp"
namespace Blink
{
	using namespace std;

	class Interface
	{
	private:
		AsyncInput input;
		bool dollar_printed = false;
	protected:
		Interface() {}
		~Interface(){}

		map<string, function<void()>> commands;
	private:
		void run_command(const string& command)
		{
			if (commands.find(command) == commands.end())
			{
				cout << "command " <<'`'+ command+'`' << " doesn't exist!" << endl;
			}
			else
			{
				commands[command]();
				cout << endl;
			}
		}
	public:
		void run()
		{
			if (!dollar_printed)
			{
				cout << "$";
				dollar_printed = true;
			}
			input.read();
			if (input.can_get_result())
			{
				run_command(input.get_result());
				dollar_printed = false;
			}
		}
	};
};

