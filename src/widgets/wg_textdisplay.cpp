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
		if( text.markedLink() )
			return PointerStyle::Hand;

		return m_pointerStyle;
	}

	//____ tooltip() _____________________________________________________

	String TextDisplay::tooltip() const
	{
		if( !m_tooltip.isEmpty() )
			return m_tooltip;
		else
			return OO(text)._tooltip();
	}

	//____ matchingWidth() _______________________________________________________

	MU TextDisplay::matchingWidth(MU height) const
	{
		//TODO: Need to remove padding before calculations as well.

		MU textWidth = OO(text)._matchingWidth(height);

		textWidth += m_skin.contentPaddingSize().w;

		return textWidth;
	}

	//____ matchingHeight() _______________________________________________________

	MU TextDisplay::matchingHeight(MU width) const
	{
		//TODO: Need to remove padding before calculations as well.

		MU textHeight = OO(text)._matchingHeight(width);

		textHeight += m_skin.contentPaddingSize().h;

		return textHeight;
	}

	//____ preferredSize() _____________________________________________________________

	Size TextDisplay::preferredSize() const
	{
		Size contentSize = OO(text)._preferredSize();

		return m_skin.sizeForContent(contentSize);
	}

	//____ _render() ________________________________________________________

	void TextDisplay::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect canvas = m_skin.contentRect(_canvas, m_state);

		OO(text)._render(pDevice, canvas);
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
		OO(text)._setState(state);
		_requestRender(); //TODO: Only requestRender if text appearance has changed (let OO(text).setState() return if rendering is needed)
	}


	//____ _receive() ______________________________________________________________

	void TextDisplay::_receive( Msg * pMsg )
	{
		Widget::_receive(pMsg);
	/*
		RectI canvas = geo();
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(canvas, m_state);

		OO(text).receive( pMsg, pHandler, canvas );
	*/
	}


	//____ _cloneContent() _______________________________________________________

	void TextDisplay::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const TextDisplay * pOrg = static_cast<const TextDisplay*>(_pOrg);

//		m_text = pOrg->m_text;
	}

	//____ setSkin() _______________________________________________________

	void TextDisplay::setSkin( Skin * pSkin )
	{
		Size oldTextCanvas = m_size - m_skin.contentPaddingSize();
		Widget::setSkin(pSkin);

		Size newTextCanvas = m_size - m_skin.contentPaddingSize();

		if (newTextCanvas != oldTextCanvas)
			OO(text)._setSize(newTextCanvas);
	}

	//____ _resize() ________________________________________________

	void TextDisplay::_resize( const Size& size )
	{
		Widget::_resize(size);

		Size textSize = size - m_skin.contentPaddingSize();

		OO(text)._setSize( textSize );
	}


} // namespace wg
