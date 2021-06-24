#pragma once
#include<string>
#include <conio.h>
#include <stdio.h>
using namespace std;
namespace Blink
{
	class AsyncInput
	{
	private:
		string result;
		bool ready = false;
		bool clear_result = false;
	public:
		AsyncInput() {}
		~AsyncInput() {}

		void read()
		{
			if (clear_result)
			{
				result.clear();
				clear_result = false;
			}
			if (_kbhit())
			{
				char ch = _getch();
				if (ch != 13)
				{
					result += ch;
					cout << ch;
				}
				else
				{
					ready = true;
					cout << endl;
				}
				
			}
		}
		bool can_get_result() { return ready; }
		string get_result()
		{
			ready = false;
			clear_result = true;
			return result;
		}
	};
};