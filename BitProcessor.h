/*
	Simple module to process binary numbers.
*/
#pragma once
#include <sstream>
#include <bitset>
#include <string>
namespace Blink
{
namespace
{
	using namespace std;
	string XOR(string a, string b)
	{
		string result = "";
		size_t size = b.size();
		for (size_t i = 0; i < size; i++)
		{
			if (a[i] != b[i]) {
				result += "1";
			}
			else {
				result += "0";
			}
		}
		return result;
	}
	string to_binary(const string& str)
	{
		//convert regular string to binary one

		std::ostringstream oss;
		for (auto c : str) {
			oss << bitset<8>(c);
		}
		return oss.str();
	}
	string from_binary(const string& str)
	{
		//convert binary string to regular one

		stringstream sstream(str);
		string output;
		while (sstream.good())
		{
			bitset<8> bits;
			sstream >> bits;
			char c = char(bits.to_ulong());
			output += c;
		}
		return output;
	}
};
};