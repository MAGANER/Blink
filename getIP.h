#ifndef GET_IP_H
#define GET_IP_H
#include<fstream>
#include<string>
#include<iostream>
#include"SFML/Network/IpAddress.hpp"
using namespace std;
namespace
{
	string get_ip(bool net_mode)
	{
		cout << "hey, man!" << endl;
		cout << net_mode ? sf::IpAddress::getPublicAddress().toString() :
			sf::IpAddress::getLocalAddress().toString();
		return net_mode?sf::IpAddress::getPublicAddress().toString():
						sf::IpAddress::getLocalAddress().toString();
	}
};
#endif //GET_IP_H