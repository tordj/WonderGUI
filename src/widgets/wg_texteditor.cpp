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
#include <wg_internal.h>

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

		MU textHeight = OO(text)._matchingHeight( width );

		textHeight += m_skin.contentPaddingSize().h;

		return textHeight;
	}

	//____ preferredSize() _____________________________________________________________

	Size TextEditor::preferredSize() const
	{
		Size contentSize = OO(text)._preferredSize();

		return m_skin.sizeForContent(contentSize);
	}

	//____ _render() ________________________________________________________

	void TextEditor::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect canvas = m_skin.contentRect(_canvas, m_state);

		OO(text)._render( pDevice, canvas );
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

		OO(text)._setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void TextEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		OO(text)._receive( pMsg );
	}


	//____ _cloneContent() _______________________________________________________

	void TextEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const TextEditor * pOrg = static_cast<const TextEditor*>(_pOrg);
	}

	//____ setSkin() _______________________________________________________

	void TextEditor::setSkin( Skin * pSkin )
	{
		Size oldTextCanvas = m_size - m_skin.contentPaddingSize();
		Widget::setSkin(pSkin);

		Size newTextCanvas = m_size - m_skin.contentPaddingSize();

		if (newTextCanvas != oldTextCanvas)
			OO(text)._setSize(newTextCanvas);
	}

	//____ _resize() ________________________________________________

	void TextEditor::_resize( const Size& size )
	{
		Widget::_resize( size );

		OO(text)._setSize(size - m_skin.contentPaddingSize());
	}

} // namespace wg
