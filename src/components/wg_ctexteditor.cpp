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
#include <wg_ctexteditor.h>
#include <wg_charseq.h>
#include <wg_msg.h>
#include <wg_inputhandler.h>
#include <wg_util.h>

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
	using namespace Util;

	const TypeInfo CTextEditor::TYPEINFO = { "CTextEditor", &CTextDisplay::TYPEINFO };

	//____ constructor _____________________________________________________________

	CTextEditor::CTextEditor(Holder * pHolder ) : CTextDisplay(pHolder)
	{
		m_editMode = TextEditMode::Editable;
		m_maxLines = 0;
		m_maxChars = 0;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CTextEditor::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setMaxLines() _________________________________________________________

	bool CTextEditor::setMaxLines(int maxLines)
	{
		if (maxLines < 0)
			return false;

		m_maxLines = maxLines;

		//TODO: Cut text and refresh if larger than limit.

		return true;
	}

	//____ setMaxChars() _________________________________________________________

	bool CTextEditor::setMaxChars(int maxChars)
	{
		if (maxChars < 0)
			return false;

		m_maxChars = maxChars;

		//TODO: Cut text and refresh if larger than limit.

		return true;
	}


	//____ _receive() ___________________________________________________________

	void CTextEditor::_receive( Msg * pMsg )
	{
		CTextDisplay::_receive( pMsg );

		MsgType type = pMsg->type();

		switch( type )
		{
			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			{
				if( m_editState.bCaret )
				{
					KeyMsg * p = static_cast<KeyMsg*>(pMsg);
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
				if(static_cast<KeyMsg*>(pMsg)->translatedKeyCode() == Key::Shift )
					m_editState.bShiftDown = false;
				break;

			case MsgType::TextInput:
				caretPut(static_cast<TextInputMsg*>(pMsg)->text() );
				break;

			case MsgType::EditCommand:
			{
				EditCmd cmd = static_cast<EditCommandMsg*>(pMsg)->command();

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
				auto p = static_cast<MousePressMsg*>(pMsg);
				if( p->button() == MouseButton::Left && m_editMode != TextEditMode::Static )
				{
					if( m_state.isFocused() )
					{
						_caretToPos(p->pointerPosRaw() - _globalPos());
						m_editState.bButtonDown = true;
					}
					else
						_requestFocus();
				}
				break;
			}

			case MsgType::MouseDrag:			// Move only caret pos
			{
				auto p = static_cast<MouseDragMsg*>(pMsg);
				if( p->button() == MouseButton::Left && m_editMode != TextEditMode::Static )
					if( m_state.isFocused() )
						_caretToPos(p->pointerPosRaw() - _globalPos());
				break;
			}

			case MsgType::MouseRelease:
			{
				auto p = static_cast<MouseReleaseMsg*>(pMsg);
				if( p->button() == MouseButton::Left && m_editMode != TextEditMode::Static )
					m_editState.bButtonDown = false;
				break;
			}
			case MsgType::MouseDoubleClick:		// Select word
			{
				auto p = static_cast<MouseDoubleClickMsg*>(pMsg);
				if( p->button() == MouseButton::Left && m_editMode != TextEditMode::Static )
					if( m_state.isFocused() )
						caretSelectWord();
				break;
			}

			default:
				break;

		}
	}



	//____ _clear() _________________________________________________________________

	void CTextEditor::_clear()
	{
		m_editState.caretOfs = 0;
		m_editState.selectOfs = 0;
		m_editState.wantedOfs = -1;

		CTextDisplay::_clear();
		_updateDisplayArea();
	}

	//____ _set() ___________________________________________________________________

	void CTextEditor::_set( const CharSeq& seq )
	{
		//TODO: Cut sequence if too many lines or chars.

		CTextDisplay::_set( seq );
		_caretToEnd();
	}

	void CTextEditor::_set( const CharBuffer * pBuffer )
	{
		//TODO: Cut sequence if too many lines or chars.

		CTextDisplay::_set( pBuffer );
		_caretToEnd();
	}

	void CTextEditor::_set( const String& str )
	{
		//TODO: Cut sequence if too many lines or chars.

		CTextDisplay::_set( str );
		_caretToEnd();
	}

	//____ _append() ________________________________________________________________

	int CTextEditor::_append( const CharSeq& seq )
	{
		//TODO: Cut sequence if too many lines or chars.

		// Appending text should move carret if carret is at end of text and nothing is selected,
		// otherwise not affect caret or selection.

		if( m_editState.caretOfs == m_editState.selectOfs && m_editState.caretOfs == m_charBuffer.length() )
		{
			int newOfs = m_editState.caretOfs + CTextDisplay::_append( seq );
			_textMapper()->caretMove( this, newOfs, m_editState.caretOfs );

			m_editState.caretOfs = newOfs;
			m_editState.selectOfs = newOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();
		}

		return CTextDisplay::_append( seq );
	}

	//____ _insert() ________________________________________________________________

	int CTextEditor::_insert( int ofs, const CharSeq& seq )
	{
		//TODO: Cut sequence if too many lines or chars.

		limit( ofs, 0, m_charBuffer.length() );

		int added = CTextDisplay::_insert(ofs,seq);

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
				_textMapper()->caretMove( this, caretOfs, m_editState.caretOfs );
			else
				_textMapper()->selectionChange( this, selectOfs, caretOfs, m_editState.caretOfs, m_editState.selectOfs );

			m_editState.caretOfs = caretOfs;
			m_editState.selectOfs = selectOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();
		}
		return added;
	}

	//____ _replace() _______________________________________________________________

	int CTextEditor::_replace( int ofs, int nDelete, const CharSeq& seq )
	{
		//TODO: Cut sequence if too many lines or chars.
		//TODO: Implement correctly!!!


		limit( ofs, 0, m_charBuffer.length() );
		limit( nDelete, 0, m_charBuffer.length() - ofs );

		int sizeModif = CTextDisplay::_replace(ofs,nDelete,seq);

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

	//____ _erase() ________________________________________________________________

	int CTextEditor::_erase( int ofs, int len )
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
				_textMapper()->caretMove( this, caretOfs, m_editState.caretOfs );
			else
				_textMapper()->selectionChange( this, selectOfs, caretOfs, m_editState.selectOfs, m_editState.caretOfs );

			m_editState.caretOfs = caretOfs;
			m_editState.selectOfs = selectOfs;
			m_editState.wantedOfs = -1;
			_updateDisplayArea();
		}

		int ret = CTextDisplay::_erase( ofs, len );
		_updateDisplayArea();
		return ret;
	}

	//____ _setState() ______________________________________________________________

	void CTextEditor::_setState( State state )
	{
		if( state.isFocused() != m_state.isFocused() && m_editMode == TextEditMode::Editable )
		{
			if( state.isFocused() )
			{
				// Enable and place caret

				m_editState.bCaret = true;
//				m_editState.caretOfs = m_charBuffer.length();		//TODO: Not always reset caretOfs.
//				m_editState.selectOfs = m_editState.caretOfs;
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

				_textMapper()->caretMove(this, m_editState.caretOfs, m_editState.caretOfs);
			}
			else
			{
				//TODO: Re-render portion that was covered by caret.

				m_editState.bCaret = false;
			}

		}


		// Set this last, so that bCaret is set when we call

		CTextDisplay::_setState(state);
	}


	//____ render() ______________________________________________________________

	void CTextEditor::_render( GfxDevice * pDevice, const RectI& _canvas )
	{
		_textMapper()->render(this, pDevice, qpixToPixels(_canvas) );
	}

	//____ setEditMode() _______________________________________________________

	void CTextEditor::setEditMode( TextEditMode mode )
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

	bool CTextEditor::select( int beg, int end )
	{
		if( m_editMode == TextEditMode::Static )
			return false;

		int max = m_charBuffer.length();

		limit( beg, 0, max );
		limit( end, 0, max );

		if( beg != m_editState.selectOfs || end != m_editState.caretOfs )
		{
			_textMapper()->selectionChange( this, beg, end, m_editState.selectOfs, m_editState.caretOfs );

			m_editState.selectOfs = beg;
			m_editState.caretOfs = end;
		}

		_updateDisplayArea();
		return true;
	}

	//____ selectAll() _________________________________________________________

	bool CTextEditor::selectAll()
	{
		return select( 0, m_charBuffer.length() );
	}

	//____ unselect() __________________________________________________________

	bool CTextEditor::unselect()
	{
		if( m_editMode == TextEditMode::Static )
			return false;

		if( m_editState.selectOfs != m_editState.caretOfs )
		{
			_textMapper()->selectionChange( this, m_editState.caretOfs, m_editState.caretOfs, m_editState.selectOfs, m_editState.caretOfs );

			m_editState.selectOfs = m_editState.caretOfs;
		}

		return true;
	}

	//____ selectionBegin() ____________________________________________________

	int CTextEditor::selectionBegin() const
	{
		return m_editState.selectOfs;
	}

	//____ selectionEnd() ______________________________________________________

	int CTextEditor::selectionEnd() const
	{
		return m_editState.caretOfs;
	}

	//____ selectionSize() ______________________________________________________

	int CTextEditor::selectionSize() const
	{
		return abs(m_editState.caretOfs-m_editState.selectOfs);
	}

	//____ eraseSelected() _____________________________________________________

	int CTextEditor::eraseSelected()
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

	bool CTextEditor::setCaretOfs( int ofs )
	{
		if( m_editMode != TextEditMode::Editable )
			return false;

		limit( ofs, 0, m_charBuffer.length() );

		return _moveCaret( ofs, MoveMethod::ApiCall );
	}

	//____ caretPos() ___________________________________________________________

	int CTextEditor::caretOfs() const
	{
		if( !m_editState.bCaret )
			return -1;

		return m_editState.caretOfs;
	}
	//____ caretPut() __________________________________________________________

	int CTextEditor::caretPut( const CharSeq& seq )
	{
		if( !m_editState.bCaret )
			return 0;

		// Copy sequence to a buffer for manipulation (this is a slow way of doing it)

		CharBuffer 	buffer;
		buffer.pushBack(seq);

		// Remove any newline characters if we only have one line (special case)

		if( m_maxLines == 1 )
		{
			const Char * p = buffer.chars();
			int len = buffer.length();

			for( int i = 0 ; i < len ; i++ )
			{
				if( p[i].isEndOfLine() )
				{
					buffer.remove(i);
					i--; len--;
				}
			}
		}
		else if( m_maxLines > 1 )	// Limit input to lines that will fit
		{
			//TODO: Implement
		}

		// Limit amount of characters

		if( m_maxChars > 0 )
		{
			int max = m_maxChars - m_charBuffer.length() + selectionSize();
			if( buffer.length() > max )
				buffer.remove(max, INT_MAX );
		}

		// Apply carets styling to all unstyled characters

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

	bool CTextEditor::caretPut( uint16_t c )
	{
		//TODO: Implement
		return false;
	}

	//____ caretUp() ___________________________________________________________

	bool CTextEditor::caretUp()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretUp(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretDown() _________________________________________________________

	bool CTextEditor::caretDown()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretDown(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretLeft() _________________________________________________________

	bool CTextEditor::caretLeft()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretLeft(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretRight() ________________________________________________________

	bool CTextEditor::caretRight()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretRight(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretNextWord() _____________________________________________________

	bool CTextEditor::caretNextWord()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretNextWord(this, m_editState.caretOfs);
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretPrevWord() _____________________________________________________

	bool CTextEditor::caretPrevWord()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretPrevWord(this, m_editState.caretOfs);
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretEraseNextChar() ________________________________________________

	bool CTextEditor::caretEraseNextChar()
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

	bool CTextEditor::caretErasePrevChar()
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
			_textMapper()->caretMove(this, 0, 0);		// Force restart of the caret animation cycle.
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

	bool CTextEditor::caretEraseNextWord()
	{
		return false; //TODO: Implement!
	}

	//____ caretErasePrevWord() ________________________________________________

	bool CTextEditor::caretErasePrevWord()
	{
		return false; //TODO: Implement!
	}




	//_____ caretLineBegin() ___________________________________________________

	bool CTextEditor::caretLineBegin()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretHome(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretLineEnd() ______________________________________________________

	bool CTextEditor::caretLineEnd()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = _textMapper()->caretEnd(this, m_editState.caretOfs, m_editState.wantedOfs );
		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretTextBegin() ____________________________________________________

	bool CTextEditor::caretTextBegin()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = 0;
		m_editState.wantedOfs = -1;

		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ caretTextEnd() ______________________________________________________

	bool CTextEditor::caretTextEnd()
	{
		if( !m_editState.bCaret )
			return false;

		int caretOfs = m_charBuffer.length();		// Caret placed on terminator char following the string.
		m_editState.wantedOfs = -1;

		return _moveCaret( caretOfs, MoveMethod::Keyboard );
	}

	//____ _caretToEnd() __________________________________________________________

	void CTextEditor::_caretToEnd()
	{
		m_editState.caretOfs = m_charBuffer.length();		// Caret placed on terminator char following the string.
		m_editState.selectOfs = m_editState.caretOfs;
		m_editState.wantedOfs = -1;
		_updateInsertStyle();
		_updateDisplayArea();
	}


	//____ _caretToPos() __________________________________________________________

	bool CTextEditor::_caretToPos( CoordI pos)
	{
		int ofs = _textMapper()->caretToPos(this, qpixToPixels(pos), m_editState.wantedOfs );
		return _moveCaret( ofs, MoveMethod::Mouse );
	}

	//____ _caretSelectWord() __________________________________________________

	bool CTextEditor::caretSelectWord()
	{
		//TODO: Implement!
		return false;
	}

	//____ _caretSelectLine() __________________________________________________

	bool CTextEditor::caretSelectLine()
	{
		//TODO: Implement!
		return false;
	}


	//____ _moveCaret() ______________________________________________

	bool CTextEditor::_moveCaret( int caretOfs, MoveMethod method )
	{
		int selectOfs = m_editState.selectOfs;

		if( method == MoveMethod::ApiCall || (method == MoveMethod::Keyboard && !m_editState.bShiftDown) ||
			(method == MoveMethod::Mouse && !(m_editState.bShiftDown || m_editState.bButtonDown)) )
		{
			selectOfs = caretOfs;
		}

		// Notify textmapper of caret and selection changes

		if( m_editState.selectOfs == selectOfs || m_editState.selectOfs != m_editState.caretOfs )
			_textMapper()->selectionChange( this, selectOfs, caretOfs, m_editState.selectOfs, m_editState.caretOfs );
		else
			_textMapper()->caretMove( this, caretOfs, m_editState.caretOfs );

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

	void CTextEditor::_updateDisplayArea()
	{
		// Make sure caret and surrounding characters are visible (and preferably whole selection, if any)
		//

		int beg = m_editState.caretOfs;
		int end = m_editState.selectOfs;

		if (end < beg)
			std::swap(beg, end);

		RectI niceToHave = pixelsToQpix(_textMapper()->rectForRange(this, beg, end - beg));
		RectI mustHave = pixelsToQpix(_textMapper()->rectForCaret(this));

		_requestVisibility(mustHave, niceToHave);
	}

	//____ _updateInsertStyle() ________________________________________________

	void CTextEditor::_updateInsertStyle()
	{

	}



} // namespace wg
