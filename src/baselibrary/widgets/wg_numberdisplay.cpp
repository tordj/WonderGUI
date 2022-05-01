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

#include	<wg_numberdisplay.h>
#include	<wg_gfxdevice.h>
#include	<wg_internal.h>

namespace wg
{


	const TypeInfo NumberDisplay::TYPEINFO = { "NumberDisplay", &Widget::TYPEINFO };


	//____ NumberDisplay() _________________________________________________________________

	NumberDisplay::NumberDisplay() : display(this)
	{
	}

	NumberDisplay::NumberDisplay(const Blueprint& bp) : display(this)
	{
		_initFromBlueprint(bp);
		display._initFromBlueprint(bp.display);
	}

	//____ ~NumberDisplay() ___________________________________________________________

	NumberDisplay::~NumberDisplay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& NumberDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _defaultSize() __________________________________________________________

	SizeSPX NumberDisplay::_defaultSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX size = display._defaultSize(scale);

		return m_skin.sizeForContent(size, scale);
	}


	//____ _refresh() ____________________________________________________________

	void NumberDisplay::_refresh( void )
	{
		display._refresh();
		Widget::_refresh();
	}

	//____ _render() _____________________________________________________________

	void NumberDisplay::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
		display._render(pDevice, _contentRect() + _canvas.pos(), m_scale, m_state);
	}

	//____ _setState() ______________________________________________________

	void NumberDisplay::_setState( State state )
	{
		if (display._stateChangeNeedsRender(m_state, state))
			_requestRender(_contentRect());

		Widget::_setState(state);
	}


} // namespace wg
