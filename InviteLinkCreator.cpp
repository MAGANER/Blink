#include "InviteLinkCreator.h"


std::string Blink::get_curr_time()
{
	auto curr_time = chrono::system_clock::now();
	auto c_curr_time = chrono::system_clock::to_time_t(curr_time);
	auto time_str = ctime(&c_curr_time);
	return time_str;
}
std::string Blink::create_invite_link(const string& ip,
									  const string& port,
									  const string& room,
									  const string& IV,
									  const string& KEY,
									  const string& password)
{
	//link data will be encrypted
	json link;
	link["ip"]   = ip;
	link["port"] = port;
	link["iv"]   = IV;
	link["key"]  = KEY;
	link["room"]     = Encryption::SHA::sha256(room);
	link["password"] = Encryption::SHA::sha256(password);

	link["time"] = Encryption::SHA::sha256(get_curr_time());

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
	cipher += SEPARATOR;

	//add data about noise
	string noise_size_data;
	for (auto& ch : noise_chunks)
	{
		noise_size_data += to_string(ch.size());
	}
	cipher += noise_size_data;

	//save data about key
	string key_size = to_string(str_keys.first.size());
	string key_data = key_size + str_keys.first;
	cipher = key_data + key_size + cipher;
	//

	cipher = encr::str_to_hex(cipher);

	//compress it
	string compressed = inner::compress(cipher);
	return compressed;
}
string Blink::decrypt_invite_link(const string& link)
{
	//decompress it
	string decompressed = inner::decompress(link);
	string cipher = encr::hex_to_str(decompressed);

	//get data about key
	int key_size = atoi(Functools::slice(cipher, 0, 4).c_str());
	string key = Functools::slice(cipher, 4, key_size+4);
	//exlude key data
	cipher = Functools::slice(cipher, key_size + 6, cipher.size());

	//find separator
	string sep = SEPARATOR;
	auto sep_pos = cipher.find(sep);

	//get noise data
	if (sep_pos == string::npos)
	{
		cout << "can not retrieve link!" << endl;
		return "";
	}
	size_t sep_end = sep_pos + sep.size();
	string noise_data = Functools::slice(cipher, sep_end, cipher.size());
	cipher = Functools::slice(cipher, 0, sep_pos);

	//get sizes of noise strings
	size_t noise_str_number = noise_data.size() / 2;
	vector<size_t> sizes;

	int begin = 0, end = 2;
	for (size_t i = 0; i < noise_str_number; ++i)
	{
		string val = Functools::slice(noise_data, begin, end);
		begin += 2;
		end   += 2;
		size_t size = atoi(val.c_str());
		sizes.push_back(size);
	}

	//get cipher chunks
	vector<string> chunks;
	const int chunk_size = 384;
	
	size_t junk_len = 0;
	const size_t dx = 2;
	int counter = 1;

	//take first chunk
	string first = Functools::slice(cipher, dx, chunk_size + dx);
	chunks.push_back(first);

	for (auto s : Functools::slice(sizes, 0, sizes.size()-1))
	{
		junk_len += s;
		int begin = dx + junk_len + counter       * chunk_size;
		int end   = dx + junk_len + (counter + 1) * chunk_size;
		string chunk = Functools::slice(cipher, begin, end);
		chunks.push_back(chunk);
		counter++;
	}
	//decrypt it
	auto _key = encr::rsa::str_to_priv_key(key);
	return encr::rsa::decrypt(_key, chunks);
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