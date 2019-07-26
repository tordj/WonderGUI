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

	const char TextDisplay::CLASSNAME[] = {"TextDisplay"};



	//____ TextDisplay() _________________________________________________________________

	TextDisplay::TextDisplay() : m_text(this), text(&m_text)
	{
	}


	//____ Destructor _____________________________________________________________

	TextDisplay::~TextDisplay()
	{
	}


	//____ isInstanceOf() _________________________________________________________

	bool TextDisplay::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * TextDisplay::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	TextDisplay_p TextDisplay::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextDisplay_p( static_cast<TextDisplay*>(pObject) );

		return 0;
	}

	//____ pointerStyle() ________________________________________

	PointerStyle TextDisplay::pointerStyle() const
	{
		if( m_text.markedLink() )
			return PointerStyle::Hand;

		return m_pointerStyle;
	}

	//____ tooltipString() _____________________________________________________

	String TextDisplay::tooltipString() const
	{
		if( !m_tooltip.isEmpty() )
			return m_tooltip;
		else
			return m_text.tooltip();
	}

	//____ _matchingWidth() _______________________________________________________

	int TextDisplay::_matchingWidth(int height) const
	{
		//TODO: Need to remove padding before calculations as well.

		int textWidth = m_text.matchingWidth(height);

		if (m_pSkin)
			textWidth += m_pSkin->_contentPadding().w;

		return textWidth;
	}

	//____ _matchingHeight() _______________________________________________________

	int TextDisplay::_matchingHeight(int width) const
	{
		//TODO: Need to remove padding before calculations as well.

		int textHeight = m_text.matchingHeight(width);

		if (m_pSkin)
			textHeight += m_pSkin->_contentPadding().h;

		return textHeight;
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI TextDisplay::_preferredSize() const
	{
		SizeI contentSize = m_text.preferredSize();

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

		m_text.render(pDevice, canvas);
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
		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let m_text.setState() return if rendering is needed)
	}


	//____ _receive() ______________________________________________________________

	void TextDisplay::_receive( Msg * pMsg )
	{
		Widget::_receive(pMsg);
	/*
		RectI canvas = geo();
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(canvas, m_state);

		m_text.receive( pMsg, pHandler, canvas );
	*/
	}


	//____ _cloneContent() _______________________________________________________

	void TextDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const TextDisplay * pOrg = static_cast<const TextDisplay*>(_pOrg);

		m_text = pOrg->m_text;
	}

	//____ _setSkin() _______________________________________________________

	void TextDisplay::_setSkin( Skin * pSkin )
	{
		SizeI oldTextCanvas = m_pSkin ? m_size - m_pSkin->_contentPadding() : m_size;
		Widget::_setSkin(pSkin);

		SizeI newTextCanvas = m_pSkin ? m_size - m_pSkin->_contentPadding() : m_size;

		if (newTextCanvas != oldTextCanvas)
			m_text.setSize(newTextCanvas);
	}

	//____ _setSize() ________________________________________________

	void TextDisplay::_setSize( const SizeI& size )
	{
		Widget::_setSize(size);

		SizeI textSize = size;
		if( m_pSkin )
			textSize -= m_pSkin->_contentPadding();

		m_text.setSize( textSize );
	}


} // namespace wg
