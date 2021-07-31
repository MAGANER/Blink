#pragma once
#include<chrono>
#include<string>
#include"encryption/Encryption.h"
#include"json.hpp"
#include"sql/Functools.hpp"
#include"NoiseGenerator.hpp"
#include"BitProcessor.h"
#include"gzip.h"
#define SEPARATOR "du-deg00dwe-ed"

namespace Blink
{
using namespace std;
using namespace nlohmann;
namespace encr = Encryption;

string create_invite_link(const string& ip,
						  const string& port,
						  const string& key,
						  const string& iv,
						  const string& room,
						  const string& password);

string encrypt_invite_link(const string& link);
string decrypt_invite_link(const string& link);


namespace inner 
{
inline string get_const_noise1()
{
	const string noise = encr::str_to_hex("D7dz5Y$r6~gs%G'B,gj-uWF+LzZU{`K)+n~^@cz2Zkd^'&efZzB}Myd%y3f{T}SX%&$f%8;[dJgPJXD<[Zq5Q?@7R)H(8(?#vz[x_4da/C^V6NYU,(N4gY%DQ#}2RG6W@a7D6{tqtYxa_Wz6x4B]ce,tkut;=8z`J=4?FY_5<{qzFf,uq-Fey<n]`Le:FQHZ:\s24F@u[2LA%<]s3(XMF*F//DGBw5#L:;^F/qH@Nt~Wb^),Cx]f<a*ug6`WsjCy");
	return noise;
}
inline string get_const_noise2()
{
	const string noise = encr::str_to_hex("HH=hVgdPzL+S!=^!kN376PRyVLL%*Dq_9d64GmaC#ypa&Dt!#-F%V9kwKgYGPH-&TW&J7^3mur!8x3u%yJ$!%V?YA=tRfkS4ZVkN7@mp#QP68&VjUNhqYwBG_eAq2E5Prb@habY&N*N-kgu-2&P7fr7cH^9HmAXJVzXFPn-BxpCr9+!%5$f#dZjzjX_6cF*?mWwUV9v_JJ5UqT5RcA!m$cu9DeaN=EH=+3watBmz!yExCQvn6QhZUE#?PJ*+t=jq");
	return noise;
}
inline string get_const_noise3()
{
	const string noise = encr::str_to_hex("U&+DnPgv-*+zSyfW_B^9Q!$!AkH6pFADuPRU^3p9#F-y8X4LsGeR&N+WF6?dj6Mvt8u-qkL-P25^^dhdExUL@TXw4X45%Cy&cDuvBGmB*5dPMa3gRLdkt3+6VTp4T!pHCXv8Y@-QLntucaHjARgbb?RKXBzwnHuMV6ajW=L#s5-u=GS5-6YVHEwAVYsvPm+pHqw-e?EpRHcu@KBYj_^GZFBe4ej2mja^9UQTBpZszRQ^BudC_Mc_kymDwQKAFaES");
	return noise;
}

string compress(const string& data);
string decompress(const string& data);
};
};

