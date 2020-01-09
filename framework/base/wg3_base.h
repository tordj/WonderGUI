
#pragma once

#include <wg_base.h>

namespace wg
{
	class Base
	{
	public:
		static char *		memStackAlloc(int bytes) { return WgBase::MemStackAlloc(bytes); }
		static void			memStackRelease(int bytes) { WgBase::MemStackRelease(bytes); }

        static void         handleError( ErrorCode code, const char * pMsg, const Object * pObject, const char * pClass, const char * pFunction, const char * pFile, int line )
                            { WgBase::handleError(code, pMsg, pObject, pClass, pFunction, pFile, line); }

        static int          pixelQuartersPerPoint() { return 4; }
        static float        scale() { return 1.f; }
	};
}
