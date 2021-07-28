#pragma once
#include<string>
#include<vector>
#include<random>
namespace Blink
{
namespace
{
	using namespace std;
	vector<char> get_ascii_charset()
	{
		vector<char> charset;
		for (int i = 0; i < 257; i++)
		{
			char ch = i;
			charset.push_back(ch);
		}
		return charset;
	}
	char get_random_char(const vector<char>& charset)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, charset.size()-1);

		return charset[dist(mt)];
	}
	int get_random_int(int beg, int end)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(beg, end);
		return dist(mt);
	}
	string generate_noise_string(size_t size, const vector<char>& charset)
	{
		string noise;
		for (size_t i = 0; i < size; ++i)
		{
			noise.push_back(get_random_char(charset));
		}
		return noise;
	}
};
};