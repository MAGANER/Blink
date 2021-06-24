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
	public:
		AsyncInput() {}
		~AsyncInput() {}

		void read()
		{
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
			return result;
		}
	};
};