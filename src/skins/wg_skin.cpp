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

	//____ minSize() ______________________________________________________________

	Size Skin::minSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ _preferredSize() ______________________________________________________________

	Size Skin::preferredSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ contentPadding() ______________________________________________________________

	Border Skin::contentPadding(State state) const
	{
		return Border(m_contentPadding).aligned();
	}

	//____ contentPaddingSize() ______________________________________________________________

	Size Skin::contentPaddingSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ contentOfs() ______________________________________________________________

	Coord Skin::contentOfs(State state) const
	{
		return Coord(m_contentPadding.left, m_contentPadding.top).aligned();
	}

	//____ sizeForContent() ___________________________________________________

	Size Skin::sizeForContent(const Size& contentSize) const
	{
		return contentSize + Size(Border(m_contentPadding).aligned());
	}

	//____ contentRect() ______________________________________________________

	Rect Skin::contentRect(const Rect& canvas, State state) const
	{
		return (canvas - Border(m_contentPadding).aligned()).aligned();
	}

	//____ isOpaque() _________________________________________________________

	bool Skin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	bool Skin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return m_bOpaque;
	}

	//____ isStateIdentical() ____________________________________________________

	bool Skin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return true;
	}

	//____ fractionChangeRect() ______________________________________

	Rect Skin::fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const
	{
		return Rect();
	}


} // namespace wg
