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

const char WgSwitch::CLASSNAME[] = {"Switch"};

//____ Constructor ____________________________________________________________

WgSwitch::WgSwitch()
{
}

//____ Destructor _____________________________________________________________

WgSwitch::~WgSwitch()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgSwitch::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgSwitch::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgSwitchPtr WgSwitch::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgSwitchPtr( static_cast<WgSwitch*>(pObject.rawPtr()) );

	return 0;
}


//____ setValue() _____________________________________________________________

void WgSwitch::setValue( int value )
{
    //TODO: Implement!
}

//____ preferredSize() __________________________________________________________

WgSize WgSwitch::preferredSize() const
{
	WgSize contentSize(40,40);

	if( m_pSkin )
		return m_pSkin->sizeForContent(contentSize);
	else
		return contentSize;
}


//____ _onCloneContent() _______________________________________________________

void WgSwitch::_onCloneContent( const WgWidget * _pOrg )
{
	const WgSwitch * pOrg = static_cast<const WgSwitch*>(_pOrg);
}

//____ _onRender() _____________________________________________________________

void WgSwitch::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->contentRect(_canvas,m_state);
	else
		canvas = _canvas;

	int sz = WgMin( canvas.w,canvas.h );

	if( sz > 12 )
	{
		int itemSize = sz/4;
		int stepping = itemSize/2;

		int y = canvas.y;
		int x = canvas.x + (sz - itemSize) / 2;
		for( int i = 0 ; i < 3 ; i++ )
		{
			pDevice->clipDrawElipse( _clip, WgRect(x,y,itemSize,itemSize), WgColor::white );
			y += itemSize + stepping;
		}
	}	
}

//____ _onAlphaTest() ___________________________________________________________

bool WgSwitch::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	return true;
}





