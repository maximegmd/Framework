#include <Crypt/RSA.h>

#include <cryptopp\files.h>
#include <cryptopp\rsa.h>
#include <cryptopp\osrng.h>
#include <cryptopp\hex.h>
#include <cryptopp\des.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace CryptoPP;
using namespace std;

namespace Framework{
	namespace Crypt
	{
		//---------------------------------------------------------------------
		static CryptoPP::RSA::PublicKey publicKey;
		static CryptoPP::RSA::PrivateKey privateKey;
		//---------------------------------------------------------------------
		void Load(const string& filename, BufferedTransformation& bt)
		{
			

			FileSource file(filename.c_str(), true /*pumpAll*/);

			file.TransferTo(bt);
			bt.MessageEnd();
		}
		//---------------------------------------------------------------------
		void LoadPrivateKey(const string& filename, PrivateKey& key)
		{
			if(!boost::filesystem::exists(filename)) return;

			ByteQueue queue;

			Load(filename, queue);
			key.Load(queue);	
		}
		//---------------------------------------------------------------------
		void LoadPublicKey(const string& filename, PublicKey& key)
		{
			if(!boost::filesystem::exists(filename)) return;

			ByteQueue queue;

			Load(filename, queue);
			key.Load(queue);	
		}
		//---------------------------------------------------------------------
		void Save(const std::string& filename, const CryptoPP::BufferedTransformation& bt)
		{
			CryptoPP::FileSink file(filename.c_str());

			bt.CopyTo(file);
			file.MessageEnd();
		}
		//---------------------------------------------------------------------
		void SavePrivateKey(const std::string& filename, const CryptoPP::PrivateKey& key)
		{
			CryptoPP::ByteQueue queue;
			key.Save(queue);

			Save(filename, queue);
		}
		//---------------------------------------------------------------------
		void SavePublicKey(const std::string& filename, const CryptoPP::PublicKey& key)
		{
			CryptoPP::ByteQueue queue;
			key.Save(queue);

			Save(filename, queue);
		}
		//---------------------------------------------------------------------
		void RSA::MakeKeys()
		{
			CryptoPP::AutoSeededRandomPool rng;

			CryptoPP::InvertibleRSAFunction params;
			params.GenerateRandomWithKeySize( rng, 2048 );

			CryptoPP::RSA::PrivateKey privateKey( params );
			CryptoPP::RSA::PublicKey publicKey( params );

			SavePrivateKey("private.key", privateKey);
			SavePublicKey("public.key", publicKey);
		}
		//---------------------------------------------------------------------
		void RSA::Init()
		{
			LoadPublicKey("public.key", publicKey);
			LoadPrivateKey("private.key", privateKey);
		}
		//---------------------------------------------------------------------
		std::string RSA::Encrypt(const std::string& pData)
		{
			std::string cpy = pData;
			CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
			CryptoPP::AutoSeededRandomPool rng;
			
			std::string result;
			for(size_t i = 0; i < pData.size(); i+= encryptor.FixedMaxPlaintextLength())
			{
				std::string ciphered;
				std::string toCrypt = cpy.substr(0, cpy.size() > encryptor.FixedMaxPlaintextLength() ? encryptor.FixedMaxPlaintextLength() : cpy.size());
				cpy.erase(0, toCrypt.size());

				CryptoPP::StringSource(toCrypt, true, new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::StringSink(ciphered)));
				result.append(ciphered);
			}
			return result;
		}
		//---------------------------------------------------------------------
		std::string RSA::Decrypt(const std::string& pData)
		{
			CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
			CryptoPP::AutoSeededRandomPool rng;
			std::vector<std::string> deciphered;
			for(size_t i = 0; i < pData.size(); i+= decryptor.FixedCiphertextLength())
			{
				std::string tmp;
				CryptoPP::StringSource(std::string((char*)&pData[i], decryptor.FixedCiphertextLength()), true, new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::StringSink(tmp)));
				deciphered.push_back(tmp);
			}
			std::string result;
			for(size_t i=0; i < deciphered.size(); i++)
			{
				result += deciphered.at(i);
			}

			return result;
		}
		//---------------------------------------------------------------------
	}
}