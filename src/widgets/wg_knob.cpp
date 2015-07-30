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

const char WgKnob::CLASSNAME[] = {"Knob"};

//____ Constructor ____________________________________________________________

WgKnob::WgKnob()
{
}

//____ Destructor _____________________________________________________________

WgKnob::~WgKnob()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgKnob::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgKnob::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgKnob_p WgKnob::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgKnob_p( static_cast<WgKnob*>(pObject.rawPtr()) );

	return 0;
}


//____ setValue() _____________________________________________________________

void WgKnob::setValue( float value )
{
    //TODO: Implement!
}

//____ preferredSize() __________________________________________________________

WgSize WgKnob::preferredSize() const
{
	return WgSize(40,40);
}


//____ _onCloneContent() _______________________________________________________

void WgKnob::_onCloneContent( const WgWidget * _pOrg )
{
	const WgKnob * pOrg = static_cast<const WgKnob*>(_pOrg);
}

//____ _onRender() _____________________________________________________________

void WgKnob::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice, _canvas, _window, _clip);

	int sz = WgMin( _canvas.w,_canvas.h );

	if( sz > 1 )
	{
		pDevice->clipDrawElipse( _clip, WgRect(_canvas.pos(),sz,sz), WgColor::pink );
	}
}

//____ _onAlphaTest() ___________________________________________________________

bool WgKnob::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	return true;
}


