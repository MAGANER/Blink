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
#include"BitProcessor.h"

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
						  const string& password,
						  bool decentralysed);

string encrypt_invite_link(const string& link, const string& recepient_name);
string decrypt_invite_link(const string& link, const string& my_name);
string get_curr_time();
};

