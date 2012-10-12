#pragma once

#include <string>

namespace Framework
{
	namespace Crypt
	{
		class RSA
		{
		public:

			/**
			 * Decrypt RSA data
			 * @param pBuffer The buffer to decrypt
			 * @return The decrypted data
			 */
			static void Init();
			static std::string Decrypt(const std::string& pBuffer);
			static std::string Encrypt(const std::string& pBuffer);
			static void		   MakeKeys();

		};
	}
}