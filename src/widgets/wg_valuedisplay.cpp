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
	
	ValueDisplay::ValueDisplay() : m_value(this), value(&m_value)
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
	
	ValueDisplay_p ValueDisplay::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ValueDisplay_p( static_cast<ValueDisplay*>(pObject) );
	
		return 0;
	}
	
	
	//____ preferredSize() __________________________________________________________
	
	Size ValueDisplay::preferredSize() const
	{
		Size size = m_value.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(size);
		else
			return size;
	}
	
	
	//____ _refresh() ____________________________________________________________
	
	void ValueDisplay::_refresh( void )
	{
		m_value.refresh();
		Widget::_refresh();
	}
	
	//____ _render() _____________________________________________________________
	
	void ValueDisplay::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
		m_value.render(pDevice, _canvas);
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void ValueDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
	}
	
	//____ _setState() ______________________________________________________
	
	void ValueDisplay::_setState( State state )
	{
		Widget::_setState(state);
		m_value.setState(state);
	}
	
	//____ _setSkin() _______________________________________________________
	
	void ValueDisplay::_setSkin( Skin * pSkin )
	{
		Widget::_setSkin(pSkin);
	}

} // namespace wg
