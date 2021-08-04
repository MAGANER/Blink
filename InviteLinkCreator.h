#pragma once
#include<chrono>
#include<string>
#include"encryption/Encryption.h"
#include"json.hpp"
#include"sql/Functools.hpp"
#include"NoiseGenerator.hpp"
#include"BitProcessor.h"
#include"gzip.h"
#define SEPARATOR "+++"+encr::SHA::sha256("<du-deg00dwe-ed-ha----te-niggers-hail-hitler-fuck-jewish-scum-hail-white-race-1488-rahowa-fuck-you-assheadisyourrealnamesoiloveanalcuntuhategovernmentihateyouandfuckoffdouchebag>")+"---"
#define SEPARATOR1 "---"+encr::SHA::sha256("SoYESSSSSiwanna_____dri_haha_k beeer,man,iwanna kill you and burn entire worldcosssssssyousuckscskckscks333435---=098191&$^%303223-___@)@)(@(@(999929329(@(@9$(@$(@%@(%(@(@%@(@%(@(%(@(%@(*%8fhjf sefew fnew fewjfrewrw54-9305344tktk3 4tt3 4593 9 koage34ao04a39a[9 0[6j 6")+"+++"
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
						  const string& password);

string encrypt_invite_link(const string& link);
string decrypt_invite_link(const string& link);


namespace inner 
{
string compress(const string& data);
string decompress(const string& data);
};
};

