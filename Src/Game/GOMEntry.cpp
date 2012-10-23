#include "GOMEntry.hpp"

namespace Game
{
	void IGOMEntry::SetDirty(bool dirty)
	{
		if(dirty)
		{
			flags |= kDirty;
		}
		else
		{
			flags &= ~kDirty;
		}
	}

	bool IGOMEntry::IsDirty()
	{
		return flags & kDirty;
	}
}