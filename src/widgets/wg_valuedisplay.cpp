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

#include	<wg_valuedisplay.h>
#include	<wg_gfxdevice.h>


const char WgValueDisplay::CLASSNAME[] = {"ValueDisplay"};


//____ WgValueDisplay() _________________________________________________________________

WgValueDisplay::WgValueDisplay() : m_field(this), value(&m_field)
{
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
		return WgValueDisplayPtr( static_cast<WgValueDisplay*>(pObject.RawPtr()) );

	return 0;
}


//____ PreferredSize() __________________________________________________________

WgSize WgValueDisplay::PreferredSize() const
{
	WgSize size = m_field.PreferredSize();

	if( m_pSkin )
		return m_pSkin->SizeForContent(size);
	else
		return size;
}


//____ _onRefresh() ____________________________________________________________

void WgValueDisplay::_onRefresh( void )
{
	m_field.OnRefresh();
	WgWidget::_onRefresh();
}

//____ _onRender() _____________________________________________________________

void WgValueDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	m_field.OnRender(pDevice, _canvas, _clip);
}

//____ _onCloneContent() _______________________________________________________

void WgValueDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	WgValueDisplay * pOrg = (WgValueDisplay *) _pOrg;
}

//____ _onStateChanged() ______________________________________________________

void WgValueDisplay::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);
	m_field.SetState(m_state);
}

//____ _onSkinChanged() _______________________________________________________

void WgValueDisplay::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
//	m_text.SetColorSkin(pNewSkin);
}

//____ _onFieldDirty() _________________________________________________________

void WgValueDisplay::_onFieldDirty( WgField * pField )
{
	_requestRender();
}

//____ _onFieldResize() ________________________________________________________

void WgValueDisplay::_onFieldResize( WgField * pField )
{
	_requestResize();
	_requestRender();
}

//____ _onValueModified() ______________________________________________________

void WgValueDisplay::_onValueModified( WgModValueField * pField )
{
	_requestResize();
	_requestRender();
}


