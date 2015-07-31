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

namespace wg 
{
	
	
	const char WgValueDisplay::CLASSNAME[] = {"ValueDisplay"};
	
	
	//____ WgValueDisplay() _________________________________________________________________
	
	WgValueDisplay::WgValueDisplay() : m_field(this), value(&m_field)
	{
	}
	
	//____ ~WgValueDisplay() ___________________________________________________________
	
	WgValueDisplay::~WgValueDisplay()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgValueDisplay::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgValueDisplay::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgValueDisplay_p WgValueDisplay::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgValueDisplay_p( static_cast<WgValueDisplay*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ preferredSize() __________________________________________________________
	
	WgSize WgValueDisplay::preferredSize() const
	{
		WgSize size = m_field.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(size);
		else
			return size;
	}
	
	
	//____ _onRefresh() ____________________________________________________________
	
	void WgValueDisplay::_onRefresh( void )
	{
		m_field.onRefresh();
		WgWidget::_onRefresh();
	}
	
	//____ _onRender() _____________________________________________________________
	
	void WgValueDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender(pDevice,_canvas,_window,_clip);
		m_field.onRender(pDevice, _canvas, _clip);
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
		m_field.setState(m_state);
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void WgValueDisplay::_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin )
	{
		WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	//	m_text.setColorSkin(pNewSkin);
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
	
	

} // namespace wg
