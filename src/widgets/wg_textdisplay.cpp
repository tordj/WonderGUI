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

namespace wg
{

	const TypeInfo TextDisplay::TYPEINFO = { "TextDisplay", &Widget::TYPEINFO };



	//____ TextDisplay() _________________________________________________________________

	TextDisplay::TextDisplay() : text(this)
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
		if( _text().markedLink() )
			return PointerStyle::Hand;

		return m_pointerStyle;
	}

	//____ tooltip() _____________________________________________________

	String TextDisplay::tooltip() const
	{
		if( !m_tooltip.isEmpty() )
			return m_tooltip;
		else
			return _text()._tooltip();
	}

	//____ _matchingWidth() _______________________________________________________

	int TextDisplay::_matchingWidth(int height) const
	{
		//TODO: Need to remove padding before calculations as well.

		int textWidth = _text()._matchingWidth(height);

		if (m_pSkin)
			textWidth += m_pSkin->_contentPaddingSize().w;

		return textWidth;
	}

	//____ _matchingHeight() _______________________________________________________

	int TextDisplay::_matchingHeight(int width) const
	{
		//TODO: Need to remove padding before calculations as well.

		int textHeight = _text()._matchingHeight(width);

		if (m_pSkin)
			textHeight += m_pSkin->_contentPaddingSize().h;

		return textHeight;
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI TextDisplay::_preferredSize() const
	{
		SizeI contentSize = _text()._preferredSize();

		if (m_pSkin)
			return m_pSkin->_sizeForContent(contentSize);
		else
			return contentSize;
	}

	//____ _render() ________________________________________________________

	void TextDisplay::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectI canvas;
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		_text()._render(pDevice, canvas);
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
		_text()._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let _text().setState() return if rendering is needed)
	}


	//____ _receive() ______________________________________________________________

	void TextDisplay::_receive( Msg * pMsg )
	{
		Widget::_receive(pMsg);
	/*
		RectI canvas = geo();
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(canvas, m_state);

		_text().receive( pMsg, pHandler, canvas );
	*/
	}


	//____ _cloneContent() _______________________________________________________

	void TextDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const TextDisplay * pOrg = static_cast<const TextDisplay*>(_pOrg);

//		m_text = pOrg->m_text;
	}

	//____ _setSkin() _______________________________________________________

	void TextDisplay::_setSkin( Skin * pSkin )
	{
		SizeI oldTextCanvas = m_pSkin ? m_size - m_pSkin->_contentPaddingSize() : m_size;
		Widget::_setSkin(pSkin);

		SizeI newTextCanvas = m_pSkin ? m_size - m_pSkin->_contentPaddingSize() : m_size;

		if (newTextCanvas != oldTextCanvas)
			_text()._setSize(newTextCanvas);
	}

	//____ _resize() ________________________________________________

	void TextDisplay::_resize( const SizeI& size )
	{
		Widget::_resize(size);

		SizeI textSize = size;
		if( m_pSkin )
			textSize -= m_pSkin->_contentPaddingSize();

		_text()._setSize( textSize );
	}


} // namespace wg
