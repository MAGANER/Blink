#include"Encryption.h"
using namespace Encryption;
Encryption::AES::key_iv Encryption::AES::get_random_key()
{
	AutoSeededRandomPool rnd;

	SecByteBlock key(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
	rnd.GenerateBlock(key, key.size());

	SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
	rnd.GenerateBlock(iv, iv.size());
	
	return make_pair(key, iv);
}

/////////////////
///**************
//these 2 functions were taken from my old project
//so i think it's better to keep it as is,
//but they could be better
string Encryption::AES::convert_bytes(const SecByteBlock& bytes)
{
	string curr = (const char*)bytes.data();
	string new_key;
	for (size_t i = 0;i<curr.size();++i)
	{
		auto ch = curr[i];
		int code = (int)ch;
		new_key += to_string(code)+'|';
	}
	return new_key;
}
CryptoPP::SecByteBlock Encryption::AES::convert_to_bytes(const string& key)
{
	string bytes;

	string curr_ch;
	for (auto& ch : key)
	{
		if (ch != '|')curr_ch += ch;
		else
		{
			bytes += (char)atoi(curr_ch.c_str());
			curr_ch.clear();
		}
	}

	SecByteBlock _key(reinterpret_cast<CryptoPP::byte*>(&bytes[0]), CryptoPP::AES::DEFAULT_KEYLENGTH);
	return _key;
}
///**************
/////////////////
CryptoPP::SecByteBlock Encryption::AES::_convert_to_bytes(const string& key)
{
	string bytes;

	string curr_ch;
	for (auto& ch : key)
	{
		if (ch != '|')curr_ch += ch;
		else
		{
			bytes += (char)atoi(curr_ch.c_str());
			curr_ch.clear();
		}
	}
	if (bytes.empty())bytes = curr_ch;

	SecByteBlock _key(reinterpret_cast<CryptoPP::byte*>(&bytes[0]), CryptoPP::AES::DEFAULT_KEYLENGTH);
	return _key;
}

std::string Encryption::AES::encrypt(const key_iv& key_iv, const string& text)
{
	if (text.empty())return "";
	try
	{
		CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV(key_iv.first, key_iv.first.size(), key_iv.second);

		string _text = text;
		//while ((_text.size() % 16) != 0)_text +=(char)0;

		string cipher;
		StringSource s(_text, true,
			//new HexEncoder(
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter
		//)
		); // StringSource

		return cipher;
	}
	catch (const Exception& e)
	{
		cout <<"AES::encryption:"<< e.what();
		exit(-1);
	}
}
std::string Encryption::AES::decrypt(const key_iv& key_iv, const string& cipher)
{
	if (cipher.empty())return "";
	try
	{
		CBC_Mode<CryptoPP::AES>::Decryption d;
		d.SetKeyWithIV(key_iv.first, key_iv.first.size(), key_iv.second);

		string recovered;
		StringSource s(cipher, true,
			//new CryptoPP::HexDecoder(
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		//)
		); // StringSource
		return recovered;
	}
	catch (const Exception& e)
	{
		cout << "AES::decryption:" << e.what();
		return "-1";
	}
}
std::string Encryption::SHA::sha256(const string& data)
{
	string result;

	SHA256 hash;
	hash.Update((const CryptoPP::byte*)data.data(), data.size());
	result.resize(hash.DigestSize());
	hash.Final((CryptoPP::byte*)&result[0]);

	string hex_result;
	HexEncoder encoder;
	encoder.Put((const CryptoPP::byte*)result.data(), result.size());
	encoder.MessageEnd();

	word64 size = encoder.MaxRetrievable();
	if (size)
	{
		hex_result.resize(size);
		encoder.Get((CryptoPP::byte*)&hex_result[0], hex_result.size());
	}
	return hex_result;
}
string Encryption::str_to_hex(const string& str)
{
	string result;
	StringSource ss(str, true,
		new HexEncoder(
			new StringSink(result)
		) // HexEncoder
	); // StringSource

	return result;
}
string Encryption::hex_to_str(const string& hex)
{
	string result;
	StringSource ss(hex, true,
		new HexDecoder(
			new StringSink(result)
		) // HexEncoder
	); // StringSource

	return result;
}