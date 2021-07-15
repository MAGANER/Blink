#pragma once
namespace w
{
#include<windows.h>
};
#include<string>
#include <conio.h>
#include <stdio.h>
#include"sql/Functools.hpp"
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

				if (ch == 8)
				{
					result = Functools::slice(result, 0, result.size() - 1);
					printf("\033[D");
					printf(" ");
					printf("\033[D");
				}
				else if (ch != 13)
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

			auto end = result.find(';');
			if (result.find(';') != string::npos)
				result = Functools::slice(result, 0, end-1);
			return result;
		}
	};
};