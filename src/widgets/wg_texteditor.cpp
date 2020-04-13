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

	const TypeInfo TextEditor::TYPEINFO = { "TextEditor", &Widget::TYPEINFO };



	//____ TextEditor() _________________________________________________________________

	TextEditor::TextEditor() : text(this)
	{
	}


	//____ Destructor _____________________________________________________________

	TextEditor::~TextEditor()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TextEditor::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ matchingHeight() _______________________________________________________

	MU TextEditor::matchingHeight( MU width ) const
	{
		//TODO: Remove skin contentPadding before calculations.

		MU textHeight = _text()._matchingHeight( width );

		if( m_pSkin )
			textHeight += m_pSkin->contentPaddingSize().h;

		return textHeight;
	}

	//____ preferredSize() _____________________________________________________________

	Size TextEditor::preferredSize() const
	{
		Size contentSize = _text()._preferredSize();

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

		_text()._render( pDevice, canvas );
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

		_text()._setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void TextEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		_text()._receive( pMsg );
	}


	//____ _cloneContent() _______________________________________________________

	void TextEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const TextEditor * pOrg = static_cast<const TextEditor*>(_pOrg);
	}

	//____ _setSkin() _______________________________________________________

	void TextEditor::_setSkin( Skin * pSkin )
	{
		Size oldTextCanvas = m_pSkin ? m_size - m_pSkin->contentPaddingSize() : m_size;
		Widget::_setSkin(pSkin);

		Size newTextCanvas = m_pSkin ? m_size - m_pSkin->contentPaddingSize() : m_size;

		if (newTextCanvas != oldTextCanvas)
			_text()._setSize(newTextCanvas);
	}

	//____ _resize() ________________________________________________

	void TextEditor::_resize( const Size& size )
	{
		Widget::_resize( size );

		if( m_pSkin )
			_text()._setSize(size - m_pSkin->contentPaddingSize());
		else
			_text()._setSize(size);
	}

} // namespace wg
