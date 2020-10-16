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
#include <wg_base.h>

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

	//____ setContentPadding() ____________________________________________________

	void Skin::setContentPadding(const BorderI& padding)
	{
		m_contentPadding = padding;
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

	//____ dirtyRect() ________________________________________________________

	Rect Skin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos, float* pNewStateFractions,
		float* pOldStateFractions) const
	{
		return Rect();
	}

	//____ animationLength() ____________________________________________

	int Skin::animationLength(State state) const
	{
		return 0;
	}

	//____ transitioningStates() __________________________________________________

	Bitmask<uint8_t> Skin::transitioningStates() const
	{
		return Bitmask<uint8_t>(0);
	}

	//____ transitionTimes() ____________________________________________

	const int* Skin::transitionTimes() const
	{
		const int	transitionTimes[StateBits_Nb] = { 0,0,0,0,0,0 };

		return transitionTimes;
	}


} // namespace wg
