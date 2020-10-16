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
#include	<wg_internal.h>

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

	//____ preferredSize() __________________________________________________________

	Size ValueDisplay::preferredSize() const
	{
		Size size = OO(value)._preferredSize();

		return m_skin.sizeForContent(size);
	}


	//____ _refresh() ____________________________________________________________

	void ValueDisplay::_refresh( void )
	{
		OO(value)._refresh();
		Widget::_refresh();
	}

	//____ _render() _____________________________________________________________

	void ValueDisplay::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
		OO(value)._render(pDevice, _canvas);
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
		OO(value)._setState(state);
	}

	//____ setSkin() _______________________________________________________

	void ValueDisplay::setSkin( Skin * pSkin )
	{
		//TODO: Refresh size for the CValueDisplay

		Widget::setSkin(pSkin);
	}

} // namespace wg
