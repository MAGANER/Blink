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

std::string Encryption::AES::encrypt(const key_iv& key_iv, const string& text)
{
	if (text.empty())return "";
	try
	{
		CBC_Mode<CryptoPP::AES>::Encryption e;
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
std::string Encryption::AES::decrypt(const key_iv& key_iv, const string& cipher)
{
	if (cipher.empty())return "";
	try
	{
		CBC_Mode<CryptoPP::AES>::Decryption d;
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
rsa::spair rsa::get_random_keys()
{
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	RSA::PrivateKey private_key(params);
	RSA::PublicKey public_key(params);
	
	ByteQueue priv_key_queue, pub_key_queue;
	private_key.Save(priv_key_queue);
	public_key.Save(pub_key_queue);

	string priv_key, pub_key;

	StringSink priv_key_sink(priv_key);
	priv_key_queue.CopyTo(priv_key_sink);
	priv_key_sink.MessageEnd();

	StringSink pub_key_sink(pub_key);
	pub_key_queue.CopyTo(pub_key_sink);
	pub_key_sink.MessageEnd();

	string encoded_priv_key;
	StringSource priv_key_ss(priv_key, true,
		new HexEncoder(
			new StringSink(encoded_priv_key)
		) 
	); 

	string encoded_pub_key;
	StringSource pub_key_ss(pub_key, true,
		new HexEncoder(
			new StringSink(encoded_pub_key))
	);

	return make_pair(encoded_priv_key, encoded_pub_key);
}
rsa::keys rsa::spair_to_keys(const rsa::spair& k)
{
	string decoded_priv_key, decoded_pub_key;
	StringSource ss_priv_key(k.first, true,
		new HexDecoder(
			new StringSink(decoded_priv_key)
		) 
	); 
	StringSource ss_pub_key(k.second, true,
		new HexDecoder(
			new StringSink(decoded_pub_key)
		)
	);

	StringSource ss_pub(decoded_pub_key, true);
	StringSource ss_priv(decoded_priv_key, true);

	RSA::PrivateKey pkey;
	pkey.Load(ss_priv);

	RSA::PublicKey pukey;
	pukey.Load(ss_pub);

	return make_pair(pkey,pukey);
}
string rsa::encrypt(const RSA::PublicKey& key,
				    const string& data)
{
	RSAES_OAEP_SHA_Encryptor e(key);
	AutoSeededRandomPool rng;

	string cipher;
	StringSource ss1(data, true,
		new PK_EncryptorFilter(rng, e,
			new StringSink(cipher)
		) // PK_EncryptorFilter
	); // StringSource
	return cipher;
}
string rsa::decrypt(const RSA::PrivateKey& key,
					const string& cipher)
{
	RSAES_OAEP_SHA_Decryptor d(key);
	AutoSeededRandomPool rng;
	string decoded;

	StringSource ss(cipher, true,
		new PK_DecryptorFilter(rng, d,
			new StringSink(decoded)
		) // PK_DecryptorFilter
	); // StringSource
	return decoded;
}