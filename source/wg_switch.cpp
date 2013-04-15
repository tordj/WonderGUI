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



#include <wg_switch.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"Switch"};

//____ Constructor ____________________________________________________________

WgSwitch::WgSwitch()
{
	m_bOpaque = false;
}

//____ Destructor _____________________________________________________________

WgSwitch::~WgSwitch()
{
}

//____ Type() _________________________________________________________________

const char * WgSwitch::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgSwitch::GetClass()
{
	return c_widgetType;
}

//____ SetValue() _____________________________________________________________

void WgSwitch::SetValue( int value )
{
    //TODO: Implement!
}

//____ PreferredSize() __________________________________________________________

WgSize WgSwitch::PreferredSize() const
{
	return WgSize(40,40);
}


//____ _onCloneContent() _______________________________________________________

void WgSwitch::_onCloneContent( const WgWidget * _pOrg )
{
	const WgSwitch * pOrg = static_cast<const WgSwitch*>(_pOrg);
}

//____ _onRender() _____________________________________________________________

void WgSwitch::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	int sz = WgMin( _canvas.w,_canvas.h );

	if( sz > 12 )
	{
		int itemSize = sz/4;
		int stepping = itemSize/2;

		int y = _canvas.y;
		int x = _canvas.x + (sz - itemSize) / 2;
		for( int i = 0 ; i < 3 ; i++ )
		{
			pDevice->ClipDrawElipse( _clip, WgRect(x,y,itemSize,itemSize), WgColor::white );
			y += itemSize + stepping;
		}
	}	
}

//____ _onAlphaTest() ___________________________________________________________

bool WgSwitch::_onAlphaTest( const WgCoord& ofs )
{
	return false;
}


//____ _onEnable() _____________________________________________________________

void WgSwitch::_onEnable()
{
    _requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgSwitch::_onDisable()
{
    _requestRender();
}




