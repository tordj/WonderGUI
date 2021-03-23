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
	using namespace Util;

	const TypeInfo CText::TYPEINFO = { "CText", &GeoComponent::TYPEINFO };

	//____ constructor _____________________________________________________________

	CText::CText(Holder * pHolder ) : GeoComponent( pHolder )
	{
		_textMapper()->addText(this);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CText::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ Destructor ______________________________________________________________

	CText::~CText()
	{
		_textMapper()->removeText(this);
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

		_textMapper()->removeText(this);
		m_pTextMapper = pTextMapper;
		_textMapper()->addText(this);
	}

	//____ clearTextMapper() ________________________________________________________

	void CText::clearTextMapper()
	{
		if( !m_pTextMapper )
			return;

		_textMapper()->removeText(this);
		m_pTextMapper = 0;
		_textMapper()->addText(this);
	}

	//____ _setState() ______________________________________________________________

	void CText::_setState( State state )
	{
		if( state == m_state )
			return;

		State old = m_state;
		m_state = state;
		_textMapper()->onStateChanged( this, state, old );
	}

	//____ _preferredSize() _________________________________________________________

	Size CText::_preferredSize() const
	{
		return _textMapper()->preferredSize(this);
	}

	//____ _matchingWidth() _________________________________________________________

	MU CText::_matchingWidth( MU height ) const
	{
		return _textMapper()->matchingWidth(this, height);
	}

	//____ _matchingHeight() ________________________________________________________

	MU CText::_matchingHeight( MU width ) const
	{
		return _textMapper()->matchingHeight(this, width);
	}

	//____ _charAtPos() ___________________________________________________________

	int CText::_charAtPos( Coord pos ) const
	{
		return _textMapper()->charAtPos(this,pos);
	}

	//____ _charRect() ____________________________________________________________

	Rect CText::_charRect( int charOfs ) const
	{
		return _textMapper()->charRect(this, charOfs);
	}

	//____ _charLine() ____________________________________________________________

	int CText::_charLine( int charOfs ) const
	{
		return _textMapper()->charLine(this, charOfs);
	}


	//____ _refresh() _____________________________________________________________

	void CText::_refresh()
	{
		_textMapper()->onRefresh(this);
	}

	//____ _setSize() _____________________________________________________________

	void CText::_setSize( Size size )
	{
		if( size == m_size )
			return;

		Size oldSize = m_size;
		m_size = size;
		_textMapper()->onResized(this,size, oldSize);
	}

	//_____ _render() _____________________________________________________________

	void  CText::_render( GfxDevice * pDevice, const Rect& _canvas )
	{
		_textMapper()->render(this, pDevice, _canvas );
	}

	//____ _rectForRange() __________________________________________________________

	Rect  CText::_rectForRange( int ofs, int length ) const
	{
		return _textMapper()->rectForRange(this, ofs, length);
	}

	//____ _tooltip() _______________________________________________________________

	String CText::_tooltip() const
	{
		return _textMapper()->tooltip(this);
	}

	//____ _getString() ___________________________________________________________________

	String CText::_getString() const
	{
		return String(&m_charBuffer);
	}

	//____ _clear() _________________________________________________________________

	void CText::_clear()
	{
		int removed = m_charBuffer.length();
		m_charBuffer.clear();
		_textMapper()->onTextModified(this, 0, removed, 0);
	}

	//___ _set() ____________________________________________________________________

	void CText::_set(const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = seq;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	void CText::_set(const CharBuffer * buffer)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = *buffer;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	void CText::_set(const String& str)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = str;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	//____ _append() ________________________________________________________________

	int CText::_append(const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int ofs = m_charBuffer.length();
		int len = m_charBuffer.pushBack(seq);
		_textMapper()->onTextModified(this, ofs, 0, len);
		return len;
	}

	//____ _insert() ________________________________________________________________

	int CText::_insert(int ofs, const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int len = m_charBuffer.insert(ofs, seq);
		_textMapper()->onTextModified(this, ofs, 0, seq.length());
		return len;
	}

	//____ _replace() ___________________________________________________________

	int CText::_replace(int ofs, int nDelete, const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int diff = m_charBuffer.replace(ofs, nDelete, seq);
		_textMapper()->onTextModified(this, ofs, nDelete, seq.length());
		return diff;
	}

	//____ _erase() ____________________________________________________________

	int CText::_erase(int ofs, int len)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.remove(ofs, len);
		_textMapper()->onTextModified(this, ofs, len, 0);
		return removed;
	}

	//____ _setCharStyle() ______________________________________________________

	void CText::_setCharStyle(TextStyle * pStyle)
	{
		m_charBuffer.setStyle(pStyle);
		_textMapper()->onCharStyleChanged(this);
	}

	void CText::_setCharStyle(TextStyle * pStyle, int ofs, int len)
	{
		m_charBuffer.setStyle(pStyle, ofs, len);
		_textMapper()->onCharStyleChanged(this, ofs, len);
	}

	//____ _clearCharStyle() ____________________________________________________

	void CText::_clearCharStyle()
	{
		m_charBuffer.clearStyle();
		_textMapper()->onCharStyleChanged(this);
	}

	void CText::_clearCharStyle(int ofs, int len)
	{
		m_charBuffer.clearStyle(ofs, len);
		_textMapper()->onCharStyleChanged(this, ofs, len);
	}

	//____ _mapperRequestRender() _______________________________________________

	void CText::_mapperRequestRender(const Rect& rect)
	{
		_requestRender(rect);
	}



} // namespace wg
