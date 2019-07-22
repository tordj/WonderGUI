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

namespace wg
{

	const char StaticColorSkin::CLASSNAME[] = {"StaticColorSkin"};


	//____ create() _______________________________________________________________

	StaticColorSkin_p StaticColorSkin::create( Color col )
	{
		return StaticColorSkin_p(new StaticColorSkin(col));
	}

	//____ Constructor ____________________________________________________________

	StaticColorSkin::StaticColorSkin( Color col )
	{
		m_color = col;
	}

	//____ isInstanceOf() _________________________________________________________

	bool StaticColorSkin::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Skin::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StaticColorSkin::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	StaticColorSkin_p StaticColorSkin::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StaticColorSkin_p( static_cast<StaticColorSkin*>(pObject) );

		return 0;
	}

	//____ render() ______________________________________________________________

	void StaticColorSkin::render( GfxDevice * pDevice, const Rect& canvas, State state ) const
	{
		pDevice->fill( canvas, m_color );
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

	bool StaticColorSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_color.a == 255 ? true : false;
	}

	//____ minSize() ______________________________________________________________

	SizeQ StaticColorSkin::minSize() const
	{
		return SizeQ();
	}

	//____ preferredSize() ______________________________________________________________

	SizeQ StaticColorSkin::preferredSize() const
	{
		return SizeQ();
	}

	//____ contentPadding() ______________________________________________________________

	SizeQ StaticColorSkin::contentPadding() const
	{
		return SizeQ();
	}

	//____ contentOfs() ______________________________________________________________

	CoordQ StaticColorSkin::contentOfs( State state ) const
	{
		return CoordQ();
	}

	//____ sizeForContent() ___________________________________________________

	SizeQ  StaticColorSkin::sizeForContent( const SizeQ contentSize ) const
	{
		return contentSize;
	}

	//____ contentRect() ______________________________________________________

	RectQ  StaticColorSkin::contentRect( const RectQ& canvas, State state ) const
	{
		return canvas;
	}

	//____ markTest() _________________________________________________________

	bool StaticColorSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		return ( canvas.contains(ofs) && ((int)m_color.a) >= opacityTreshold );
	}

	//____ isStateIdentical() _________________________________________________

	bool StaticColorSkin::isStateIdentical( State state, State comparedTo ) const
	{
		return true;
	}

} // namespace wg
