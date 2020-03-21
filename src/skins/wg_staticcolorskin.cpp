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

#include <wg_staticcolorskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	const TypeInfo StaticColorSkin::TYPEINFO = { "StaticColorSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticColorSkin_p StaticColorSkin::create( Color col )
	{
		return StaticColorSkin_p(new StaticColorSkin(col));
	}

	//____ constructor ____________________________________________________________

	StaticColorSkin::StaticColorSkin( Color col )
	{
		m_color = col;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticColorSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isStateIdentical() _________________________________________________

	bool StaticColorSkin::isStateIdentical(State state, State comparedTo) const
	{
		return true;
	}

	//____ isOpaque() ______________________________________________________________

	bool StaticColorSkin::isOpaque() const
	{
		return m_color.a == 255 ? true : false;
	}

	bool StaticColorSkin::isOpaque(State state) const
	{
		return m_color.a == 255 ? true : false;
	}

	//____ _isOpaque() ______________________________________________________________

	bool StaticColorSkin::_isOpaque( const RectI& rect, const SizeI& canvasSize, State state ) const
	{
		return m_color.a == 255 ? true : false;
	}

	//____ _render() ______________________________________________________________

	void StaticColorSkin::_render(GfxDevice * pDevice, const RectI& canvas, State state) const
	{
		pDevice->fill(rawToPixels(canvas), m_color);
	}

	//____ _minSize() ______________________________________________________________

	SizeI StaticColorSkin::_minSize() const
	{
		return SizeI();
	}

	//____ _preferredSize() ______________________________________________________________

	SizeI StaticColorSkin::_preferredSize() const
	{
		return SizeI();
	}

	//____ _contentPadding() ______________________________________________________________

	BorderI StaticColorSkin::_contentPadding() const
	{
		return BorderI();
	}


	//____ _contentPaddingSize() ______________________________________________________________

	SizeI StaticColorSkin::_contentPaddingSize() const
	{
		return SizeI();
	}

	//____ _contentOfs() ______________________________________________________________

	CoordI StaticColorSkin::_contentOfs( State state ) const
	{
		return CoordI();
	}

	//____ _sizeForContent() ___________________________________________________

	SizeI StaticColorSkin::_sizeForContent( const SizeI contentSize ) const
	{
		return contentSize;
	}

	//____ _contentRect() ______________________________________________________

	RectI StaticColorSkin::_contentRect( const RectI& canvas, State state ) const
	{
		return canvas;
	}

	//____ _markTest() _________________________________________________________

	bool StaticColorSkin::_markTest( const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold ) const
	{
		return ( canvas.contains(ofs) && ((int)m_color.a) >= opacityTreshold );
	}


} // namespace wg
