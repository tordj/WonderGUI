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


	const TypeInfo ValueDisplay::TYPEINFO = { "ValueDisplay", &Widget::TYPEINFO };


	//____ ValueDisplay() _________________________________________________________________

	ValueDisplay::ValueDisplay() : value(this)
	{
	}

	//____ ~ValueDisplay() ___________________________________________________________

	ValueDisplay::~ValueDisplay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ValueDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _preferredSize() __________________________________________________________

	SizeI ValueDisplay::_preferredSize() const
	{
		SizeI size = _value()._preferredSize();

		if( m_pSkin )
			return m_pSkin->_sizeForContent(size);
		else
			return size;
	}


	//____ _refresh() ____________________________________________________________

	void ValueDisplay::_refresh( void )
	{
		_value()._refresh();
		Widget::_refresh();
	}

	//____ _render() _____________________________________________________________

	void ValueDisplay::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
		_value()._render(pDevice, _canvas);
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
		_value()._setState(state);
	}

	//____ _setSkin() _______________________________________________________

	void ValueDisplay::_setSkin( Skin * pSkin )
	{
		Widget::_setSkin(pSkin);
	}

} // namespace wg
