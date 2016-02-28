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

#ifndef WG_TEXTSTYLEMANAGER_DOT_H
#define WG_TEXTSTYLEMANAGER_DOT_H


#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

namespace wg
{
	class TextStyleManager
	{
	friend class Base;
	friend class TextStyle;
	friend class Char;
	friend class CharBuffer;
	friend class TextTool;
	public:
	
		static TextStyle_p 	getPointer( TextStyle_h handle );
		static inline int	getNbStyles() { return s_size; }
		
	protected:
//		TextStyleManager();
//		~TextStyleManager();
		
		static void init();
		static void exit();

		static TextStyle_h			_reserveHandle();
		static void					_releaseHandle( TextStyle_h handle );
		static inline TextStyle * 	_getPointer( TextStyle_h handle ) { return s_pLookupTable[handle-1]; }

		static TextStyle **	s_pLookupTable;			// Pointer at handle->pointer lookup table.		
		static int			s_capacity;				// Size in entries for lookup table.
		static int			s_size;					// Size in entries for lookup table.
		static int			s_nextAvailable;		// Offset in table for next available entry.
	};
}

#endif // WG_TEXTSTYLEMANAGER_DOT_H
