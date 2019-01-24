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

#include <wg_texteditor.h>
#include <wg_key.h>
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char TextEditor::CLASSNAME[] = {"TextEditor"};
	
	
	
	//____ TextEditor() _________________________________________________________________
	
	TextEditor::TextEditor() : m_text(this), text(&m_text)
	{
	}
	
	
	//____ Destructor _____________________________________________________________
	
	TextEditor::~TextEditor()
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool TextEditor::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * TextEditor::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	TextEditor_p TextEditor::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextEditor_p( static_cast<TextEditor*>(pObject) );
	
		return 0;
	}
		
	//____ matchingHeight() _______________________________________________________
	
	int TextEditor::matchingHeight( int width ) const
	{
		int textHeight = m_text.matchingHeight( width );
	
		if( m_pSkin )
			textHeight += m_pSkin->contentPadding().h;
	
		return textHeight;
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size TextEditor::preferredSize() const
	{
		Size contentSize = m_text.preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	//____ _render() ________________________________________________________
	
	void TextEditor::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		m_text.render( pDevice, canvas );	
	}
	
	//____ _refresh() _______________________________________________________
	
	void TextEditor::_refresh( void )
	{
		//TODO: Implement more I believe...
	
		Widget::_refresh();
	}
	
	//____ _setState() ______________________________________________________
	
	void TextEditor::_setState( State state )
	{
		Widget::_setState(state);
	
		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}
	
	
	//____ _receive() ______________________________________________________________
	
	void TextEditor::_receive( Msg * pMsg )
	{
		MsgType type = pMsg->type();

		Widget::_receive( pMsg );
		m_text.receive( pMsg );	
	}
	
	
	//____ _cloneContent() _______________________________________________________
	
	void TextEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const TextEditor * pOrg = static_cast<const TextEditor*>(_pOrg);
	
		m_text = pOrg->m_text;
	}
	
	//____ _setSkin() _______________________________________________________
	
	void TextEditor::_setSkin( Skin * pSkin )
	{
		//TODO: Possibly notify text about new canvas size.
		
		Widget::_setSkin(pSkin);
	}
	
	//____ _setSize() ________________________________________________
	
	void TextEditor::_setSize( const Size& size )
	{
		Widget::_setSize( size );
		
		int width = size.w;
	
		if( m_pSkin )
			m_text.setSize(size - m_pSkin->contentPadding());
		else
			m_text.setSize(size);
	}

} // namespace wg
