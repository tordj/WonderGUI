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

#include <wg_colorskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>

const char WgColorSkin::CLASSNAME[] = {"ColorSkin"};


//____ Create() _______________________________________________________________

WgColorSkinPtr WgColorSkin::Create( WgColor col )
{
	return WgColorSkinPtr(new WgColorSkin(col));
}

//____ Constructor ____________________________________________________________

WgColorSkin::WgColorSkin( WgColor col )
{
	m_color = col;
}

//____ IsInstanceOf() _________________________________________________________

bool WgColorSkin::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSkin::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgColorSkin::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgColorSkinPtr WgColorSkin::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgColorSkinPtr( static_cast<WgColorSkin*>(pObject.GetRealPtr()) );

	return 0;
}


	
void WgColorSkin::Render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const
{
	pDevice->Fill( _clip, m_color );
}
	
bool WgColorSkin::IsOpaque() const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::IsOpaque(WgState state) const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const
{
	return m_color.a == 255 ? true : false;
}

WgSize WgColorSkin::MinSize() const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::PreferredSize() const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::ContentPadding() const
{
	return WgSize(0,0);
}

WgSize  WgColorSkin::SizeForContent( const WgSize contentSize ) const
{
	return contentSize;
}

WgRect  WgColorSkin::ContentRect( const WgRect& canvas, WgState state ) const
{
	return canvas;
}

bool WgColorSkin::MarkTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const
{
	return ( canvas.Contains(ofs) && ((int)m_color.a) >= opacityTreshold );
}

bool WgColorSkin::IsStateIdentical( WgState state, WgState comparedTo ) const
{
	return true;
}
