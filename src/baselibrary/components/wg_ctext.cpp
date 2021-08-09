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

	const TypeInfo CText::TYPEINFO = { "CText", &WidgetComponent::TYPEINFO };

	//____ constructor _____________________________________________________________

	CText::CText(Widget * pWidget ) : WidgetComponent( pWidget )
	{
		_layout()->addText(this);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CText::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ Destructor ______________________________________________________________

	CText::~CText()
	{
		_layout()->removeText(this);
	}

	//____ setStyle() ______________________________________________________________

	void CText::setStyle( TextStyle * pStyle )
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = pStyle;
		_layout()->onStyleChanged(this, pStyle, pOld);
	}

	//____ clearStyle() ____________________________________________________________

	void CText::clearStyle()
	{
		TextStyle_p pOld = m_pStyle;			// Keep ref count until onStyleChanged has been called.
		m_pStyle = 0;
		_layout()->onStyleChanged(this, nullptr, pOld);
	}

	//____ setLayout() __________________________________________________________

	void CText::setLayout( TextLayout * pLayout )
	{
		if( pLayout == m_pLayout )
			return;

		_layout()->removeText(this);
		m_pLayout = pLayout;
		_layout()->addText(this);
	}

	//____ clearLayout() ________________________________________________________

	void CText::clearLayout()
	{
		if( !m_pLayout )
			return;

		_layout()->removeText(this);
		m_pLayout = 0;
		_layout()->addText(this);
	}

	//____ _setState() ______________________________________________________________

	void CText::_setState( State state )
	{
		if( state == m_state )
			return;

		State old = m_state;
		m_state = state;
		_layout()->onStateChanged( this, state, old );
	}

	//____ _preferredSize() _________________________________________________________

	SizeSPX CText::_preferredSize(int scale) const
	{
		return _layout()->preferredSize(this, scale);
	}

	//____ _matchingWidth() _________________________________________________________

	spx CText::_matchingWidth( spx height, int scale ) const
	{
		return _layout()->matchingWidth(this, height, scale);
	}

	//____ _matchingHeight() ________________________________________________________

	spx CText::_matchingHeight( spx width, int scale ) const
	{
		return _layout()->matchingHeight(this, width, scale);
	}

	//____ _charAtPos() ___________________________________________________________

	int CText::_charAtPos( CoordSPX pos ) const
	{
		return _layout()->charAtPos(this,pos);
	}

	//____ _charRect() ____________________________________________________________

	RectSPX CText::_charRect( int charOfs ) const
	{
		return _layout()->charRect(this, charOfs);
	}

	//____ _charLine() ____________________________________________________________

	int CText::_charLine( int charOfs ) const
	{
		return _layout()->charLine(this, charOfs);
	}


	//____ _refresh() _____________________________________________________________

	void CText::_refresh()
	{
		_layout()->onRefresh(this);
	}

	//____ _setSize() _____________________________________________________________

	void CText::_setSize( SizeSPX size, int scale )
	{
		if( size == m_size )
			return;

		SizeSPX oldSize = m_size;
		int		oldScale = scale;
		m_size = size;
		m_scale = scale;
		_layout()->onResized(this,size, oldSize, scale, oldScale);
	}

	//_____ _render() _____________________________________________________________

	void  CText::_render( GfxDevice * pDevice, const RectSPX& _canvas )
	{
		_layout()->render(this, pDevice, _canvas );
	}

	//____ _rectForRange() __________________________________________________________

	RectSPX  CText::_rectForRange( int ofs, int length ) const
	{
		return _layout()->rectForRange(this, ofs, length);
	}

	//____ _tooltip() _______________________________________________________________

	String CText::_tooltip() const
	{
		return _layout()->tooltip(this);
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
		_layout()->onTextModified(this, 0, removed, 0);
	}

	//___ _setText() ____________________________________________________________________

	void CText::_setText(const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = seq;
		_layout()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	void CText::_setText(const CharBuffer * buffer)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = *buffer;
		_layout()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	void CText::_setText(const String& str)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = str;
		_layout()->onTextModified(this, 0, removed, m_charBuffer.length());
	}

	//____ _append() ________________________________________________________________

	int CText::_append(const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int ofs = m_charBuffer.length();
		int len = m_charBuffer.pushBack(seq);
		_layout()->onTextModified(this, ofs, 0, len);
		return len;
	}

	//____ _insert() ________________________________________________________________

	int CText::_insert(int ofs, const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int len = m_charBuffer.insert(ofs, seq);
		_layout()->onTextModified(this, ofs, 0, seq.length());
		return len;
	}

	//____ _replace() ___________________________________________________________

	int CText::_replace(int ofs, int nDelete, const CharSeq& seq)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int diff = m_charBuffer.replace(ofs, nDelete, seq);
		_layout()->onTextModified(this, ofs, nDelete, seq.length());
		return diff;
	}

	//____ _erase() ____________________________________________________________

	int CText::_erase(int ofs, int len)
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.remove(ofs, len);
		_layout()->onTextModified(this, ofs, len, 0);
		return removed;
	}

	//____ _setCharStyle() ______________________________________________________

	void CText::_setCharStyle(TextStyle * pStyle)
	{
		m_charBuffer.setStyle(pStyle);
		_layout()->onCharStyleChanged(this);
	}

	void CText::_setCharStyle(TextStyle * pStyle, int ofs, int len)
	{
		m_charBuffer.setStyle(pStyle, ofs, len);
		_layout()->onCharStyleChanged(this, ofs, len);
	}

	//____ _clearCharStyle() ____________________________________________________

	void CText::_clearCharStyle()
	{
		m_charBuffer.clearStyle();
		_layout()->onCharStyleChanged(this);
	}

	void CText::_clearCharStyle(int ofs, int len)
	{
		m_charBuffer.clearStyle(ofs, len);
		_layout()->onCharStyleChanged(this, ofs, len);
	}

	//____ _mapperRequestRender() _______________________________________________

	void CText::_mapperRequestRender(const RectSPX& rect)
	{
		_requestRender(rect);
	}



} // namespace wg
