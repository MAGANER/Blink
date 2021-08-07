/*
* Base class for all console menu interfaces
* It provides common mechanism of command handling
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

	/*
	* I don't like this solution, cos it's pretty C-style procedure,
	* but, yeah, it's kinda not bad to make it short
	* and it makes feel like a hacker from 80s.
	* (BTW, C-hackers from 80s didn't use lambda functions)
	* ((and C++, too))
	*/
	namespace
	{
		//overwrapped it to nameless namespace 
		//to avoid multidefinition error
		void process_input(AsyncInput& input,
						   bool& dollar_printed,
						   function<void(string)> action)
		{
			//this async input doesn't block main thread,
			//so dollar printing will be eternal,
			//so we we print one again after input data is received
			if (!dollar_printed)
			{
				cout << "$";
				dollar_printed = true;
			}
			input.read();
			if (input.can_get_result())
			{
				action(input.get_result());
				dollar_printed = false;
			}
		}

		string get_secret_data()
		{
			AsyncInput input;
			while (!input.can_get_result())input.read(true);
			return input.get_result();
		}
	};

	typedef map<string, function<void()>> command_hash;
	class Interface
	{
	private:
		AsyncInput input;
		bool dollar_printed = false;
	protected:
		Interface() 
		{
			commands["exit!"] = function<void()>
				(
					[&]() {exit(0); }
				);
#ifdef WIN32
			commands["cls"] = function<void()>
				(
					[&]() {system("cls"); }
			);
#endif
#ifdef unix
			commands["cls"] = function<void()>
				(
					[&]() {system("clear"); }
			);
#endif
		}
		~Interface(){}

		command_hash commands;
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
		virtual void run()
		{
			auto action = [&](const string& command) { run_command(command);  };
			process_input(input, dollar_printed, action);
		}
	};
};

