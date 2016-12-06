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

#include <wg_edittextitem.h>
#include <wg_charseq.h>

// TODO: Go to next/previous word.
// TODO: Correctly render selected text.
// TODO: Move cursor by mouse, with selection.
// TODO: Optimize rendering.
// TODO: Optimize text changes.
// TODO: Double/tripple click to select word/line
// TODO: Support for cut/copy/paste
// TODO: Support for undo/redo
// TODO: Cursor have correct size.
// TODO: Check that we request resize properly.


namespace wg 
{
	
	
	//____ Constructor _____________________________________________________________
	
	EditTextItem::EditTextItem( Widget * pWidget ) : TextItem(pWidget)
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
		m_editMode = TextEditMode::Editable;
	}

	//____ receive() ___________________________________________________________

	void EditTextItem::receive( const Msg_p& pMsg )
	{
		TextItem::receive( pMsg );

		MsgType type = pMsg->type();
		
		switch( type )
		{			
			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			{
				if( m_editState.bCaret )
				{
					KeyMsg_p p = KeyMsg::cast(pMsg);
					ModifierKeys modKeys = p->modKeys();
				
					switch( p->translatedKeyCode() )
					{
					case Key::Shift:
						m_editState.bShiftDown = true;
						break;
					case Key::Left:	
						if( modKeys & MODKEY_CTRL )
							caretPrevWord();
						else
							caretLeft();
						break;
					case Key::Right:
						if( modKeys & MODKEY_CTRL )
							caretNextWord();
						else
							caretRight();
						break;
		
					case Key::Up:
							caretUp();
						break;
		
					case Key::Down:
							caretDown();
						break;
		
					case Key::Backspace:
						if( hasSelection() )
							eraseSelected();
						else if( modKeys & MODKEY_CTRL )
							caretErasePrevWord();
						else
							caretErasePrevChar();
						break;
		
					case Key::Delete:
						if( hasSelection() )
							eraseSelected();
						else if( modKeys & MODKEY_CTRL )
							caretEraseNextWord();
						else
							caretEraseNextChar();
						break;
		
					case Key::Home:
						if( modKeys & MODKEY_CTRL )
							caretTextBegin();
						else
							caretLineBegin();
						break;
		
					case Key::End:
						if( modKeys & MODKEY_CTRL )
							caretTextEnd();
						else
							caretLineEnd();
						break;

					case Key::Return:
						caretPut( "\n" );
						break;
		
					default:
						break;

					}

				}
				break;
			}
			
			case MsgType::KeyRelease:
				if( KeyMsg::cast(pMsg)->translatedKeyCode() == Key::Shift )
					m_editState.bShiftDown = false;
				break;

			case MsgType::TextInput:
				caretPut( TextInputMsg::cast(pMsg)->text() );				
				break;
	
			case MsgType::EditCommand:
			{
				EditCmd cmd = EditCommandMsg::cast(pMsg)->command();
			
				switch( cmd )
				{
					case EditCmd::Cut:
					break;
					
					case EditCmd::Copy:
					break;
					
					case EditCmd::Paste:
					break;
					
					case EditCmd::Undo:
					break;
					
					case EditCmd::Redo:
					break;
					
					case EditCmd::SelectAll:
						selectAll();
					break;
					
					default:
					break;
				}
			
				break;
			}

			case MsgType::MousePress:			// Move caret pos and selection pos. Shift-press: only move caret pos.
			{
				auto p = static_cast<MousePressMsg*>(pMsg.rawPtr());
				if( p->button() == MouseButton::Left )
				{
					caretToPos(p->pointerPos() - _globalPos());
					m_editState.bButtonDown = true;
				}
				break;
			}

			case MsgType::MouseDrag:			// Move only caret pos
			{	
				auto p = static_cast<MouseDragMsg*>(pMsg.rawPtr());
				if( p->button() == MouseButton::Left )
					caretToPos(p->pointerPos() - _globalPos());
				break;
			}
			
			case MsgType::MouseRelease:
			{
				auto p = static_cast<MouseReleaseMsg*>(pMsg.rawPtr());
				if( p->button() == MouseButton::Left )
					m_editState.bButtonDown = false;
				break;
			}
			case MsgType::MouseDoubleClick:		// Select word
			{
				auto p = static_cast<MouseDoubleClickMsg*>(pMsg.rawPtr());
				if( p->button() == MouseButton::Left )
					caretSelectWord();
				break;
			}

			default:
				break;
				
		}
	}

	
	
	//____ clear() _________________________________________________________________
	
	void EditTextItem::clear()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;
	
		TextItem::clear();
		_updateDisplayArea();
	}
	
	//____ set() ___________________________________________________________________
	
	void EditTextItem::set( const CharSeq& seq )
	{
		TextItem::set( seq );
		_caretToEnd( MoveMethod::ApiCall );
	}
	
	void EditTextItem::set( const CharBuffer * pBuffer )
	{
		TextItem::set( pBuffer );
		_caretToEnd( MoveMethod::ApiCall );
	}
	
	void EditTextItem::set( const String& str )
	{
		TextItem::set( str );
		_caretToEnd( MoveMethod::ApiCall );
	}
	
	//____ append() ________________________________________________________________
	
	int EditTextItem::append( const CharSeq& seq )
	{
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
				
		return TextItem::append( seq );
	}
	
	//____ insert() ________________________________________________________________
	
	int EditTextItem::insert( int ofs, const CharSeq& seq )
	{
		if( ofs <= m_editState.caretOfs )
			m_editState.caretOfs += seq.length();
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
	
		return TextItem::insert(ofs,seq);
	}
	
	//____ replace() _______________________________________________________________
	
	int EditTextItem::replace( int ofs, int nDelete, const CharSeq& seq )
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
	
		return TextItem::replace(ofs,nDelete,seq);
	}
	
	//____ erase() ________________________________________________________________
	
	int EditTextItem::erase( int ofs, int len )
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
		
		return TextItem::remove( ofs, len );
	}
	
	//____ setState() ______________________________________________________________
	
	void EditTextItem::setState( State state )
	{
		if( state.isFocused() != m_state.isFocused() && m_editMode == TextEditMode::Editable )
		{
			if( state.isFocused() )
			{
				// Enable and place caret
				
				m_editState.bCaret = true;
				m_editState.caretOfs = m_charBuffer.length();		//TODO: Not always reset caretOfs.
				m_editState.selectOfs = m_editState.caretOfs;
				m_editState.wantedOfs = -1;

				// Update carets charstyle

				int ofs = m_editState.caretOfs > 0 ? m_editState.caretOfs-1 : 0;
				m_editState.pCharStyle = m_charBuffer.chars()[ofs].stylePtr();

				// Check modifier keys, update status

				ModifierKeys modKeys = Base::inputHandler()->modifierKeys();

				if( modKeys & MODKEY_SHIFT )
					m_editState.bShiftDown = true;
				else
					m_editState.bShiftDown = false;

				// Restart caret animation

				_textMapper()->pokeCaret(this);		// TODO: Should not be needed here anymore.
			}
			else
			{
				m_editState.bCaret = false;				
			}
			
		}
			
		
		// Set this last, so that bCaret is set when we call
		
		TextItem::setState(state);
	}
	
	
	//____ onRender() ______________________________________________________________
	
	void EditTextItem::onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip )
	{
		_textMapper()->renderItem(this, pDevice, _canvas, _clip);		
	}
	
	//____ setEditMode() _______________________________________________________
	
	void EditTextItem::setEditMode( TextEditMode mode )
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
	
	bool EditTextItem::select( int begin, int end )
	{
		if( m_editMode == TextEditMode::Static )
			return false;
			
		int max = m_charBuffer.length();	
			
		limit( begin, 0, max );
		limit( end, 0, max );

		if( begin != m_editState.selectOfs || end != m_editState.caretOfs )
		{
			int oldSel = m_editState.selectOfs;
			int oldCaret = m_editState.caretOfs;
			
			m_editState.selectOfs = begin;
			m_editState.caretOfs = end;
			
			_textMapper()->selectionChanged( this, oldSel, oldCaret );			
		}
		
		return true;
	}

	//____ selectAll() _________________________________________________________
	
	bool EditTextItem::selectAll()
	{
		return select( 0, m_charBuffer.length() );
	}
	
	//____ unselect() __________________________________________________________

	bool EditTextItem::unselect()
	{
		if( m_editMode == TextEditMode::Static )
			return false;

		if( m_editState.selectOfs != m_editState.caretOfs )
		{
			m_editState.selectOfs = m_editState.caretOfs;
			_requestRender();						//TODO: Optimize. Only render parts that have been selected or unselected.

			//TODO: Signal that selection (and possibly cursor position) has changed.
		}
		
		return true;		
	}
	
	//____ selectionBegin() ____________________________________________________

	int EditTextItem::selectionBegin() const
	{
		return m_editState.selectOfs;
	}
	
	//____ selectionEnd() ______________________________________________________

	int EditTextItem::selectionEnd() const
	{
		return m_editState.caretOfs;		
	}
	
	//____ eraseSelected() _____________________________________________________

	int EditTextItem::eraseSelected()
	{
		if( m_editMode == TextEditMode::Static )
			return -1;
			
		int beg = m_editState.selectOfs;
		int end = m_editState.caretOfs;

		if( beg == end )
			return 0;

		if( beg > end )
			std::swap( beg, end );

		return erase( beg, end - beg );
	}


	//____ setCaretPos() _______________________________________________________
	
	bool EditTextItem::setCaretOfs( int ofs )
	{
		if( m_editMode != TextEditMode::Editable )
			return false;
			
		int max = m_charBuffer.length();	
			
		limit( ofs, 0, max );

		if( ofs != m_editState.selectOfs || ofs != m_editState.caretOfs )
		{
			m_editState.selectOfs = ofs;
			m_editState.caretOfs = ofs;
			m_editState.wantedOfs = -1;
			_requestRender();						//TODO: Optimize. Only render parts that have been selected or unselected.

			//TODO: Signal that selection (and possibly caret position) has changed.
		}

		return true;
	}

	//____ caretPos() ___________________________________________________________

	int EditTextItem::caretOfs() const
	{
		if( !m_editState.bCaret )
			return -1;
	
		return m_editState.caretOfs;
	}
	//____ caretPut() __________________________________________________________

	int EditTextItem::caretPut( const CharSeq& seq )
	{
		if( !m_editState.bCaret )
			return 0;

		// Apply carets styling to all unstyled characters (this is a slow way of doing it)

		CharBuffer 	buffer;
		buffer.pushBack(seq);

		if( m_editState.pCharStyle )
		{
			for( int i = 0 ; i < buffer.length() ; i++ )
				if( buffer[i].styleHandle() == 0 )
					buffer[i].setStyle( m_editState.pCharStyle );			
		}

		//
		int retVal;

		if( m_editState.caretOfs != m_editState.selectOfs )		// Selection should be replaced with put content.
		{
			int beg = m_editState.selectOfs;
			int end = m_editState.caretOfs;

			if( beg > end )
				std::swap( beg, end );

			retVal = replace( beg, end-beg, &buffer );					
		}
		else													// Just insert the put content
			retVal = insert( m_editState.caretOfs, &buffer );

		_updateDisplayArea();
		return retVal;
	}
	
	bool EditTextItem::caretPut( uint16_t c )
	{
		//TODO: Implement
		return false;
	}

	//____ caretUp() ___________________________________________________________

	bool EditTextItem::caretUp()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = _textMapper()->caretUp(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}
	
	//____ caretDown() _________________________________________________________

	bool EditTextItem::caretDown()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = _textMapper()->caretDown(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}
	
	//____ caretLeft() _________________________________________________________

	bool EditTextItem::caretLeft()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretLeft(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretRight() ________________________________________________________
	
	bool EditTextItem::caretRight()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = _textMapper()->caretRight(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}
	
	//____ caretNextWord() _____________________________________________________
	
	bool EditTextItem::caretNextWord()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretNextWord(this, m_editState.caretOfs);
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretPrevWord() _____________________________________________________
	
	bool EditTextItem::caretPrevWord()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretPrevWord(this, m_editState.caretOfs);
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretEraseNextChar() ________________________________________________
		
	bool EditTextItem::caretEraseNextChar()
	{
		if( !m_editState.bCaret )
			return false;

		if( m_editState.caretOfs != m_editState.selectOfs )		// Selection should be replaced with put content.
		{
			eraseSelected();
			return true;
		}
		else if( m_editState.caretOfs < m_charBuffer.length() )
		{
			erase( m_editState.caretOfs, 1 );
			return true;
		}

		return false;
	}

	//____ caretErasePrevChar() ________________________________________________

	bool EditTextItem::caretErasePrevChar()
	{
		if( !m_editState.bCaret )
			return false;

		if( m_editState.caretOfs != m_editState.selectOfs )		// Selection should be replaced with put content.
		{
			eraseSelected();
			return true;
		}
		else if( m_editState.caretOfs > 0 )							
		{
			m_editState.caretOfs--;
			erase( m_editState.caretOfs, 1 );
			return true;
		}

		return false;
	}

	//____ caretEraseNextWord() ________________________________________________

	bool EditTextItem::caretEraseNextWord()
	{
		return false; //TODO: Implement!		
	}

	//____ caretErasePrevWord() ________________________________________________

	bool EditTextItem::caretErasePrevWord()
	{
		return false; //TODO: Implement!		
	}




	//_____ caretLineBegin() ___________________________________________________

	bool EditTextItem::caretLineBegin()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = _textMapper()->caretHome(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}
	
	//____ caretLineEnd() ______________________________________________________
	
	bool EditTextItem::caretLineEnd()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = _textMapper()->caretEnd(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretTextBegin() ____________________________________________________
	
	bool EditTextItem::caretTextBegin()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = 0;
		m_editState.wantedOfs = -1;

		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}
	
	//____ caretTextEnd() ______________________________________________________
	
	bool EditTextItem::caretTextEnd()
	{
		if( !m_editState.bCaret )
			return false;
		
		int caretOfs = m_charBuffer.length();		// Caret placed on terminator char following the string.
		m_editState.wantedOfs = -1;

		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	
	//____ _caretToBegin() __________________________________________________________
	
	bool EditTextItem::_caretToBegin( MoveMethod method )
	{
		int caretOfs = 0;
		m_editState.wantedOfs = -1;

		return _moveCaret(caretOfs, method);
	}

	//____ _caretToEnd() __________________________________________________________
	
	bool EditTextItem::_caretToEnd( MoveMethod method )
	{
		int caretOfs = m_charBuffer.length();		// Caret placed on terminator char following the string.
		m_editState.wantedOfs = -1;

		return _moveCaret(caretOfs, method);
	}


	//____ _caretToPos() __________________________________________________________

	bool EditTextItem::caretToPos( Coord pos)
	{
		int ofs = _textMapper()->caretToPos(this,pos, m_editState.wantedOfs );
		return _moveCaret( ofs, MoveMethod::Mouse );
	}

	//____ _caretSelectWord() __________________________________________________
	
	bool EditTextItem::caretSelectWord()
	{
		//TODO: Implement!
		return false;
	}

	//____ _caretSelectLine() __________________________________________________
	
	bool EditTextItem::caretSelectLine()
	{
		//TODO: Implement!
		return false;
	}


	//____ _moveCaret() ______________________________________________

	bool EditTextItem::_moveCaret( int caretOfs, MoveMethod method )
	{
		bool retVal;

		if( caretOfs != m_editState.caretOfs )
		{
			int oldSelectOfs = m_editState.selectOfs;
			int oldCaretOfs = m_editState.caretOfs;

			m_editState.caretOfs = caretOfs;

			if( method == MoveMethod::ApiCall || (method == MoveMethod::Keyboard && !m_editState.bShiftDown) ||
				(method == MoveMethod::Mouse && !(m_editState.bShiftDown || m_editState.bButtonDown)) )
			{
				m_editState.selectOfs = caretOfs;
			}

			// Notify textmapper of caret and selection changes

			if( m_editState.selectOfs == oldSelectOfs || oldSelectOfs != oldCaretOfs )
				_textMapper()->selectionChanged( this, oldSelectOfs, oldCaretOfs );
			else
				_textMapper()->caretMoved( this, oldCaretOfs );
			
			
			// Set charStyle to first in selection or character left of caret if there is no selection.
			
			int ofs; 
			if( m_editState.selectOfs != m_editState.caretOfs )
				ofs = min( m_editState.selectOfs, m_editState.caretOfs );
			else
				ofs = caretOfs > 0 ? caretOfs-1 : 0;
			
			m_editState.pCharStyle = m_charBuffer.chars()[ofs].stylePtr();			

			retVal = true;
		}
		else
		{
			_textMapper()->pokeCaret( this );					// Animation sequence should restart on every caret move.
			retVal = false;								// Caret was not moved.
		}

		_updateDisplayArea();
		return retVal;
	}

	//____ _updateDisplayArea() _______________________________________________

	void EditTextItem::_updateDisplayArea()
	{
		// Make sure caret and surrounding characters are visible (and preferably whole selection, if any)
		// 

		int beg = m_editState.caretOfs;
		int end = m_editState.selectOfs;

		if (end < beg)
			std::swap(beg, end);

		if (beg > 0)
			beg--;
		if (end < m_charBuffer.length())
			end++;

		Rect preferred = _textMapper()->rectForRange(this, beg, end - beg);

		beg = m_editState.caretOfs;
		end = m_editState.caretOfs;

		if (beg > 0)
			beg--;
		if (end < m_charBuffer.length())
			end++;

		Rect prio = _textMapper()->rectForRange(this, beg, end - beg);

		_requestVisibility(preferred, prio);
	}

	//____ _editState() ________________________________________________________

	const EditState * EditTextItem::_editState() const
	{
		return &m_editState;
	}



} // namespace wg
