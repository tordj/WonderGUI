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
#include <wg_eventhandler.h>

const char WgTextDisplay::CLASSNAME[] = {"TextDisplay"};



//____ WgTextDisplay() _________________________________________________________________

WgTextDisplay::WgTextDisplay() : m_text(this), text(&m_text)
{
}


//____ Destructor _____________________________________________________________

WgTextDisplay::~WgTextDisplay()
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgTextDisplay::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgTextDisplay::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgTextDisplayPtr WgTextDisplay::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextDisplayPtr( static_cast<WgTextDisplay*>(pObject.RawPtr()) );

	return 0;
}

//____ MatchingWidth() _______________________________________________________

int WgTextDisplay::MatchingWidth( int height ) const
{
	int textWidth = m_text.MatchingWidth( height );

	if( m_pSkin )
		textWidth += m_pSkin->ContentPadding().w;

	return textWidth;
}

//____ MatchingHeight() _______________________________________________________

int WgTextDisplay::MatchingHeight( int width ) const
{
	int textHeight = m_text.MatchingHeight( width );

	if( m_pSkin )
		textHeight += m_pSkin->ContentPadding().h;

	return textHeight;
}

//____ PreferredSize() _____________________________________________________________

WgSize WgTextDisplay::PreferredSize() const
{
	WgSize contentSize = m_text.PreferredSize();

	if( m_pSkin )
		return m_pSkin->SizeForContent(contentSize);
	else
		return contentSize;
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgTextDisplay::PointerStyle() const
{
	if( m_text.GetMarkedLink() )
		return WG_POINTER_HAND;

	return m_pointerStyle;
}

//____ TooltipString() _____________________________________________________

WgString WgTextDisplay::TooltipString() const
{
	if( !m_tooltip.IsEmpty() )
		return m_tooltip;
	else
		return m_text.Tooltip();
}

//____ _onRender() ________________________________________________________

void WgTextDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(_canvas, m_state);
	else
		canvas = _canvas;

	m_text.OnRender(pDevice, canvas, _clip);
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
	m_text.SetState(m_state);
	_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
}


//____ _onEvent() ______________________________________________________________

void WgTextDisplay::_onEvent( const WgEventPtr& pEvent )
{
	WgWidget::_onEvent(pEvent);
/*
	WgRect canvas = Geo();
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(canvas, m_state);

	m_text.OnEvent( pEvent, pHandler, canvas );
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
		textSize -= m_pSkin->ContentPadding();

	m_text.OnNewSize( textSize );
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

