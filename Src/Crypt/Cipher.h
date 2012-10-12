#pragma once

#include <zlib\zlib.h>
#include <cryptopp\salsa.h>
#include <fstream>
#include <boost/noncopyable.hpp>
#include <cstdint>
#include <sstream>


namespace Framework
{
	namespace Crypt
	{
		class Cipher : private boost::noncopyable
		{
		public:

			/**
			 * Construct a cipher
			 * @param pEncryptKey The salsa encryption key
			 * @param pDecryptKey The salsa decryption key
			 * @param pEncryptIV The salsa encryption iv
			 * @param pDecryptIV The salsa encryption iv
			 */
			Cipher(const std::string& pEncryptKey, const std::string& pDecryptKey, const std::string& pEncryptIV, const std::string& pDecryptIV);

			~Cipher();
			/**
			 * Decrypt the data using the salsa20
			 * @param pData The data to decrypt
			 * @return The decrypted data
			 */
			std::string Decrypt(const std::string& pData);
			/**
			 * Encrypt the data using the salsa20
			 * @param pData The data to encrypt
			 * @return The encrypted data
			 */
			std::string Encrypt(const std::string& pData);
			/**
			 * Compress the data using the zlib
			 * @param pData The data to compress
			 * @return The compressed data
			 */
			std::string Compress  (const std::string& pData);
			/**
			 * Decompress the data using the zlib
			 * @param pData The data to decompress
			 * @return The decompressed data
			 */
			std::string Decompress(const std::string& pData);

		private:

			CryptoPP::Salsa20::Decryption mDecryptor;
			CryptoPP::Salsa20::Encryption mEncryptor;

			z_stream					  mInStream;
			z_stream					  mOutStream;
		};
	}
}