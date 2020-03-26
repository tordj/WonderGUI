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

#include <wg_textstylemanager.h>


namespace wg
{
	TextStyle ** TextStyleManager::s_pLookupTable = 0;		// Pointer at handle->pointer lookup table.
	int			TextStyleManager::s_capacity = 0;			// SizeI in entries for lookup table.
	int			TextStyleManager::s_size = 0;
	int			TextStyleManager::s_nextAvailable = -1;		// Offset in table for next available entry.

	//____ getPointer()_________________________________________________________

	TextStyle_p TextStyleManager::getPointer( TextStyle_h handle )
	{
		return s_pLookupTable[handle-1];
	}


	//____ init()_______________________________________________________________

	void TextStyleManager::init()
	{
		int capacity = 16;

		s_pLookupTable = new TextStyle*[capacity];
		s_capacity = capacity;
		s_nextAvailable = 0;

		for( int i = 0 ; i < capacity ; i++ )
			* (int*)(&s_pLookupTable[i]) = i+1;
	}

	//____ exit()_______________________________________________________________

	void TextStyleManager::exit()
	{
		//TODO: Something if s_size > 0 (there are still existing TextStyles)

		delete [] s_pLookupTable;

		s_pLookupTable = 0;
		s_capacity = 0;
		s_size = 0;
		s_nextAvailable = -1;
	}

	//____ _reserveHandle()_____________________________________________________

	uint16_t TextStyleManager::_reserveHandle(TextStyle * p)
	{
		if( s_size == s_capacity )
		{
			int newCapacity = s_capacity * 2;

			TextStyle ** pNewTable = new TextStyle*[newCapacity];

			for( int i = 0 ; i < s_capacity ; i++ )
				pNewTable[i] = s_pLookupTable[i];

			for( int i = s_capacity ; s_capacity < newCapacity-1 ; i++ )
				* (int*)(&pNewTable[i]) = i+1;

			* (int*)(&pNewTable[newCapacity-1]) = -1;

			delete [] s_pLookupTable;

			s_pLookupTable = pNewTable;
			s_nextAvailable = s_capacity;
			s_capacity = newCapacity;
		}

		int idx = s_nextAvailable;
		s_nextAvailable = * (int *)(&s_pLookupTable[idx]);
		s_pLookupTable[idx] = p;
		s_size++;


		return static_cast<TextStyle_h>(idx+1);
	}

	//____ _releaseHandle()_____________________________________________________

	void TextStyleManager::_releaseHandle( TextStyle_h handle )
	{
		int idx = handle - 1;

		* (int*)(&s_pLookupTable[idx]) = s_nextAvailable;
		s_nextAvailable = idx;
		s_size--;
	}


} //namespace wg
