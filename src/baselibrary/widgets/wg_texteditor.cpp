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

	TextEditor::TextEditor() : editor(this)
	{
	}

	TextEditor::TextEditor(const Blueprint& bp) : editor(this)
	{
		_initFromBlueprint(bp);
		editor._initFromBlueprint(bp.editor);
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

	//____ _matchingWidth() _______________________________________________________

	spx TextEditor::_matchingWidth(spx height, int scale) const
	{
		scale = _fixScale(scale);
		SizeSPX padding = _contentPaddingSize();

		height -= padding.h;
		spx textWidth = editor._matchingWidth(height, scale);
		textWidth += padding.w;
		return textWidth;
	}

	//____ _matchingHeight() _______________________________________________________

	spx TextEditor::_matchingHeight( spx width, int scale ) const
	{
		scale = _fixScale(scale);
		SizeSPX padding = _contentPaddingSize();

		width -= padding.w;
		spx textHeight = editor._matchingHeight( width, scale );
		textHeight += padding.h;
		return textHeight;
	}

	//____ _defaultSize() _____________________________________________________________

	SizeSPX TextEditor::_defaultSize(int scale) const
	{
		scale = _fixScale(scale);

		SizeSPX contentSize = editor._defaultSize(scale);

		return m_skin.sizeForContent(contentSize,scale);
	}

	//____ _render() ________________________________________________________

	void TextEditor::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectSPX canvas = m_skin.contentRect(_canvas, m_scale, m_state);

		editor._render( pDevice, canvas );
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

		editor._setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void TextEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		editor._receive( pMsg );
	}


	//____ _resize() ________________________________________________

	void TextEditor::_resize( const SizeSPX& size, int scale )
	{
		Widget::_resize( size, scale );

		editor._setSize(size - m_skin.contentPaddingSize(m_scale), m_scale);
	}

} // namespace wg
