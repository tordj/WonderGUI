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

#include <wg_textitem.h>

namespace wg 
{
	
	//____ Constructor _____________________________________________________________
	
	TextItem::TextItem( Widget * pWidget ) : TextBaseItem( pWidget )
	{
	}
	
	//____ clear() _________________________________________________________________
	
	void TextItem::clear()
	{
		int removed = m_charBuffer.length();
		m_charBuffer.clear();
		_textMapper()->onTextModified(this, 0, removed, 0 );
	}
	
	//___ set() ____________________________________________________________________
	
	void TextItem::set( const CharSeq& seq )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()
		
		int removed = m_charBuffer.length();
		m_charBuffer = seq;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length() );
	}
	
	void TextItem::set( const CharBuffer * buffer )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = * buffer;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length() );
	}
	
	void TextItem::set( const String& str )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int removed = m_charBuffer.length();
		m_charBuffer = str;
		_textMapper()->onTextModified(this, 0, removed, m_charBuffer.length() );
	}
	
	//____ append() ________________________________________________________________
	
	int TextItem::append( const CharSeq& seq )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int ofs = m_charBuffer.length();
		int len = m_charBuffer.pushBack(seq);
		_textMapper()->onTextModified(this, ofs, 0, len );
		return len;
	}
	
	//____ insert() ________________________________________________________________
	
	int TextItem::insert( int ofs, const CharSeq& seq )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()
 
		int len = m_charBuffer.insert(ofs,seq);
		_textMapper()->onTextModified(this, ofs, 0, seq.length() );
		return len;
	}
	
	//____ replace() ___________________________________________________________
	
	int TextItem::replace( int ofs, int nDelete, const CharSeq& seq )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()

		int diff = m_charBuffer.replace(ofs,nDelete,seq);
		_textMapper()->onTextModified(this, ofs, nDelete, seq.length() );
		return diff;
	}
	
	//____ delete() ____________________________________________________________
	
	int TextItem::remove( int ofs, int len )
	{
		//TODO: Check and respect boundaries. Guarantee correct parameters to onTextModified()
		
		int removed = m_charBuffer.remove(ofs,len);
		_textMapper()->onTextModified(this, ofs, len, 0 );
		return removed;
	}
	
	//____ markedLink() _____________________________________________________
	
	TextLink_p TextItem::markedLink() const
	{
		//TODO: Implement!

		return TextLink_p();
	}
	
	//____ setCharStyle() ______________________________________________________
	
	void TextItem::setCharStyle( const TextStyle_p& pStyle )
	{
		m_charBuffer.setStyle(pStyle);
		_textMapper()->onCharStyleChanged(this );
	}
	
	void TextItem::setCharStyle( const TextStyle_p& pStyle, int ofs, int len)
	{
		m_charBuffer.setStyle(pStyle, ofs, len);
		_textMapper()->onCharStyleChanged(this, ofs,len);		
	}
	
	//____ clearCharStyle() ____________________________________________________

	void TextItem::clearCharStyle()
	{
		m_charBuffer.clearStyle();
		_textMapper()->onCharStyleChanged(this);		
	}

	void TextItem::clearCharStyle( int ofs, int len )
	{
		m_charBuffer.clearStyle(ofs,len);
		_textMapper()->onCharStyleChanged(this, ofs,len);
	}

	
	//____ receive() ___________________________________________________________

	void TextItem::receive( const Msg_p& pMsg )
	{
		switch( pMsg->type() )
		{			
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				TextLink_p pLink;

				// Get link from character properties

				Coord localPos = static_cast<InputMsg*>( pMsg.rawPtr() )->pointerPos() - _globalPos();				
				int markedChar = _textMapper()->charAtPos(this, localPos);
				if( markedChar >= 0 )
				{
					TextStyle_p pStyle = m_charBuffer.chars()[markedChar].stylePtr();
					if( pStyle )
						pLink = pStyle->combLink();
				}	

				// Fall back to items default style

				if( !pLink && m_pStyle )
					pLink = m_pStyle->combLink();
					

				// Post messages if there was a change in what link we are pointing at

				if( pLink != m_pMarkedLink )
				{
					MsgRouter_p	pRouter = Base::msgRouter();
				
					if( m_pMarkedLink )
						pRouter->post( new LinkMouseLeaveMsg( m_pWidget, m_pMarkedLink ) );
				
					if( pLink )
						pRouter->post( new LinkMouseEnterMsg( m_pWidget, pLink ));
	
					m_pMarkedLink = pLink;
				}
				
				break;
			}
			case MsgType::MouseLeave:

				if( m_pMarkedLink )
				{
					Base::msgRouter()->post( new LinkMouseLeaveMsg( m_pWidget, m_pMarkedLink ) );
					m_pMarkedLink = 0;
				}
				break;

			case MsgType::MousePress:
				if( m_pMarkedLink )
				{
					MouseButton button = static_cast<MousePressMsg*>(pMsg.rawPtr())->button();
					Base::msgRouter()->post( new LinkMousePressMsg( m_pWidget, m_pMarkedLink, button ) );

					if( button == MouseButton::Left )
						pMsg->swallow();
				}
				break;

			case MsgType::MouseDrag:
			
				//TODO: Add swallow under right circumstances.
				break;

			case MsgType::MouseRepeat:
				if( m_pMarkedLink )
				{
					MouseButton button = static_cast<MouseRepeatMsg*>(pMsg.rawPtr())->button();
					Base::msgRouter()->post( new LinkMouseRepeatMsg( m_pWidget, m_pMarkedLink, button ) );			

					if( button == MouseButton::Left )
						pMsg->swallow();
				}
				break;

			case MsgType::MouseRelease:
				if( m_pMarkedLink )
				{
					MouseButton button = static_cast<MouseReleaseMsg*>(pMsg.rawPtr())->button();
					Base::msgRouter()->post( new LinkMouseReleaseMsg( m_pWidget, m_pMarkedLink, button ) );			

					if( button == MouseButton::Left )
						pMsg->swallow();
				}
				break;

			case MsgType::MouseClick:
				if( m_pMarkedLink )
				{
					MouseButton button = static_cast<MouseClickMsg*>(pMsg.rawPtr())->button();
					Base::msgRouter()->post( new LinkMouseClickMsg( m_pWidget, m_pMarkedLink, button ) );			

					if( button == MouseButton::Left )
					{
						Base::msgRouter()->post( new LinkSelectMsg( m_pWidget, m_pMarkedLink ) );			
						pMsg->swallow();
					}
				}
				break;

			case MsgType::MouseDoubleClick:
				if( m_pMarkedLink )
				{
					MouseButton button = static_cast<MouseDoubleClickMsg*>(pMsg.rawPtr())->button();
					Base::msgRouter()->post( new LinkMouseDoubleClickMsg( m_pWidget, m_pMarkedLink, button ) );			

					if( button == MouseButton::Left )
						pMsg->swallow();
				}
				break;
            
            default:
                break;
		}
	}

	

} // namespace wg
