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

#include <wg_textdisplay.h>
#include <wg_key.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>

const char WgTextDisplay::CLASSNAME[] = {"TextDisplay"};



//____ WgTextDisplay() _________________________________________________________________

WgTextDisplay::WgTextDisplay() : m_text(this), text(&m_text)
{
}


//____ Destructor _____________________________________________________________

WgTextDisplay::~WgTextDisplay()
{
}


//____ isInstanceOf() _________________________________________________________

bool WgTextDisplay::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgTextDisplay::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgTextDisplayPtr WgTextDisplay::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgTextDisplayPtr( static_cast<WgTextDisplay*>(pObject.rawPtr()) );

	return 0;
}

//____ matchingWidth() _______________________________________________________

int WgTextDisplay::matchingWidth( int height ) const
{
	int textWidth = m_text.matchingWidth( height );

	if( m_pSkin )
		textWidth += m_pSkin->contentPadding().w;

	return textWidth;
}

//____ matchingHeight() _______________________________________________________

int WgTextDisplay::matchingHeight( int width ) const
{
	int textHeight = m_text.matchingHeight( width );

	if( m_pSkin )
		textHeight += m_pSkin->contentPadding().h;

	return textHeight;
}

//____ preferredSize() _____________________________________________________________

WgSize WgTextDisplay::preferredSize() const
{
	WgSize contentSize = m_text.preferredSize();

	if( m_pSkin )
		return m_pSkin->sizeForContent(contentSize);
	else
		return contentSize;
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgTextDisplay::PointerStyle() const
{
	if( m_text.getMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ TooltipString() _____________________________________________________

WgString WgTextDisplay::TooltipString() const
{
	if( !m_tooltip.isEmpty() )
		return m_tooltip;
	else
		return m_text.tooltip();
}

//____ _onRender() ________________________________________________________

void WgTextDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->contentRect(_canvas, m_state);
	else
		canvas = _canvas;

	m_text.onRender(pDevice, canvas, _clip);
}

//____ _onRefresh() _______________________________________________________

void WgTextDisplay::_onRefresh( void )
{
	//TODO: Implement more I believe...

	WgWidget::_onRefresh();
}

//____ _onStateChanged() ______________________________________________________

void WgTextDisplay::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);
	m_text.setState(m_state);
	_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
}


//____ _onMsg() ______________________________________________________________

void WgTextDisplay::_onMsg( const WgMsgPtr& pMsg )
{
	WgWidget::_onMsg(pMsg);
/*
	WgRect canvas = geo();
	if( m_pSkin )
		canvas = m_pSkin->contentRect(canvas, m_state);

	m_text.onMsg( pMsg, pHandler, canvas );
*/
}


//____ _onCloneContent() _______________________________________________________

void WgTextDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	const WgTextDisplay * pOrg = static_cast<const WgTextDisplay*>(_pOrg);

	m_text = pOrg->m_text;
}

//____ _onSkinChanged() _______________________________________________________

void WgTextDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
}

//____ _onNewSize() ________________________________________________

void WgTextDisplay::_onNewSize( const WgSize& size )
{
	WgWidget::_onNewSize(size);

	WgSize textSize = size;
	if( m_pSkin )
		textSize -= m_pSkin->contentPadding();

	m_text.onNewSize( textSize );
}

//____ _onFieldDirty() _________________________________________________________

void WgTextDisplay::_onFieldDirty( WgField * pField )
{
	_requestRender();
}

//____ _onFieldResize() ________________________________________________________

void WgTextDisplay::_onFieldResize( WgField * pField )
{
	_requestResize();
	_requestRender();
}

