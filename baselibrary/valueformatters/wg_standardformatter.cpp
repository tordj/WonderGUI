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

#include <wg_standardformatter.h>

namespace wg
{

	const TypeInfo StandardFormatter::TYPEINFO = { "StandardFormatter", &ValueFormatter::TYPEINFO };

	//____ constructor _____________________________________________________________

	StandardFormatter::StandardFormatter()
	{
	}

	StandardFormatter::StandardFormatter( const CharSeq& format )
	{
	}

	//____ Destructor ______________________________________________________________

	StandardFormatter::~StandardFormatter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StandardFormatter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ format() _______________________________________________________________

	String StandardFormatter::format( int64_t value, int scale ) const
	{
		//TODO: Implement!
		return String();
	}

	String StandardFormatter::format( double value ) const
	{
		//TODO: Implement!
		return String();
	}

} // namespace wg
