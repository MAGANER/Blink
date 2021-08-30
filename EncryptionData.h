#pragma once
#include<string>
#include<utility>
#include"encryption/Encryption.h"
namespace Blink
{
    using namespace std;
    namespace encr = Encryption;
    struct EncryptionData
    {
        EncryptionData(){}
        EncryptionData(const string& key,
            const string& iv)
        {
            data = make_pair(encr::AES::convert_to_bytes(key),
                             encr::AES::convert_to_bytes(iv));
        }
        ~EncryptionData() {}
        encr::AES::key_iv data;
    };
};