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

	//____ isOpaque() ______________________________________________________________

	bool StaticColorSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_color.a == 255 ? true : false;
	}

	//____ render() ______________________________________________________________

	void StaticColorSkin::render(GfxDevice * pDevice, const Rect& canvas, State state) const
	{
		pDevice->fill(canvas.px(), m_color);
	}

	//____ minSize() ______________________________________________________________

	Size StaticColorSkin::minSize() const
	{
		return Size();
	}

	//____ preferredSize() ______________________________________________________________

	Size StaticColorSkin::preferredSize() const
	{
		return Size();
	}

	//____ contentPadding() ______________________________________________________________

	Border StaticColorSkin::contentPadding() const
	{
		return Border();
	}


	//____ contentPaddingSize() ______________________________________________________________

	Size StaticColorSkin::contentPaddingSize() const
	{
		return Size();
	}

	//____ _contentOfs() ______________________________________________________________

	Coord StaticColorSkin::contentOfs( State state ) const
	{
		return Coord();
	}

	//____ _sizeForContent() ___________________________________________________

	Size StaticColorSkin::sizeForContent( const Size& contentSize ) const
	{
		return contentSize;
	}

	//____ contentRect() ______________________________________________________

	Rect StaticColorSkin::contentRect( const Rect& canvas, State state ) const
	{
		return canvas;
	}

	//____ markTest() _________________________________________________________

	bool StaticColorSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		return ( canvas.contains(ofs) && ((int)m_color.a) >= opacityTreshold );
	}


} // namespace wg
