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
	
	TextDisplay_p TextDisplay::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextDisplay_p( static_cast<TextDisplay*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int TextDisplay::matchingWidth( int height ) const
	{
		int textWidth = m_text.matchingWidth( height );
	
		if( m_pSkin )
			textWidth += m_pSkin->contentPadding().w;
	
		return textWidth;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int TextDisplay::matchingHeight( int width ) const
	{
		int textHeight = m_text.matchingHeight( width );
	
		if( m_pSkin )
			textHeight += m_pSkin->contentPadding().h;
	
		return textHeight;
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size TextDisplay::preferredSize() const
	{
		Size contentSize = m_text.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	//____ pointerStyle() ________________________________________
	
	WgPointerStyle TextDisplay::pointerStyle() const
	{
		if( m_text.getMarkedLink() )
			return WG_POINTER_HAND;
	
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
	
	//____ _onRender() ________________________________________________________
	
	void TextDisplay::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;
	
		m_text.onRender(pDevice, canvas, _clip);
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void TextDisplay::_onRefresh( void )
	{
		//TODO: Implement more I believe...
	
		Widget::_onRefresh();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void TextDisplay::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
		m_text.setState(m_state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}
	
	
	//____ _onMsg() ______________________________________________________________
	
	void TextDisplay::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
	/*
		Rect canvas = geo();
		if( m_pSkin )
			canvas = m_pSkin->contentRect(canvas, m_state);
	
		m_text.onMsg( pMsg, pHandler, canvas );
	*/
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void TextDisplay::_onCloneContent( const Widget * _pOrg )
	{
		const TextDisplay * pOrg = static_cast<const TextDisplay*>(_pOrg);
	
		m_text = pOrg->m_text;
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void TextDisplay::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
	}
	
	//____ _onNewSize() ________________________________________________
	
	void TextDisplay::_onNewSize( const Size& size )
	{
		Widget::_onNewSize(size);
	
		Size textSize = size;
		if( m_pSkin )
			textSize -= m_pSkin->contentPadding();
	
		m_text.onNewSize( textSize );
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void TextDisplay::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void TextDisplay::_onFieldResize( Field * pField )
	{
		_requestResize();
		_requestRender();
	}
	

} // namespace wg
