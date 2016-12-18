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

#include <algorithm>
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
		_caretToEnd();
	}
	
	void EditTextItem::set( const CharBuffer * pBuffer )
	{
		TextItem::set( pBuffer );
		_caretToEnd();
	}
	
	void EditTextItem::set( const String& str )
	{
		TextItem::set( str );
		_caretToEnd();
	}
	
	//____ append() ________________________________________________________________
	
	int EditTextItem::append( const CharSeq& seq )
	{
		// Appending text should move carret if carret is at end of text and nothing is selected, 
		// otherwise not affect caret or selection.

		if( m_editState.caretOfs == m_editState.selectOfs && m_editState.caretOfs == m_charBuffer.length() )
		{
			int newOfs = m_editState.caretOfs + TextItem::append( seq );
			_textMapper()->caretMove( this, newOfs );
			
			m_editState.caretOfs = newOfs;
			m_editState.selectOfs = newOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();
		}

		return TextItem::append( seq );		
	}
	
	//____ insert() ________________________________________________________________
	
	int EditTextItem::insert( int ofs, const CharSeq& seq )
	{
		limit( ofs, 0, m_charBuffer.length() );

		int added = TextItem::insert(ofs,seq);
		
		/* Inserting text should affect the selection as little as possible. Therefore:
		 * 
		 * Selected text remains selected when new is inserted before or after.
		 * If text is inserted into the selection, the selection grows.
		 * 
		 * Inserting text to position of caretOfs or selectOfs is considered to
		 * insert the text before or after the selection, it will not grow the selection.
		 * 
		 * Inserting text to caretOfs will move caretOfs only if no text is selected.
		 */

		int caretOfs = m_editState.caretOfs;
		int selectOfs = m_editState.selectOfs;
		
		int selBeg = std::min(caretOfs,selectOfs);
		int selEnd = std::max(caretOfs,selectOfs);
		
		if( ofs <= selBeg )				// move selection or caret
		{
			caretOfs += added;
			selectOfs += added;
		}
		else if( ofs < selEnd )			// grow selection
		{
			if( caretOfs == selEnd )
				caretOfs += added;
			else
				selectOfs += added;
		}

		if( caretOfs != m_editState.caretOfs || selectOfs != m_editState.selectOfs )
		{
			if( caretOfs == selectOfs )
				_textMapper()->caretMove( this, caretOfs );
			else
				_textMapper()->selectionChange( this, selectOfs, caretOfs );

			m_editState.caretOfs = caretOfs;
			m_editState.selectOfs = selectOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();			
		}
		return added;
	}
	
	//____ replace() _______________________________________________________________
	
	int EditTextItem::replace( int ofs, int nDelete, const CharSeq& seq )
	{
		//TODO: Implement correctly!!!
		
		
		limit( ofs, 0, m_charBuffer.length() );
		limit( nDelete, 0, m_charBuffer.length() - ofs );
		
		int sizeModif = TextItem::replace(ofs,nDelete,seq);

		// Replacing text should not affect caret or selection except where necessary.

		if( m_editState.caretOfs >= ofs + nDelete )
			m_editState.caretOfs += sizeModif;
		else if( m_editState.caretOfs > ofs )
			m_editState.caretOfs = ofs;
			
		if( m_editState.selectOfs >= ofs + nDelete )
			m_editState.selectOfs += sizeModif;
		else if( m_editState.selectOfs > ofs )
			m_editState.selectOfs = ofs;

		_updateDisplayArea();
		return sizeModif;
	}
	
	//____ erase() ________________________________________________________________
	
	int EditTextItem::erase( int ofs, int len )
	{
		limit( ofs, 0, m_charBuffer.length() );
		limit( len, 0, m_charBuffer.length() - ofs );

		/* Inserting text should affect the selection as little as possible. Therefore:
		 * 
		 * All text that was selected before any text was erased and wasn't erased stays selected.
		 * 
		 * Caret stays where it is if possible. If caret position was erased, caret is moved to
		 * first character after erased section.
		 */
		 
		int caretOfs = m_editState.caretOfs;
		if( caretOfs > ofs )
		{
			if( caretOfs < ofs + len )
				caretOfs = ofs;
			else
				caretOfs -= len;
		}
		
		int selectOfs = m_editState.selectOfs;
		if( selectOfs > ofs )
		{
			if( selectOfs < ofs + len )
				selectOfs = ofs;
			else
				selectOfs -= len;
		}
		
		if( caretOfs != m_editState.caretOfs || selectOfs != m_editState.selectOfs )
		{
			if( m_editState.caretOfs == m_editState.selectOfs )
				_textMapper()->caretMove( this, caretOfs );
			else
				_textMapper()->selectionChange( this, selectOfs, caretOfs );

			m_editState.caretOfs = caretOfs;
			m_editState.selectOfs = selectOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();			
		}		
		
		int ret = TextItem::remove( ofs, len );
		_updateDisplayArea();
		return ret;
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

				_textMapper()->caretMove(this, m_editState.caretOfs);
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
	
	bool EditTextItem::select( int beg, int end )
	{
		if( m_editMode == TextEditMode::Static )
			return false;
			
		int max = m_charBuffer.length();	
			
		limit( beg, 0, max );
		limit( end, 0, max );

		if( beg != m_editState.selectOfs || end != m_editState.caretOfs )
		{
			_textMapper()->selectionChange( this, beg, end );			

			m_editState.selectOfs = beg;
			m_editState.caretOfs = end;
		}
		
		_updateDisplayArea();
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
			_textMapper()->selectionChange( this, m_editState.caretOfs, m_editState.caretOfs );			

			m_editState.selectOfs = m_editState.caretOfs;
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
			
		limit( ofs, 0, m_charBuffer.length() );

		return _moveCaret( ofs, MoveMethod::ApiCall );
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

		int caretOfs = m_editState.caretOfs;
		if( caretOfs == 0 )
		{
			_textMapper()->caretMove(this, 0);
			return false;
		}
		else
		{
			caretOfs--;
			erase(caretOfs, 1);
			return true;
		}
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

	//____ _caretToEnd() __________________________________________________________
	
	void EditTextItem::_caretToEnd()
	{
		m_editState.caretOfs = m_charBuffer.length();		// Caret placed on terminator char following the string.
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
		_updateInsertStyle();
		_updateDisplayArea();
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
		int selectOfs = m_editState.selectOfs;

		if( method == MoveMethod::ApiCall || (method == MoveMethod::Keyboard && !m_editState.bShiftDown) ||
			(method == MoveMethod::Mouse && !(m_editState.bShiftDown || m_editState.bButtonDown)) )
		{
			selectOfs = caretOfs;
		}

		// Notify textmapper of caret and selection changes

		if( m_editState.selectOfs == selectOfs || m_editState.selectOfs != m_editState.caretOfs )
			_textMapper()->selectionChange( this, selectOfs, caretOfs );
		else
			_textMapper()->caretMove( this, caretOfs );

		// Set charStyle to first in selection or character left of caret if there is no selection.
		
		int ofs; 
		if( selectOfs != caretOfs )
			ofs = min( selectOfs, caretOfs );
		else
			ofs = caretOfs > 0 ? caretOfs-1 : 0;
		
		m_editState.pCharStyle = m_charBuffer.chars()[ofs].stylePtr();			

		// Finalize

		bool ret = (m_editState.caretOfs == caretOfs);

		m_editState.caretOfs = caretOfs;
		m_editState.selectOfs = selectOfs;
		_updateInsertStyle();
		_updateDisplayArea();
		
		return ret;
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

		Rect preferred = _textMapper()->rectForRange(this, beg, end - beg);
		Rect prio = _textMapper()->rectForCaret(this);

		_requestVisibility(preferred, prio);
	}

	//____ _updateInsertStyle() ________________________________________________

	void EditTextItem::_updateInsertStyle()
	{
		
	}


	//____ _editState() ________________________________________________________

	const EditState * EditTextItem::_editState() const
	{
		return &m_editState;
	}



} // namespace wg
