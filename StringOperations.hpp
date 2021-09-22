#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<utility>
#include"sql/Functools.hpp"
namespace Blink
{
namespace
{
	namespace StringOperations
	{
		vector<string> split(const string& s, char delim)
		{
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
			function<bool(char)> filter_elem_pd = [&](char ch) { return isprint(ch); };
			function<string(string)> filter_str = [&](const string& str)
			{
				return Functools::filter(str, filter_elem_pd);
			};
			return Functools::map(Functools::filter(elems, should_copy), filter_str);
		}
	};
};
};