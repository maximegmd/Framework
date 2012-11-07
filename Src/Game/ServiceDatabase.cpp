#include <Game/ServiceDatabase.hpp>

namespace Game
{
	ServiceDatabase::ServiceDatabase()
	{

	}

	ServiceDatabase::~ServiceDatabase()
	{

	}

	void ServiceDatabase::AddService(IService* service)
	{
		mServices.push_back(std::shared_ptr<IService>(service));
	}

	void ServiceDatabase::Update()
	{
		for(auto itor = mServices.begin(), end = mServices.end(); itor != end; ++itor)
		{

		}
	}
}