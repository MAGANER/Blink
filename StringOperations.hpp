/*
	Little module. It contains only 2 functions to split strings.
	To split simply without clearing, and to split and save only printable characters.
*/

#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<utility>
#include<locale>
#include"sql/Functools.hpp"
namespace Blink
{
namespace
{
	namespace StringOperations
	{
		vector<string> split(const string& s, char delim)
		{
			//function with shit clearing
			std::stringstream ss(s);
			std::string item;
			std::vector<std::string> elems;
			while (std::getline(ss, item, delim))
			{
				elems.push_back(item);
			}

			function<bool(string)> should_copy = [&](const string& arg)
			{
				if (arg.size() == 0)return false;
				return true;
			};

			//save only printable characters
			function<bool(char)> filter_elem_pd = [&](char ch) { return isprint(ch)>0; };
			function<string(string)> filter_str = [&](const string& str)
			{
				return Functools::filter(str, filter_elem_pd);
			};
			return Functools::map(Functools::filter(elems, should_copy), filter_str);
		}
		vector<string> simple_split(const string& s, char delim)
		{
			//function without shit clearing
			std::stringstream ss(s);
			std::string item;
			std::vector<std::string> elems;
			while (std::getline(ss, item, delim))
			{
				elems.push_back(item);
			}
			return elems;
		}
	};
};
};