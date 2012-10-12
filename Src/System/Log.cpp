#include "Log.h"
#include <ctime>

namespace Framework { namespace System
{
	Log* Log::mInstance = nullptr;

	Log::Log(const std::string& pName)
		:mLevel(VERBOSE)
	{
		mLog.open(pName, std::ios::trunc);
		mBuffer.reserve(8192); // Reserve 8KB at first
	}

	void Log::Create(const std::string& pFileName)
	{
		if(Log::mInstance == nullptr)
		{
			Log::mInstance = new Log(pFileName);
		}
	}

	Log* Log::GetInstance()
	{
		if(Log::mInstance == nullptr)
		{
			Log::mInstance = new Log();
		}
		return mInstance;
	}

	void Log::DoPrint(const char* pMessage, size_t pLength)
	{
		if(mLevel < LOW) // Print is LOW or greater
			return;
		boost::mutex::scoped_lock lock(mLock);

		AppendTime();
		AppendLine(pMessage, pLength);
	}

	void Log::DoDebug(const char* pMessage, size_t pLength)
	{
		static const char prefix[] = "[DEBUG] ";

		if(mLevel < VERBOSE) // Debug is VERBOSE or greater
			return;

		boost::mutex::scoped_lock lock(mLock);

		AppendTime();
		AppendData(prefix, sizeof(prefix));
		AppendLine(pMessage, pLength);
	}

	void Log::DoError(const char* pMessage, size_t pLength)
	{
		static const char prefix[] = "[ERROR] ";

		if(mLevel < LOW) // error is LOW or greater
			return;
		boost::mutex::scoped_lock lock(mLock);

		AppendTime();
		AppendData(prefix, sizeof(prefix));
		AppendLine(pMessage, pLength);
	}

	void Log::AppendLine(const char* pData, size_t pLength)
	{
		mBuffer.insert(mBuffer.end(), pData, pData + pLength);
		mBuffer.push_back('\n');
	}

	void Log::AppendData(const char* pData, size_t pLength)
	{
		mBuffer.insert(mBuffer.end(), pData, pData + pLength);
	}

	void Log::AppendTime()
	{
		char buffer[24];
		time_t t = time(0);
		strftime(buffer, 24, "%Y-%m-%d %H:%M:%S ", localtime(&t));
		mBuffer.insert(mBuffer.end(), buffer, buffer+20);
	}

	void Log::DoSetLevel(Level pLevel)
	{
		boost::mutex::scoped_lock lock(mLock);
		mLevel = pLevel;
	}

	void Log::DoFlush()
	{
		boost::mutex::scoped_lock lock(mLock);

		mLog.write(mBuffer.data(), mBuffer.size());
		cout.write(mBuffer.data(), mBuffer.size());
		mBuffer.clear();
		mLog.flush();
	}
}
}