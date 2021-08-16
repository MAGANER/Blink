/*
	Module implements special algorithm to encrypt inviting link.
*/

#pragma once
#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include<chrono>
#include<string>
#include"encryption/Encryption.h"
#include"json.hpp"
#include"sql/Functools.hpp"
#include"NoiseGenerator.hpp"
#include"BitProcessor.h"
#include"gzip.h"
#define SEPARATOR "+++"+encr::SHA::sha256("<du-deg00dwe-ed-ha----te-niggers-hail-hitler-fuck-jewish-scum-hail-white-race-1488-rahowa-fuck-you-assheadisyourrealnamesoiloveanalcuntuhategovernmentihateyouandfuckoffdouchebag>")+"---"


namespace Blink
{
using namespace std;
using namespace nlohmann;
namespace encr = Encryption;

string create_invite_link(const string& ip,
						  const string& port,
						  const string& room,
						  const string& IV,
						  const string& KEY,
						  const string& password);

string encrypt_invite_link(const string& link);
string decrypt_invite_link(const string& link);
string get_curr_time();

namespace inner 
{
string compress(const string& data);
string decompress(const string& data);
};
};

