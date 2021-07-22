#pragma once
#include"rijndael.h"
#include"osrng.h"
#include"modes.h"
#include "filters.h"
#include "cryptlib.h"
#include"sha.h"
#include"hex.h"
#include<utility>
#include<string>

namespace Encryption
{
	using namespace CryptoPP;
	using namespace std;
	typedef pair<SecByteBlock, SecByteBlock> key_iv;

	key_iv get_random_key();
	string convert_bytes(const SecByteBlock& bytes);
	SecByteBlock convert_to_bytes(const string& key);

	string encrypt(const key_iv& key_iv, const string& text);
	string decrypt(const key_iv& key_iv, const string& cipher);

	string sha256(const string& data);

};