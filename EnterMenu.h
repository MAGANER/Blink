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

	private:
		void create_user();
		void login();
	};
};

