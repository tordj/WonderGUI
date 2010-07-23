/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/
#ifndef WG_CODEPAGE_DOT_H
#define WG_CODEPAGE_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgCodePages
{
public:
	static inline Uint16* GetCodePage( WgCodePage codePage ) { return LookupTable[codePage]; }
		
private:		
	static Uint16* LookupTable[WG_NB_CODEPAGES];
};

#endif //WG_CODEPAGE_DOT_H
