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
#include <wg_msgrouter.h>
#include <wg_internal.h>

namespace wg
{

	const TypeInfo TextDisplay::TYPEINFO = { "TextDisplay", &Widget::TYPEINFO };



	//____ TextDisplay() _________________________________________________________________

	TextDisplay::TextDisplay() : display(this)
	{
	}


	//____ Destructor _____________________________________________________________

	TextDisplay::~TextDisplay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TextDisplay ::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ pointerStyle() ________________________________________

	PointerStyle TextDisplay::pointerStyle() const
	{
		if( display.markedLink() )
			return PointerStyle::Hand;

		return m_pointerStyle;
	}

	//____ tooltip() _____________________________________________________

	String TextDisplay::tooltip() const
	{
		if( !m_tooltip.isEmpty() )
			return m_tooltip;
		else
			return OO(display)._tooltip();
	}

	//____ _matchingWidth() _______________________________________________________

	spx TextDisplay::_matchingWidth(spx height, int scale) const
	{
		scale = _fixScale(scale);
		SizeSPX padding = _contentPaddingSize();

		height -= padding.h;
		spx textWidth = OO(display)._matchingWidth(height, scale);
		textWidth += padding.w;
		return textWidth;
	}

	//____ _matchingHeight() _______________________________________________________

	spx TextDisplay::_matchingHeight(spx width, int scale) const
	{
		scale = _fixScale(scale);
		SizeSPX padding = _contentPaddingSize();

		width -= padding.w;
		spx textHeight = OO(display)._matchingHeight(width, scale);
		textHeight += padding.h;
		return textHeight;
	}

	//____ preferredSize() _____________________________________________________________

	SizeSPX TextDisplay::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX contentSize = OO(display)._preferredSize(scale);

		return OO(skin)._sizeForContent(contentSize, scale);
	}

	//____ _render() ________________________________________________________

	void TextDisplay::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectSPX canvas = OO(skin)._contentRect(_canvas, m_scale, m_state);

		OO(display)._render(pDevice, canvas);
	}

	//____ _refresh() _______________________________________________________

	void TextDisplay::_refresh( void )
	{
		//TODO: Implement more I believe...

		Widget::_refresh();
	}

	//____ _setState() ______________________________________________________

	void TextDisplay::_setState( State state )
	{
		Widget::_setState(state);
		OO(display)._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let OO(display).setState() return if rendering is needed)
	}


	//____ _receive() ______________________________________________________________

	void TextDisplay::_receive( Msg * pMsg )
	{
		Widget::_receive(pMsg);
	/*
		RectI canvas = geo();
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(canvas, m_state);

		OO(display).receive( pMsg, pHandler, canvas );
	*/
	}


	//____ _cloneContent() _______________________________________________________

	void TextDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const TextDisplay * pOrg = static_cast<const TextDisplay*>(_pOrg);

//		m_text = pOrg->m_text;
	}

	//____ _resize() ________________________________________________

	void TextDisplay::_resize( const SizeSPX& size, int scale )
	{
		Widget::_resize(size, scale);

		SizeSPX textSize = size - OO(skin)._contentPaddingSize(m_scale);		// Using m_scale here, since that has been fixed.

		OO(display)._setSize( textSize, scale );
	}


} // namespace wg
