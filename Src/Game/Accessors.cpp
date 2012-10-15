#include "Accessors.hpp"

namespace Game
{
	void IAccessor::SetDirty(bool dirty){
		if(dirty){
			flags |= kDirty;
			Synchronize();
		}
		else flags &= ~kDirty;
	}

	bool IAccessor::IsDirty(){
		return flags & kDirty;
	}
}