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

	const TypeInfo Skin::TYPEINFO = { "Skin", &Object::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Skin::typeInfo(void) const
	{
		return TYPEINFO;
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
