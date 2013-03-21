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



#include <wg_knob.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"Knob"};

//____ Constructor ____________________________________________________________

WgKnob::WgKnob()
{
	m_bOpaque = false;
}

//____ Destructor _____________________________________________________________

WgKnob::~WgKnob()
{
}

//____ Type() _________________________________________________________________

const char * WgKnob::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgKnob::GetClass()
{
	return c_widgetType;
}

//____ SetValue() _____________________________________________________________

void WgKnob::SetValue( float value )
{
    //TODO: Implement!
}

//____ PreferredSize() __________________________________________________________

WgSize WgKnob::PreferredSize() const
{
	return WgSize(32,32);
}


//____ _onCloneContent() _______________________________________________________

void WgKnob::_onCloneContent( const WgWidget * _pOrg )
{
	const WgKnob * pOrg = static_cast<const WgKnob*>(_pOrg);
}

//____ _onRender() _____________________________________________________________

void WgKnob::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
//    pDevice->Fill( _clip, WgColor::black );

	if( _window.h > 1 )
		pDevice->ClipDrawElipse( _clip, _canvas, WgColor::pink );

}

//____ _onAlphaTest() ___________________________________________________________

bool WgKnob::_onAlphaTest( const WgCoord& ofs )
{
	return true;
}


//____ _onEnable() _____________________________________________________________

void WgKnob::_onEnable()
{
    _requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgKnob::_onDisable()
{
    _requestRender();
}




