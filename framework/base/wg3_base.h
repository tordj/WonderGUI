
#pragma once

#include <wg_base.h>

namespace wg
{
	class Base
	{
	public:
		static char *		memStackAlloc(int bytes) { return WgBase::MemStackAlloc(bytes); }
		static void			memStackRelease(int bytes) { WgBase::MemStackRelease(bytes); }
        
        static int          pixelQuartersPerPoint() { return 4; }
        static float        scale() { return 1.f; }
	};
}
