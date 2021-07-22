#include"Encryption.h"

Encryption::key_iv Encryption::get_random_key()
{
	AutoSeededRandomPool rnd;

	SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
	rnd.GenerateBlock(key, key.size());

	SecByteBlock iv(AES::BLOCKSIZE);
	rnd.GenerateBlock(iv, iv.size());
	
	return make_pair(key, iv);
}
Encryption::string Encryption::convert_bytes(const SecByteBlock& bytes)
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
Encryption::SecByteBlock Encryption::convert_to_bytes(const string& key)
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

	SecByteBlock _key(reinterpret_cast<CryptoPP::byte*>(&bytes[0]), AES::DEFAULT_KEYLENGTH);
	return _key;
}

std::string Encryption::encrypt(const key_iv& key_iv, const string& text)
{
	if (text.empty())return "";
	try
	{
		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key_iv.first, key_iv.first.size(), key_iv.second);

		string cipher;
		StringSource s(text, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter
		); // StringSource

		while ((cipher.size() % 16) != 0)cipher += (char)0;

		return cipher;
	}
	catch (const Exception& e)
	{
		cout << e.what();
	}
}

std::string Encryption::decrypt(const key_iv& key_iv, const string& cipher)
{
	if (cipher.empty())return "";
	try
	{
		CBC_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key_iv.first, key_iv.first.size(), key_iv.second);

		string recovered;
		StringSource s(cipher, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource
		return recovered;
	}
	catch (const Exception& e)
	{
		cout << e.what();
	}
}
std::string Encryption::sha256(const string& data)
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
