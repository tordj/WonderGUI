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

#include <wg_textbaseitem.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	TextBaseItem::TextBaseItem( ItemHolder * pHolder ) : Item( pHolder )
	{
		_textMapper()->addItem(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	TextBaseItem::~TextBaseItem()
	{
		_textMapper()->removeItem(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void TextBaseItem::setStyle( const TextStyle_p& pStyle )
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = pStyle;
		_textMapper()->onStyleChanged(this, pStyle.rawPtr(), pOld.rawPtr());
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void TextBaseItem::clearStyle()
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = 0;
		_textMapper()->onStyleChanged(this, nullptr, pOld.rawPtr());
	}
	
	//____ setTextMapper() __________________________________________________________
	
	void TextBaseItem::setTextMapper( const TextMapper_p& pTextMapper )
	{
		if( pTextMapper == m_pTextMapper )
			return;
			
		_textMapper()->removeItem(this);
		m_pTextMapper = pTextMapper;
		_textMapper()->addItem(this);
	}
	
	//____ clearTextMapper() ________________________________________________________
	
	void TextBaseItem::clearTextMapper()
	{
		if( !m_pTextMapper )
			return;
			
		_textMapper()->removeItem(this);
		m_pTextMapper = 0;
		_textMapper()->addItem(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void TextBaseItem::setState( State state )
	{
		if( state == m_state )
			return;
	
		State old = m_state;
		m_state = state;
		_textMapper()->onStateChanged( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	Size TextBaseItem::preferredSize() const
	{
		return _textMapper()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int TextBaseItem::matchingWidth( int height ) const
	{
		return _textMapper()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int TextBaseItem::matchingHeight( int width ) const
	{
		return _textMapper()->matchingHeight(this, width);
	}
	
	//____ charAtPos() ___________________________________________________________
	
	int TextBaseItem::charAtPos( Coord pos ) const
	{
		return _textMapper()->charAtPos(this,pos);
	}
	
	//____ charRect() ____________________________________________________________
	
	Rect TextBaseItem::charRect( int charOfs ) const
	{
		return _textMapper()->charRect(this, charOfs);
	}
	
	//____ charLine() ____________________________________________________________
	
	int TextBaseItem::charLine( int charOfs ) const
	{
		return _textMapper()->charLine(this, charOfs);
	}
	
	
	//____ refresh() _____________________________________________________________
	
	void TextBaseItem::refresh()
	{
		_textMapper()->onRefresh(this);
	}
	
	//____ setSize() _____________________________________________________________
	
	void TextBaseItem::setSize( Size size )
	{
		if( size == m_size )
			return;
	
		Size oldSize = m_size;
		m_size = size;
		_textMapper()->onResized(this,size, oldSize);
	}
	
	//_____ render() _____________________________________________________________
	
	void  TextBaseItem::render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_textMapper()->renderItem(this, pDevice, _canvas, _clip);
	}
	
	//____ rectForRange() __________________________________________________________
	
	Rect  TextBaseItem::rectForRange( int ofs, int length ) const
	{
		return _textMapper()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	String TextBaseItem::tooltip() const
	{
		return _textMapper()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	String TextBaseItem::getString() const
	{
		return String(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int TextBaseItem::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int TextBaseItem::selectionEnd() const
	{
		return 0;
	}

	//____ _editState() ________________________________________________________

	const EditState * TextBaseItem::_editState() const
	{
		return nullptr;
	}



} // namespace wg
