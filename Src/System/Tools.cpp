#include <System/Tools.h>
#include <System/Log.h>
#include <cstdio>
#include <sstream>
#include <random>
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>

using namespace std;

namespace Framework
{
	namespace System
	{
		//---------------------------------------------------------------------
		void PrintBinary(unsigned char* data, int length, const std::string& pText)
		{
			std::ostringstream os;
			os << pText << "\n";
			size_t len = (length / 16) * 16 + 16;
			for (size_t i = 0; i < len; ++i)
			{
				if(i < length)
				{
					char buf[200];
					sprintf(buf, "%02X", data[i]);
					os << buf[0] << buf[1] << ' ';
				}
				else
				{
					os << "   ";
				}

				if((i + 1) % 16 == 0)
				{
					os << " | ";
					for(size_t j = i - 15; j <= i && j < length ; ++j)
					{
						if(data[j] > 31 && data[j] < 127)
							os << data[j];
						else
							os << ".";
					}

					os << std::endl;
				}
				else if((i + 1) % 8 == 0)
					os << "  ";
			}
			os << "\n";

			System::Log::Debug(os.str());
		}
		//---------------------------------------------------------------------
		std::string GetTime()
		{
			time_t		datex;
			struct tm*	tmx;
			static const char*		months[12]	= { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

			time(&datex);
			tmx = localtime(&datex);

			std::string data;
			data.resize(512);

			auto size = sprintf(&data[0],"\"%3s-%02d-%4d %02d%%3a%02d%%3a%02d UTC\"",
				months[tmx->tm_mon], tmx->tm_mday, (1900+tmx->tm_year),
				tmx->tm_hour, tmx->tm_min, tmx->tm_sec);
			return data.substr(0, size);
		}
		//---------------------------------------------------------------------
		vector<string> Explode( const string &delimiter, const string &str)
		{
			vector<string> arr;

			int strleng = str.length();
			int delleng = delimiter.length();
			if (delleng==0)
				return arr;//no change

			int i=0;
			int k=0;
			while( i<strleng )
			{
				int j=0;
				while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
					j++;
				if (j==delleng)//found delimiter
				{
					arr.push_back(  str.substr(k, i-k) );
					i+=delleng;
					k=i;
				}
				else
				{
					i++;
				}
			}
			arr.push_back(  str.substr(k, i-k) );
			return arr;
		}
		//---------------------------------------------------------------------
		std::string RandomData(unsigned int pSize)
		{
			std::string output;

			std::normal_distribution<long double> distribution;
			std::mt19937 engine(time(0) + rand());

			while(output.size() < pSize)
			{
				std::string data;
				CryptoPP::MD5 hash;

				byte digest[ CryptoPP::MD5::DIGESTSIZE ];
				std::string message = std::to_string(distribution(engine));

				hash.CalculateDigest( digest, (const byte*)message.data(), message.length() );
				CryptoPP::HexEncoder encoder;

				encoder.Attach( new CryptoPP::StringSink( data ) );
				encoder.Put( digest, sizeof(digest) );
				encoder.MessageEnd();

				output += data;
			}
			return output.substr(0, pSize);
		}
		//---------------------------------------------------------------------
		std::string IntToString(unsigned int pValue)
		{
			std::ostringstream os;
			os << (char)((pValue >> 24) & 0xFF);
			os << (char)((pValue >> 16) & 0xFF);
			os << (char)((pValue >> 8) & 0xFF);
			os << (char)(pValue & 0xFF);

			return os.str();
		}
		//---------------------------------------------------------------------
	}
}