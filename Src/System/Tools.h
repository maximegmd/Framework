#pragma once

#include <string>
#include <vector>

using namespace std;


namespace Framework
{
	namespace System
	{
		void PrintBinary(unsigned char* data, int length, const std::string& pText = "");
		std::string GetTime();
		vector<string> Explode( const string &delimiter, const string &str);
		std::string RandomData(unsigned int pSize);
		std::string IntToString(unsigned int pValue);

	}
}