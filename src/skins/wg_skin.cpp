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
#include <wg_skin.h>
#include <wg_util.h>

namespace wg
{

	const char Skin::CLASSNAME[] = {"Skin"};

	//____ isInstanceOf() _________________________________________________________

	bool Skin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Object::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Skin::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	Skin_p Skin::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Skin_p( static_cast<Skin*>(pObject) );

		return 0;
	}

	//____ minSize() __________________________________________________________

	Size Skin::minSize() const
	{
		return Util::rawToQpix(_minSize());
	}

	//____ preferredSize() ____________________________________________________

	Size Skin::preferredSize() const
	{
		return Util::rawToQpix(_preferredSize());
	}

	//____ isOpaque() _________________________________________________________

	bool Skin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return _isOpaque(Util::qpixToRaw(rect), Util::qpixToRaw(canvasSize), state);
	}


} // namespace wg
