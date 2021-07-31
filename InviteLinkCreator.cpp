#include "InviteLinkCreator.h"


std::string Blink::create_invite_link(const string& ip,
									  const string& port,
									  const string& key,
									  const string& iv,
									  const string& room,
									  const string& password)
{
	//link data will be encrypted
	json link;
	link["ip"]   = ip;
	link["port"] = port;
	link["room"] = Encryption::SHA::sha256(room);
	link["password"] = Encryption::SHA::sha256(password);

	auto curr_time   = chrono::system_clock::now();
	auto c_curr_time = chrono::system_clock::to_time_t(curr_time);
	auto time_str = ctime(&c_curr_time);
	link["time"] = Encryption::SHA::sha256(time_str);

	return link.dump();
}
std::string Blink::encrypt_invite_link(const string& link)
{

	namespace e = encr::rsa;

	//get RSA keys
	auto str_keys = e::get_random_keys();
	auto keys = e::spair_to_keys(str_keys);

	//get encrypted RSA chunks
	auto encrypted = e::encrypt(keys.second,link);

	//get random noise chunks
	const auto charset = get_ascii_charset();
	vector<string> noise_chunks;
	for (size_t i = 0; i < encrypted.size(); ++i)
	{
		string noise = generate_noise_string(get_random_int(20, 50), charset);
		noise_chunks.push_back(noise);
	}

	//unite cipher by adding random noise chunks
	string cipher;
	for (int i = 0; i < encrypted.size(); ++i)
	{
		string chunk = encrypted[i] + noise_chunks[i];
		cipher += chunk;
	}

	//add separator
	cipher += encr::str_to_hex(SEPARATOR);

	//add data about noise
	string noise_size_data;
	for (auto& ch : noise_chunks)
	{
		noise_size_data += to_string(ch.size());
	}
	cipher += noise_size_data;

	//XOR it
	string random_bits = to_binary(generate_noise_string(1, charset));

	string xored_cipher;
	for (auto ch : cipher)
	{
		string s; s += ch;
		s = to_binary(s);
		xored_cipher += from_binary(XOR(s, random_bits));
		s.clear();
	}
	xored_cipher += from_binary(random_bits);

	//compress it
	string compressed = inner::compress(xored_cipher);
	return compressed;
}


string Blink::inner::compress(const string& data)
{
	string compressed;

	using namespace CryptoPP;
	StringSource ss(data, true,
		new Gzip(
			new StringSink(compressed)
		));
	return compressed;
}
string Blink::inner::decompress(const string& data)
{
	string decompressed;

	using namespace CryptoPP;
	StringSource ss(data, true,
		new Gunzip(
			new StringSink(decompressed)
		));
	return decompressed;
}