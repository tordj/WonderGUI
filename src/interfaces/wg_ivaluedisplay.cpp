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

#include <wg_ivaluedisplay.h>

namespace wg
{

	//____ clear() _________________________________________________________________

	void IValueDisplay::clear()
	{
		bool bModified = _comp()->value() != 0;
		_comp()->clear();

		if( bModified )
			_comp()->onValueModified();
	}

	//____ set() ___________________________________________________________________

	bool IValueDisplay::set( int64_t value, int scale )
	{
		if( _comp()->set(value,scale) )
		{
			_comp()->onValueModified();
			return true;
		}
		else
			return false;

	}

	//____ setRange() ______________________________________________________________

	bool IValueDisplay::setRange( int64_t min, int64_t max )
	{
		int64_t val = _comp()->value();

		bool retVal = _comp()->setRange(min,max);
		if( val != _comp()->value() )
			_comp()->onValueModified();

		return retVal;
	}


} // namespace wg
