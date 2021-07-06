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

	//____ _preferredSize() __________________________________________________________

	SizeSPX ValueDisplay::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX size = value._preferredSize(scale);

		return OO(skin)._sizeForContent(size, scale);
	}


	//____ _refresh() ____________________________________________________________

	void ValueDisplay::_refresh( void )
	{
		value._refresh();
		Widget::_refresh();
	}

	//____ _render() _____________________________________________________________

	void ValueDisplay::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
		value._render(pDevice, _contentRect() + _canvas.pos(), m_scale, m_state);
	}

	//____ _cloneContent() _______________________________________________________

	void ValueDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
	}

	//____ _setState() ______________________________________________________

	void ValueDisplay::_setState( State state )
	{
		if (value._stateChangeNeedsRender(m_state, state))
			_requestRender(_contentRect());

		Widget::_setState(state);
	}


} // namespace wg
