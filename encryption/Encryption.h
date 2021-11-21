/*
	special module to provide easy AES/RSA encrypting, SHA-256 
	and convertation between hex and decimal numbers
*/
#pragma once
#include"rijndael.h"
#include"osrng.h"
#include"modes.h"
#include "filters.h"
#include "cryptlib.h"
#include"sha.h"
#include "rsa.h"
#include"hex.h"
#include"../sql/Functools.hpp"
#include<utility>
#include<string>
#include<vector>
#include<iostream>

namespace Encryption
{
	using namespace CryptoPP;
	using namespace std;

	namespace AES
	{
		typedef pair<SecByteBlock, SecByteBlock> key_iv;

		key_iv get_random_key();
		string convert_bytes(const SecByteBlock& bytes);
		SecByteBlock convert_to_bytes(const string& key);
		SecByteBlock _convert_to_bytes(const string& key);

		string encrypt(const key_iv& key_iv, const string& text);
		string decrypt(const key_iv& key_iv, const string& cipher);
	};
	namespace SHA
	{
		string sha256(const string& data);
	};
	string str_to_hex(const string& str);
	string hex_to_str(const string& hex);
};