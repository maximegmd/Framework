#include <Crypt/Cipher.h>
#include <System/Tools.h>

namespace Framework
{
	namespace Crypt
	{
		//---------------------------------------------------------------------
		Cipher::Cipher(const std::string& pEncryptKey, const std::string& pDecryptKey, const std::string& pEncryptIV, const std::string& pDecryptIV)
			:mEncryptor((byte*)pEncryptKey.data(), 0x20, (byte*)pEncryptIV.data()),
			 mDecryptor((byte*)pDecryptKey.data(), 0x20, (byte*)pDecryptIV.data())
		{
			mOutStream.zalloc = Z_NULL;
			mOutStream.zfree = Z_NULL;
			mOutStream.opaque = Z_NULL;
			deflateInit2(&mOutStream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);

			mInStream.zalloc = Z_NULL;
			mInStream.zfree = Z_NULL;
			mInStream.opaque = Z_NULL;
			inflateInit2(&mInStream, -15);
		}
		Cipher::~Cipher()
		{
			inflateEnd(&mInStream);
			deflateEnd(&mOutStream);
		}
		//---------------------------------------------------------------------
		std::string Cipher::Decrypt(const std::string& pData)
		{
			std::string data(pData.size(), 0);
			mDecryptor.ProcessData((byte*)&data[0],(byte*)&pData[0], pData.size());
			return data;
		}
		//---------------------------------------------------------------------
		std::string Cipher::Encrypt(const std::string& pData)
		{
			std::string data(pData.size(), 0);
			mEncryptor.ProcessData((byte*)&data[0],(byte*)&pData[0], pData.size());
			return data;
		}
		//---------------------------------------------------------------------
		std::string Cipher::Compress  (const std::string& pData)
		{
			std::string data;

			mOutStream.avail_in = (uint32_t)pData.size();
			mOutStream.next_in = (Bytef*)&pData[0];

			do
			{
				Bytef chunk[32768];
				mOutStream.avail_out = 32768;
				mOutStream.next_out = chunk;

				int ret = deflate(&mOutStream, Z_SYNC_FLUSH);

				if(mOutStream.msg)
					printf(mOutStream.msg);

				data.append((char*)chunk, 32768 - mOutStream.avail_out);
			} while (mOutStream.avail_out == 0);

			data.erase(data.size() - 4);

			return data;
		}
		//---------------------------------------------------------------------
		std::string Cipher::Decompress(const std::string& pData)
		{
			std::string data;

			std::string inbound;
			inbound.append(pData.data(), pData.size());

			inbound.push_back(0x00);
			inbound.push_back(0x00);
			inbound.push_back((unsigned char)0xFF);
			inbound.push_back((unsigned char)0xFF);

			mInStream.avail_in = (uint32_t)inbound.size();
			mInStream.next_in = (Bytef*)&inbound[0];

			do
			{
				Bytef chunk[32768] = {0};

				mInStream.avail_out = 32768;
				mInStream.next_out = chunk;

				int ret = inflate(&mInStream, Z_SYNC_FLUSH);
				if(ret != 0)
				{
					std::ostringstream os;
					os << "Inflate error, " << ret << " msg: " << mInStream.msg << "\n";
					System::PrintBinary((unsigned char*)&inbound[0], (int)inbound.size(), os.str());

					system("pause");

					return "";
				}

				data.append((char*)chunk, 32768 - mInStream.avail_out);
			}
			while(mInStream.avail_in != 0 || mInStream.avail_out == 0);

			return data;
		}
		//---------------------------------------------------------------------
	}
}