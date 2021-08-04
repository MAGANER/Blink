/*
	EnterMenu provides an ability to create new user
	or login as already existing one
*/

#pragma once
#include "Interface.h"
#include"DataBaseProcessor.h"
namespace Blink
{
	class EnterMenu : public Interface, public DataBaseProcessor
	{
	public:
		EnterMenu(const string& encr_key);
		~EnterMenu() {}

		bool change() { return change_menu; }
		string get_user_name() { return user_name; }
	private:
		void create_user();
		void login();
		
		bool change_menu = false;
		string user_name;
	};
};

