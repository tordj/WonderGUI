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

	using namespace Util;

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Skin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setContentPadding() ____________________________________________________

	void Skin::setContentPadding(const Border& padding)
	{
		m_contentPadding = padding;
	}

	//____ contentPadding() ______________________________________________________________

	Border Skin::contentPadding() const
	{
		return m_contentPadding;
	}

	//____ setLayer() _________________________________________________________

	void Skin::setLayer(int layer)
	{
		m_layer = layer;
	}

	//____ _minSize() ______________________________________________________________

	SizeSPX Skin::_minSize(int scale) const
	{
		return SizeSPX(align(ptsToSpx(m_contentPadding,scale)));
	}

	//____ _preferredSize() ______________________________________________________________

	SizeSPX Skin::_preferredSize(int scale) const
	{
		return SizeSPX(align(ptsToSpx(m_contentPadding, scale)));
	}

	//____ _contentPaddingSize() ______________________________________________________________

	SizeSPX Skin::_contentPaddingSize(int scale) const
	{
		return SizeSPX(align(ptsToSpx(m_contentPadding, scale)));
	}

	//____ _contentOfs() ______________________________________________________________

	CoordSPX Skin::_contentOfs(int scale, State state) const
	{
		return align(ptsToSpx(Coord(m_contentPadding.left, m_contentPadding.top), scale));
	}

	//____ _sizeForContent() ___________________________________________________

	SizeSPX Skin::_sizeForContent(const SizeSPX& contentSize, int scale) const
	{
		return contentSize + SizeSPX(align(ptsToSpx(m_contentPadding, scale)));
	}

	//____ _contentRect() ______________________________________________________

	RectSPX Skin::_contentRect(const RectSPX& canvas, int scale, State state) const
	{
		return canvas - align(ptsToSpx(m_contentPadding, scale));
	}

	//____ _isOpaque() ________________________________________________________

	bool Skin::_isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
	{
		return m_bOpaque;
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX Skin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos, float* pNewStateFractions,
		float* pOldStateFractions) const
	{
		return RectSPX();
	}

	//____ _animationLength() ____________________________________________

	int Skin::_animationLength(State state) const
	{
		return 0;
	}

	//____ _transitioningStates() __________________________________________________

	Bitmask<uint8_t> Skin::_transitioningStates() const
	{
		return Bitmask<uint8_t>(0);
	}

	//____ _transitionTimes() ____________________________________________

	const int* Skin::_transitionTimes() const
	{
		const static int	transitionTimes[StateBits_Nb] = { 0,0,0,0,0,0 };

		return transitionTimes;
	}


} // namespace wg
