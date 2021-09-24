/*
	It contains only 1 function to get random port,
	because it is used in 2 modules.
	Probably there can be added some new functions.
*/
#ifndef NET_RANDOM_H
#define NET_RANDOM_H
#include<random>
#include<string>
namespace
{
namespace NetRandom
{
	int get_random_port()
	{
		using namespace std;
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dist(1, 9);

		string port;
		for (int i = 0; i < 5; ++i)
		{
			port += to_string(dist(gen));
		}
		return atoi(port.c_str());
	}
};
};
#endif //NET_RANDOM_H
