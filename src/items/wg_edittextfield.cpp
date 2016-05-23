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

#include <wg_edittextfield.h>
#include <wg_charseq.h>

namespace wg 
{
	
	
	//____ Constructor _____________________________________________________________
	
	EditTextField::EditTextField( EditTextHolder * pHolder ) : TextField(pHolder)
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
		m_editMode = TextEditMode::Editable;
	}
	
	
	//____ clear() _________________________________________________________________
	
	void EditTextField::clear()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
	
		TextField::clear();
	}
	
	//____ set() ___________________________________________________________________
	
	void EditTextField::set( const CharSeq& seq )
	{
		TextField::set( seq );
		_caretToEnd();
	}
	
	void EditTextField::set( const CharBuffer * pBuffer )
	{
		TextField::set( pBuffer );
		_caretToEnd();
	}
	
	void EditTextField::set( const String& str )
	{
		TextField::set( str );
		_caretToEnd();
	}
	
	//____ append() ________________________________________________________________
	
	int EditTextField::append( const CharSeq& seq )
	{
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
				
		return TextField::append( seq );
	}
	
	//____ insert() ________________________________________________________________
	
	int EditTextField::insert( int ofs, const CharSeq& seq )
	{
		if( ofs <= m_editState.caretOfs )
			m_editState.caretOfs += seq.length();
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
	
		return TextField::insert(ofs,seq);
	}
	
	//____ replace() _______________________________________________________________
	
	int EditTextField::replace( int ofs, int nDelete, const CharSeq& seq )
	{
		int caretOfs = m_editState.caretOfs;
		if( caretOfs >= ofs )
		{
			if( caretOfs < ofs + nDelete )
				caretOfs = ofs + seq.length();
			else
				caretOfs = caretOfs - nDelete + seq.length();
		}
		m_editState.caretOfs = caretOfs;
		m_editState.selectOfs = caretOfs;
		m_editState.wantedOfs = -1;
	
		return TextField::replace(ofs,nDelete,seq);
	}
	
	//____ erase() ________________________________________________________________
	
	int EditTextField::erase( int ofs, int len )
	{
		int caretOfs = m_editState.caretOfs;
		if( caretOfs > ofs )
		{
			if( caretOfs < ofs + len )
				caretOfs = ofs;
			else
				caretOfs -= len;
		}
		m_editState.caretOfs = caretOfs;
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
		
		return TextField::remove( ofs, len );
	}
	
	//____ setState() ______________________________________________________________
	
	void EditTextField::setState( State state )
	{
		TextField::setState(state);
	
		if( state.isFocused() && m_editMode == TextEditMode::Editable )
		{
			m_editState.bCaret = true;
			m_editState.caretOfs = m_charBuffer.length();		//TODO: Not always reset caretOfs.
			m_editState.selectOfs = m_editState.caretOfs;
			m_editState.wantedOfs = -1;
		}
		else
			m_editState.bCaret = false;
	}
	
	
	//____ onRender() ______________________________________________________________
	
	void EditTextField::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_printer()->renderField(this, pDevice, _canvas, _clip);
		
		Caret_p pCaret = _style()->combCaret();
		
		if( m_editState.bCaret && pCaret )
		{
			pCaret->render( pDevice, _printer()->charToRect(this, m_editState.caretOfs) + _canvas.pos(), _clip );
		}
	}
	
	//____ setEditMode() _______________________________________________________
	
	void EditTextField::setEditMode( TextEditMode mode )
	{
		switch( mode )
		{
			case TextEditMode::Static:
			break;
			case TextEditMode::Selectable:
			break;
			case TextEditMode::Editable:
				if( m_editState.selectOfs != m_editState.caretOfs )
				{
					
				}
			break;
		}

		m_editMode = mode;
	}

	//____ select() ____________________________________________________________
	
	bool EditTextField::select( int begin, int end )
	{
		if( m_editMode == TextEditMode::Static )
			return false;
			
		int max = m_charBuffer.length();	
			
		limit( begin, 0, max );
		limit( end, 0, max );

		if( begin != m_editState.selectOfs || end != m_editState.caretOfs )
		{
			m_editState.selectOfs = begin;
			m_editState.caretOfs = end;
			_onDirty();						//TODO: Optimize. Only render parts that have been selected or unselected.

			//TODO: Signal that selection (and possibly cursor position) has changed.
		}
		
		return true;
	}

	//____ selectAll() _________________________________________________________
	
	bool EditTextField::selectAll()
	{
		return select( 0, m_charBuffer.length() );
	}
	
	//____ unselect() __________________________________________________________

	bool EditTextField::unselect()
	{
		if( m_editMode == TextEditMode::Static )
			return false;

		if( m_editState.selectOfs != m_editState.caretOfs )
		{
			m_editState.selectOfs = m_editState.caretOfs;
			_onDirty();						//TODO: Optimize. Only render parts that have been selected or unselected.

			//TODO: Signal that selection (and possibly cursor position) has changed.
		}
		
		return true;		
	}
	
	//____ selectionBegin() ____________________________________________________

	int EditTextField::selectionBegin() const
	{
		return m_editState.caretOfs < m_editState.selectOfs ? m_editState.caretOfs : m_editState.selectOfs;
	}
	
	//____ selectionEnd() ______________________________________________________

	int EditTextField::selectionEnd() const
	{
		return m_editState.caretOfs > m_editState.selectOfs ? m_editState.caretOfs : m_editState.selectOfs;		
	}
	
	//____ eraseSelected() _____________________________________________________

	int EditTextField::eraseSelected()
	{
		if( m_editMode == TextEditMode::Static )
			return -1;
			
		int beg = m_editState.selectOfs;
		int end = m_editState.caretOfs;

		if( beg == end )
			return 0;

		if( beg < end )
			std::swap( beg, end );

		erase( beg, end - beg );		
	}


	//____ setCaretPos() _______________________________________________________
	
	bool EditTextField::setCaretPos( int pos )
	{
		if( m_editMode != TextEditMode::Editable )
			return false;
			
		int max = m_charBuffer.length();	
			
		limit( pos, 0, max );

		if( pos != m_editState.selectOfs || pos != m_editState.caretOfs )
		{
			m_editState.selectOfs = pos;
			m_editState.caretOfs = pos;
			m_editState.wantedOfs = -1;
			_onDirty();						//TODO: Optimize. Only render parts that have been selected or unselected.

			//TODO: Signal that selection (and possibly caret position) has changed.
		}

		return true;
	}

	//____ caretPos() ___________________________________________________________

	int EditTextField::caretPos() const
	{
		if( !m_editState.bCaret )
			return -1;
	
		return m_editState.caretOfs;
	}
	//____ caretPut() __________________________________________________________

	int EditTextField::caretPut( const CharSeq& seq )
	{
		if( !m_editState.bCaret )
			return 0;

		if( m_editState.caretOfs != m_editState.selectOfs )		// Selection should be replaced with put content.
		{
			int beg = m_editState.selectOfs;
			int end = m_editState.caretOfs;

			if( beg < end )
				std::swap( beg, end );

			return replace( beg, end-beg, seq );					
		}
		else													// Just insert the put content
			return insert( m_editState.caretOfs, seq );			
	}
	
	bool EditTextField::caretPut( uint16_t c )
	{
		//TODO: Implement
		return false;
	}

	//_____ caretLineBegin() ___________________________________________________

	bool EditTextField::caretLineBegin()
	{
		if( !m_editState.bCaret )
			return false;

		int ofs = _printer()->lineBegin( this, _printer()->lineOfChar( this, m_editState.caretOfs ));

		if( ofs != m_editState.caretOfs || ofs != m_editState.selectOfs )
		{
			m_editState.caretOfs = ofs;
			m_editState.selectOfs = ofs;
			m_editState.wantedOfs = -1;
			_onDirty();
		}		
		
		return true;
	}
	
	//____ caretLineEnd() ______________________________________________________
	
	bool EditTextField::caretLineEnd()
	{
		if( !m_editState.bCaret )
			return false;

		int ofs = _printer()->lineEnd( this, _printer()->lineOfChar( this, m_editState.caretOfs )) -1;

		if( ofs != m_editState.caretOfs || ofs != m_editState.selectOfs )
		{
			m_editState.caretOfs = ofs;
			m_editState.selectOfs = ofs;
			m_editState.wantedOfs = -1;
			_onDirty();
		}
		
		return true;
	}

	//____ caretTextBegin() ____________________________________________________
	
	bool EditTextField::caretTextBegin()
	{
		if( !m_editState.bCaret )
			return false;
		
		_caretToBegin();
		return true;		
	}
	
	//____ caretTextEnd() ______________________________________________________
	
	bool EditTextField::caretTextEnd()
	{
		if( !m_editState.bCaret )
			return false;
		
		_caretToEnd();
		return true;
	}

	
	//____ _caretToBegin() __________________________________________________________
	
	void EditTextField::_caretToBegin()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;	
	}

	//____ _caretToEnd() __________________________________________________________
	
	void EditTextField::_caretToEnd()
	{
		int lastChar = m_charBuffer.length();		// The text terminator is considered part of the text when placing the cursor.
		m_editState.caretOfs = lastChar;
		m_editState.selectOfs = lastChar;
		m_editState.wantedOfs = -1;	
	}

} // namespace wg
