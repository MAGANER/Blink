#pragma once
#include "Interface.h"
#include"DataBaseProcessor.h"
namespace Blink
{
	class EnterMenu : public Interface, public DataBaseProcessor
	{
	public:
		EnterMenu();
		~EnterMenu(){}

		bool change() { return change_menu; }
	private:
		void create_user();
		void login();
		
		bool change_menu = false;
	};
};

