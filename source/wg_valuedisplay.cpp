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

#include	<math.h>
#include	<wg_valuedisplay.h>
#include	<wg_font.h>
#include	<wg_gfxdevice.h>


const char WgValueDisplay::CLASSNAME[] = {"ValueDisplay"};


//____ WgValueDisplay() _________________________________________________________________

WgValueDisplay::WgValueDisplay()
{
	m_text.setHolder(this);
	m_text.SetAlignment( WG_EAST );
	m_text.SetWrap(false);
}

//____ ~WgValueDisplay() ___________________________________________________________

WgValueDisplay::~WgValueDisplay()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgValueDisplay::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgValueDisplay::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgValueDisplayPtr WgValueDisplay::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgValueDisplayPtr( static_cast<WgValueDisplay*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetFormat() ____________________________________________________________

void WgValueDisplay::SetFormat( const WgValueFormatPtr& pFormat )
{
	m_pFormat = pFormat;
	_requestRender();
}

//____ PreferredSize() __________________________________________________________

WgSize WgValueDisplay::PreferredSize() const
{
	WgSize textSize(m_text.Width(),m_text.Height());

	if( m_pSkin )
		return m_pSkin->SizeForContent(textSize);
	else
		return textSize;
}


//____ _valueModified() ________________________________________________________

void WgValueDisplay::_valueModified()
{
	// NOTE: We have decided to not post any event on _valueModified since it
	// can only be done through the API.

	_regenText();
	_requestRender();
}

//____ _textModified() _____________________________________________________________

void WgValueDisplay::_textModified(WgText * pText)
{
	_requestRender();
}


//____ _rangeModified() ________________________________________________________

void WgValueDisplay::_rangeModified()
{
}


//____ _onRefresh() ____________________________________________________________

void WgValueDisplay::_onRefresh( void )
{
	_regenText();
	_requestRender();
}

//____ _regenText() ____________________________________________________________

void WgValueDisplay::_regenText()
{
		m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pFormat );
}

//____ _onRender() _____________________________________________________________

void WgValueDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	pDevice->PrintText( _clip, &m_text, _canvas );
}

//____ _onCloneContent() _______________________________________________________

void WgValueDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	WgValueDisplay * pOrg = (WgValueDisplay *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_pFormat		= pOrg->m_pFormat;
	m_text.clone(&pOrg->m_text);
}

//____ _onStateChanged() ______________________________________________________

void WgValueDisplay::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);
	m_text.setState(m_state);
	_requestRender();				//TODO: Check for text-related difference before call.
}

//____ _onSkinChanged() _______________________________________________________

void WgValueDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}