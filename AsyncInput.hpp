/*
	It is custom input, i use instead of std::cin
	or scanf from C-standart library, because those
	functions block main thread.
	The root idea is to fetch input, when any key is hit.
*/

#pragma once
#include<string>
#include <conio.h>
#include <stdio.h>
#include<vector>
#include"sql/Functools.hpp"
#include"SFML/Window/Keyboard.hpp"
using namespace std;
namespace Blink
{
	class AsyncInput
	{
	private:
		vector<string> history;
		int history_element;
		bool called_history = false;

		string result;
		bool ready = false;
		bool clear_result = false;

		bool counter = false;
		bool clear_finally = true;

		bool key_is_pressed = false;
		void check_key()
		{
			bool up   = !sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
			bool down = !sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
			if (up && down) key_is_pressed = false;
		}
		bool dont_add_ch()
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
				   sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
		}
		void clear_input()
		{
			//move cursor left, change character with space and move cursor left again
			printf("\033[D");
			printf(" ");
			printf("\033[D");
		}
		inline bool is_backspace()
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace);
		}
		inline bool is_enter()
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
		}
	public:
		AsyncInput():history_element(0) {}
		~AsyncInput() {}

		void read(bool secret_info=false)
		{
			if (clear_result)
			{
				result.clear();
				clear_result = false;
			}
			if (_kbhit())
			{
				char ch = _getch();

				if (secret_info && !is_backspace() && !is_enter())
				{
					if (!counter)
					{
						printf(" ");
						counter = true;
					}
					printf("\033[D");
					printf("*");
					counter = false;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !key_is_pressed)
				{
					if (!history.empty())
					{
						if ((history_element - 1) != -1)
						{
							for (auto& ch : result)clear_input();

							history_element--;
							result = history[history_element];
							for (auto& ch : result)cout << ch;
							called_history = true;
						}
					}
					key_is_pressed = true;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !key_is_pressed)
				{
					if (!history.empty())
					{
						if ((history_element + 1) < history.size())
						{
							for (auto& ch : result)clear_input();

							history_element++;
							result = history[history_element];
							for (auto& ch : result)cout << ch;
							called_history = true;
						}
					}
					key_is_pressed = true;
				}

				if (is_backspace() && !result.empty())
				{
					result = Functools::slice(result, 0, result.size() - 1);
					clear_input();
				}
				else if (!is_enter() && !is_backspace() && !dont_add_ch())
				{
					result += ch;
					if (!secret_info)cout << ch;
				}
				else
				{
					if (!is_backspace() && !dont_add_ch())
					{
						history.push_back(result);
						history_element = history.size();
						if (secret_info)counter = false;
						ready = true;
						cout << endl;
					}
				}


				//if you erase everything
				if (result.empty() && called_history)
				{
					history_element = history.size();
					called_history = false;
				}
			}

			check_key();
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