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

#include <wg_ctext.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	CText::CText( ComponentHolder * pHolder ) : Component( pHolder )
	{
		_textMapper()->addComponent(this);
	}
	
	//____ Destructor ______________________________________________________________
	
	CText::~CText()
	{
		_textMapper()->removeComponent(this);
	}
	
	//____ setStyle() ______________________________________________________________
	
	void CText::setStyle( TextStyle * pStyle )
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = pStyle;
		_textMapper()->onStyleChanged(this, pStyle, pOld);
	}
	
	//____ clearStyle() ____________________________________________________________
	
	void CText::clearStyle()
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = 0;
		_textMapper()->onStyleChanged(this, nullptr, pOld);
	}
	
	//____ setTextMapper() __________________________________________________________
	
	void CText::setTextMapper( TextMapper * pTextMapper )
	{
		if( pTextMapper == m_pTextMapper )
			return;
			
		_textMapper()->removeComponent(this);
		m_pTextMapper = pTextMapper;
		_textMapper()->addComponent(this);
	}
	
	//____ clearTextMapper() ________________________________________________________
	
	void CText::clearTextMapper()
	{
		if( !m_pTextMapper )
			return;
			
		_textMapper()->removeComponent(this);
		m_pTextMapper = 0;
		_textMapper()->addComponent(this);
	}
	
	//____ setState() ______________________________________________________________
	
	void CText::setState( State state )
	{
		if( state == m_state )
			return;
	
		State old = m_state;
		m_state = state;
		_textMapper()->onStateChanged( this, state, old );
	}
	
	//____ preferredSize() _________________________________________________________
	
	Size CText::preferredSize() const
	{
		return _textMapper()->preferredSize(this);
	}
	
	//____ matchingWidth() _________________________________________________________
	
	int CText::matchingWidth( int height ) const
	{
		return _textMapper()->matchingWidth(this, height);
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int CText::matchingHeight( int width ) const
	{
		return _textMapper()->matchingHeight(this, width);
	}
	
	//____ charAtPos() ___________________________________________________________
	
	int CText::charAtPos( Coord pos ) const
	{
		return _textMapper()->charAtPos(this,pos);
	}
	
	//____ charRect() ____________________________________________________________
	
	Rect CText::charRect( int charOfs ) const
	{
		return _textMapper()->charRect(this, charOfs);
	}
	
	//____ charLine() ____________________________________________________________
	
	int CText::charLine( int charOfs ) const
	{
		return _textMapper()->charLine(this, charOfs);
	}
	
	
	//____ refresh() _____________________________________________________________
	
	void CText::refresh()
	{
		_textMapper()->onRefresh(this);
	}
	
	//____ setSize() _____________________________________________________________
	
	void CText::setSize( Size size )
	{
		if( size == m_size )
			return;
	
		Size oldSize = m_size;
		m_size = size;
		_textMapper()->onResized(this,size, oldSize);
	}
	
	//_____ render() _____________________________________________________________
	
	void  CText::render( GfxDevice * pDevice, const Rect& _canvas )
	{
		_textMapper()->render(this, pDevice, _canvas);
	}
	
	//____ rectForRange() __________________________________________________________
	
	Rect  CText::rectForRange( int ofs, int length ) const
	{
		return _textMapper()->rectForRange(this, ofs, length);
	}
	
	//____ tooltip() _______________________________________________________________
	
	String CText::tooltip() const
	{
		return _textMapper()->tooltip(this);
	}
	
	//____ getString() ___________________________________________________________________
	
	String CText::getString() const
	{
		return String(&m_charBuffer);
	}
	
	//____ selectionBegin() ________________________________________________________
	
	int CText::selectionBegin() const
	{
		return 0;
	}
	
	//____ selectionEnd() __________________________________________________________
	
	int CText::selectionEnd() const
	{
		return 0;
	}

	//____ _editState() ________________________________________________________

	const EditState * CText::_editState() const
	{
		return nullptr;
	}



} // namespace wg
