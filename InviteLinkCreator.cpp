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
									  const string& password,
		
									  bool decentralysed)
{
	//link data will be encrypted
	json link;
	link["ip"]   = ip;
	link["port"] = port;
	link["iv"]   = IV;
	link["key"]  = KEY;
	link["room"]     = Encryption::SHA::sha256(room);
	link["password"] = Encryption::SHA::sha256(password);
	link["decentralysed"] = (decentralysed ? "1" : "0");

	link["time"] = get_curr_time();

	return link.dump();
}
std::string Blink::encrypt_invite_link(const string& link, const string& recepient_name)
{
	namespace e = encr::AES;
	
	auto key = e::convert_to_bytes(encr::SHA::sha256(recepient_name));
	auto iv = e::convert_to_bytes(encr::SHA::sha256(to_string(recepient_name.size())));

	auto key_iv = make_pair(key, iv);
	auto cipher = e::encrypt(key_iv, link);

	return cipher;
}
string Blink::decrypt_invite_link(const string& link, const string& my_name)
{
	namespace e = encr::AES;

	auto key = e::convert_to_bytes(encr::SHA::sha256(my_name));
	auto iv = e::convert_to_bytes(encr::SHA::sha256(to_string(my_name.size())));

	auto key_iv = make_pair(key, iv);

	auto decipher = e::decrypt(key_iv, link);
	return decipher;
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