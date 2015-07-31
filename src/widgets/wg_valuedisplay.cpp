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
	
	
	const char ValueDisplay::CLASSNAME[] = {"ValueDisplay"};
	
	
	//____ ValueDisplay() _________________________________________________________________
	
	ValueDisplay::ValueDisplay() : m_field(this), value(&m_field)
	{
	}
	
	//____ ~ValueDisplay() ___________________________________________________________
	
	ValueDisplay::~ValueDisplay()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ValueDisplay::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ValueDisplay::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ValueDisplay_p ValueDisplay::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ValueDisplay_p( static_cast<ValueDisplay*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ preferredSize() __________________________________________________________
	
	Size ValueDisplay::preferredSize() const
	{
		Size size = m_field.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(size);
		else
			return size;
	}
	
	
	//____ _onRefresh() ____________________________________________________________
	
	void ValueDisplay::_onRefresh( void )
	{
		m_field.onRefresh();
		Widget::_onRefresh();
	}
	
	//____ _onRender() _____________________________________________________________
	
	void ValueDisplay::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
		m_field.onRender(pDevice, _canvas, _clip);
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void ValueDisplay::_onCloneContent( const Widget * _pOrg )
	{
		ValueDisplay * pOrg = (ValueDisplay *) _pOrg;
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void ValueDisplay::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
		m_field.setState(m_state);
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void ValueDisplay::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
	//	m_text.setColorSkin(pNewSkin);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void ValueDisplay::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void ValueDisplay::_onFieldResize( Field * pField )
	{
		_requestResize();
		_requestRender();
	}
	
	//____ _onValueModified() ______________________________________________________
	
	void ValueDisplay::_onValueModified( ModValueField * pField )
	{
		_requestResize();
		_requestRender();
	}
	
	

} // namespace wg
