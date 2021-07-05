/*
* This solution works for Windows as well, 
* i don't know what about Unix-like, but i think
* it can include different implementations with
* macros as Win32 e.t.c.
* So we save ipconfig result to file get its 13 line
* with ipv4 data, save it, delete file and then return data.
*/
#ifndef GET_IP_H
#define GET_IP_H
#include<fstream>
#include<string>
#include<iostream>
using namespace std;
namespace
{
	string get_ip()
	{
		system("ipconfig > .ipdata");
		ifstream file(".ipdata");

		int counter = 0;
		string line;

		//13 line contains ipv4 data
		while (getline(file, line) && counter != 13) { counter++; }
		file.close();
		system("erase .ipdata");

		//after : there are 2 spaces, don't need it
		return line.substr(line.find(":") + 2);
	}
};
#endif //GET_IP_H