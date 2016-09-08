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


#include <wg_valueeditor.h>

#include <math.h>
#include <stdlib.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_caretinstance.h>
#include <wg_key.h>
#include <wg_msg.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char ValueEditor::CLASSNAME[] = {"ValueEditor"};
	
	
	//____ ValueEditor() _________________________________________________________________
	
	ValueEditor::ValueEditor() : m_text(this), text(&m_text)
	{
		_regenText();
		m_buttonDownOfs = 0;
		m_bSelectAllOnRelease = false;
		m_maxInputChars = 256;
		m_viewOfs = Coord(0,0);
	
		m_pointerStyle	= PointerStyle::Ibeam;
	
		m_text.setWrap(false);
		m_text.setAutoEllipsis(false);
		m_text.setEditMode( TextEditMode::Editable );
	
		m_pFormat = ValueFormat::create();
		m_pUseFormat = ValueFormat::create();
	
		m_tickRouteId = 0;
	}
	
	//____ ~ValueEditor() ___________________________________________________________
	
	ValueEditor::~ValueEditor()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ValueEditor::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ValueEditor::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ValueEditor_p ValueEditor::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ValueEditor_p( static_cast<ValueEditor*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	
	//____ setFormat() ____________________________________________________________
	
	void ValueEditor::setFormat( const ValueFormat_p& pFormat )
	{
		m_pFormat		= pFormat;
		m_pUseFormat	= pFormat;
	
		_regenText();
		_requestRender();
	}
	
	
	
	//____ clear() ________________________________________________________________
	
	void ValueEditor::clear()
	{
		// Force value to zero, emit signals and request render
	
		m_value = 0;
		_valueModified();
	
		// Make the inputitem empty
	
		m_pUseFormat->setIntegers(0);
		m_pUseFormat->setForcePeriod(false);
		m_pUseFormat->setDecimals(0);
	}
	
	//____ setMaxInputChars() _____________________________________________________
	
	bool ValueEditor::setMaxInputChars( int max )
	{
		if( max <= 0 )
			return false;
		m_maxInputChars = max;
		return true;
	}
	
	
	//____ preferredSize() __________________________________________________________
	
	Size ValueEditor::preferredSize() const
	{
		Size	sz;
	
		sz.h = m_text.height();
		sz.w = 0;
	
		TextAttr	attr;
		m_text.getBaseAttr( attr );
		if( attr.pFont )
			sz.w = attr.pFont->getFont(attr.style,attr.size)->getMaxGlyphAdvance(attr.size)*5;	// By default fit at least 5 characters
	
		if( sz.h == 0 )
			return Widget::preferredSize();
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(sz);
		else
			return sz;
	}
	
	
	//____ _valueModified() ________________________________________________________
	
	void ValueEditor::_valueModified()
	{
		Base::msgRouter()->post( new ValueUpdateMsg(this,m_value,fractionalValue(),false) );
	
		m_pUseFormat->setFormat( m_pFormat );
	
		if( 0 != m_pFormat->getNoDecimalThreshold() && (int)m_value >= m_pFormat->getNoDecimalThreshold() )
		{
			// value is >= noDecimalThreshold so prevent period and decimals
			m_pUseFormat->setForceDecimals(false);
			m_pUseFormat->setDecimals(0);
		}
		_regenText();
		_requestRender();
	}
	
	//____ _rangeModified() ________________________________________________________
	
	void ValueEditor::_rangeModified()
	{
			Base::msgRouter()->post( new ValueUpdateMsg(this,m_value,fractionalValue(), false) );
	}
	
	//____ _refresh() ____________________________________________________________
	
	void ValueEditor::_refresh( void )
	{
		if( m_text.properties() && m_text.properties()->font() )
			_regenText();
	
		Widget::_refresh();
	}
	
	
	//____ _render() _____________________________________________________________
	
	void ValueEditor::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_render(pDevice,_canvas,_window,_clip);
	
		// Adjust text canvas
	
		Rect	canvas = _canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
	
		Rect textClip( canvas, _clip );
	
		if( m_text.isCaretShowing() )
		{
			int textW = m_text.width();
			if( textW > canvas.w )
				canvas.w = textW;
	
			CaretInstance * pCaret = m_text.getCaret();
			m_viewOfs = m_text.focusWindowOnRange( canvas.size(), Rect(m_viewOfs,_canvas.size()), Range( pCaret->column(),0 ) );
		}
		else
			m_viewOfs = Coord(0,0);
	
		// Print the text
	
		pDevice->printText( textClip, &m_text, canvas - m_viewOfs );
	}
	
	//____ _regenText() ____________________________________________________________
	
	void ValueEditor::_regenText()
	{
		m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat.rawPtr() );
		m_text.goEol();
	}
	
	//____ _parseValueFromInput() __________________________________________________
	
	bool ValueEditor::_parseValueFromInput( int64_t * wpResult )
	{
		// Parse text as value as a positive or negative integer within boundaries
		// return false if value had to be modified in any way.
	
		const Char * p = m_text.getBuffer()->chars();
		int		nbChars = m_text.getBuffer()->nbChars();
	
		int64_t	value = 0;
		bool	bModified = false;			// Set when value displayed isn't fully acceptable (outside boundaries or has too many decimals)
		int		nDecimals = 0;
	
		if( nbChars > 0 )
		{
			for( int i = 0 ; i < nbChars ; i++ )
			{
				uint16_t glyph = p[i].getGlyph();
	
				if( glyph == '-' )
					continue;
	
				if( glyph == m_pFormat->getPeriod() )
				{
					nDecimals = nbChars-i-1;
					if( nDecimals > m_pFormat->getDecimals() )
					{
						// Only parse as many decimals as we need, avoid unnecessary overflows.
						nbChars -= nDecimals - m_pFormat->getDecimals();
						nDecimals = m_pFormat->getDecimals();
						bModified = true;
					}
	
				}
				else if( glyph >= '0' && glyph <= '9' )
				{
					if( value <= LLONG_MAX/10 )
						value *= 10;
					else
					{
						value = LLONG_MAX;
						bModified = true;
						break;
					}
	
					int toAdd = glyph - '0';
	
					if( value <= LLONG_MAX-toAdd )
						value += toAdd;
					else
					{
						value = LLONG_MAX;
						bModified = true;
						break;
					}
				}
	
			}
	
			if( nDecimals < m_pFormat->getDecimals() )
			{
				int64_t multiplier = (int64_t) pow(10.0,m_pFormat->getDecimals()-nDecimals);
	
				if( value <= LLONG_MAX/multiplier )
					value *= multiplier;
				else
				{
					value = LLONG_MAX;
					bModified = true;
				}
			}
	
			if( p[0].getGlyph() == '-' )
			{
				if( value == LLONG_MAX )
					value = m_rangeMin;
				else
				{
					value = -value;
					if( value < m_rangeMin )
						value = m_rangeMin;
				}
			}
			else
			{
				if( value > m_rangeMax )
					value = m_rangeMax;
			}
	
		}
	
		* wpResult = value;
		return bModified;
	}
	
	//____ _receive() ______________________________________________________________
	
	void ValueEditor::_receive( const Msg_p& pMsg )
	{
		Widget::_receive(pMsg);
		MsgRouter_p	pHandler = Base::msgRouter();
		MsgType type = pMsg->type();
	
		if( type == MsgType::Tick )
		{
			if( m_text.getCaret() )
			{
				m_text.getCaret()->incTime( TickMsg::cast(pMsg)->timediff() );
				_requestRender();					//TODO: Should only render the cursor (and only when updated)!
			}
			return;
		}
	
	
		bool	bTextChanged = false;
		MouseButton	mousebutton = MouseButton::None;
		ModifierKeys	modKeys = MODKEY_NONE;
		Coord	pointerPos;
		
		if( pMsg->isInstanceOf( InputMsg::CLASSNAME ))
		{
			InputMsg_p p = InputMsg::cast(pMsg);
			modKeys = p->modKeys();
			pointerPos = p->pointerPos();

			if( pMsg->isMouseButtreceive() )
				mousebutton = MouseButtonMsg::cast(pMsg)->button();
		}
	
	
		Coord ofs = pointerPos - globalPos();
	
		if( type == MsgType::MousePress && mousebutton == MouseButton::Left )
		{
			if( !m_state.isFocused() )
			{
				grabFocus();
				m_bSelectAllOnRelease = true;
			}
			else
				m_bSelectAllOnRelease = false;
	
			if( modKeys & MODKEY_SHIFT )
			{
				m_text.setSelectionMode(true);
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCaret();
			}
			else
			{
				m_text.setSelectionMode(false);
				m_text.clearSelection();
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCaret();
				m_text.setSelectionMode(true);
			}
	
			m_buttonDownOfs = ofs.x;
		}
	
		if( type == MsgType::MouseDrag && mousebutton == MouseButton::Left )
		{
			if( m_state.isFocused() && ofs.x != m_buttonDownOfs )
			{
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCaret();
				m_buttonDownOfs = ofs.x;
				m_bSelectAllOnRelease = false;
			}
		}
	
		if( type == MsgType::MouseRelease )
		{
			if( m_state.isFocused() && mousebutton == MouseButton::Left )
			{
				m_text.setSelectionMode(false);
	
				if( m_bSelectAllOnRelease )
					_selectAll();
			}
		}
	
	
		if( type == MsgType::MouseDoubleClick && mousebutton == MouseButton::Left )
		{
			_selectAll();
			m_text.setSelectionMode(true);
		}
	
	
		if( type == MsgType::KeyPress || type == MsgType::KeyRepeat )
		{
			Key key = KeyMsg::cast(pMsg)->translatedKeyCode();
			switch( key )
			{
				case Key::Return:
				{
					// We already have correct value in m_value, but we want
					// to set the bModified flag if value is entered in a weird way.
	
					int64_t		value;
					bool bModified = _parseValueFromInput( &value );
	
					if( m_value < m_rangeMin )
					{
						m_value = m_rangeMin;
						bModified = true;
					}
	
					if( m_value > m_rangeMax )
					{
						m_value = m_rangeMax;
						bModified = true;
					}
	
					if( bModified )
					{
						_updateScrollbar( fractionalValue(), 0.f );
						Base::msgRouter()->post( new ValueUpdateMsg(this, m_value, fractionalValue(), false) );
	
						m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat.rawPtr() );
						m_text.goEol();
						_limitCaret();
					}
					else
					{
						releaseFocus();
					}
	
				}
				break;
	
				case Key::Left:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
					else
						m_text.setSelectionMode(false);
	
					if( modKeys & MODKEY_CTRL )
						m_text.gotoPrevWord();
					else
						m_text.goLeft();
					_limitCaret();
					break;
				case Key::Right:
					if( modKeys & MODKEY_SHIFT )
						m_text.setSelectionMode(true);
					else
						m_text.setSelectionMode(false);
	
					if( modKeys & MODKEY_CTRL )
						m_text.gotoNextWord();
					else
						m_text.goRight();
					_limitCaret();
					break;
	
				case Key::Backspace:
					if(m_text.hasSelection())
					{
						m_text.delSelection();
						bTextChanged = true;
					}
					else if( modKeys & MODKEY_CTRL )
					{
						int ofs1 = m_text.column();
						m_text.gotoPrevWord();
						_limitCaret();
						int ofs2 = m_text.column();
	
						if( ofs2 < ofs1 )
						{
							m_text.remove( ofs2, ofs1 - ofs2 );
							bTextChanged = true;
						}
	//					m_text.delPrevWord();
					}
					else
					{
						if( m_text.column() > m_pFormat->getPrefix().length() )
						{
							m_text.delPrevChar();
							bTextChanged = true;
						}
					}
	
					break;
	
				case Key::Delete:
					if(m_text.hasSelection())
					{
						m_text.delSelection();
						bTextChanged = true;
					}
					else if( modKeys & MODKEY_CTRL )
					{
						int ofs1 = m_text.column();
						m_text.gotoNextWord();
						_limitCaret();
						int ofs2 = m_text.column();
	
						if( ofs2 > ofs1 )
						{
							m_text.remove( ofs1, ofs2 - ofs1 );
							bTextChanged = true;
						}
	//					m_text.delNextWord();
					}
					else
					{
						if( m_text.column() < m_text.length() - m_pFormat->getSuffix().length() )
						{
							m_text.delNextChar();
							bTextChanged = true;
						}
	//					m_text.delNextChar();
					}
	
					break;
	
				case Key::Home:
	
					/*
					 *	I am not sure if this is the proper way to this, but in my opinion, the default
					 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( modKeys )
					{
	
					case MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if( modKeys & MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goBol();
						_limitCaret();
						break;
					}
	
					break;
	
				case Key::End:
	
					/*
				 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
			 		 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( modKeys )
					{
	
					case MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if( modKeys & MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goEol();
						_limitCaret();
						break;
					}
	
					break;
	
	
	
				default:
					break;
			}
		}
	
		if( type == MsgType::TextInput )
		{
			String str = TextInputMsg::cast(pMsg)->text();
	
			// Period is only allowed if it isn't displayed yet and value has decimals.
			// It's not allowed before a minus sign.
			// A zero is inserted (always INSERTED even if mode is replace) if there is no number before it.
	
			CaretInstance * pCaret = m_text.getCaret();
			if( pCaret )
			{
				for( int i = 0 ; i < str.length() ; i++ )
				{
					uint16_t character = str.chars()[i].getGlyph();
					if( character == m_pFormat->getPeriod() )
					{
						if( m_pFormat->getDecimals() > 0 && m_text.getBuffer()->findFirst( m_pFormat->getPeriod() ) == -1 &&
							(pCaret->column() != 0 || (*m_text.getBuffer())[0].getGlyph() != '-' ) )
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
							{
								if( pCaret->column() == 0 || (pCaret->column() == 1 && (*m_text.getBuffer())[0].getGlyph() == '-' ) )
								{
									m_text.insertChar( 0, Char('0') );
									pCaret->goRight();
								}
		
								pCaret->putChar( m_pFormat->getPeriod() );
							}
							bTextChanged = true;
						}
					}
		
					// Handle minus
					// Only allow minus at start of text and only if range allows negative values.
		
					if( character == '-' )
					{
						if( pCaret->column() == m_pFormat->getPrefix().length() && m_text.getBuffer()->findFirst( m_pFormat->getPeriod() ) == -1 &&
							m_rangeMin < 0 )
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
								pCaret->putChar( '-' );
							bTextChanged = true;
						}
					}
		
					// Take care of typed numbers 0 through 9.
		
					if( character >= '0' && character <= '9' )
					{
						if( pCaret->column() == 0 && (*m_text.getBuffer())[0].getGlyph() == '-' )
						{
						}
						else
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
								pCaret->putChar( character );
							bTextChanged = true;
						}
					}
				}
			}
		}
	
		if( bTextChanged )
		{
			int64_t		value;
			bool bModified = _parseValueFromInput( &value );
	
			if( value != m_value )
			{
				m_value = value;
	
				if( pHandler )
					pHandler->post( new ValueUpdateMsg(this,value,fractionalValue(),false) );
	
				_updateScrollbar( fractionalValue(), 0.f );
			}
		}
		
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtreceive() )
		{
			if( MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left )
				pMsg->swallow();
		}
		else if( pMsg->isKeyMsg() )
		{
			Key key = KeyMsg::cast(pMsg)->translatedKeyCode();
			if( KeyMsg::cast(pMsg)->isMovementKey() == true ||
				key == Key::Delete || key == Key::Backspace )
				pMsg->swallow();
			
			//TODO: Would be good if we didn't forward any character-creating keys either...
		}
	}
	
	//____ _selectAll() ___________________________________________________________
	
	void ValueEditor::_selectAll()
	{
		int min = m_pFormat->getPrefix().length();
		int max = m_text.length() - m_pFormat->getSuffix().length();
	
		m_text.select( min, max-min );
	}
	
	//____ _limitCaret() _________________________________________________________
	
	void ValueEditor::_limitCaret()
	{
		int min = m_pFormat->getPrefix().length();
		int max = m_text.length() - m_pFormat->getSuffix().length();
	
		CaretInstance * pCaret = m_text.getCaret();
	
		// Save selection (might get destroyed by moving cursor)
	
	//	bool bSelectionMode = pCaret->getSelectionMode();
	//	Range sel = pCaret->getSelection();
	
		// Move cursor to within prefix/suffix
	
		if( pCaret->column() < min )
			pCaret->gotoColumn( min );
		if( pCaret->column() > max )
			pCaret->gotoColumn( max );
	
		// Tweak selection if cursor was not in selection mode
	/*
		if( !bSelectionMode )
		{
			if( sel.ofs < min )
			{
				sel.len -= min - sel.ofs;
				sel.ofs = min;
			}
	
			if( sel.len < 0 )
				sel.len = 0;
	
			if( sel.ofs + sel.len > max )
				sel.len = max - sel.ofs;
	
			m_text.selectText( 0, sel.ofs, 0, sel.ofs + sel.len );
		}
	*/
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void ValueEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		ValueEditor * pOrg = (ValueEditor *) _pOrg;
	
		m_maxInputChars = pOrg->m_maxInputChars;
		m_pFormat		= pOrg->m_pFormat;
		m_pUseFormat->setFormat(pOrg->m_pFormat);
		m_text.clone(&pOrg->m_text);
	}
	
	//____ setState() _______________________________________________________
	
	void ValueEditor::_setState( State state )
	{
		State oldState = m_state;
		Widget::_setState(state);
	
		// Update text
	
		m_text.setState(state);
		_requestRender();				//TODO: Only render if text has been affected
	
		// Check if we got input focus
	
		if( state.isFocused() && !oldState.isFocused() )
		{
			m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			m_text.showCaret();
			m_text.goEol();
			m_pUseFormat = m_pFormat;
	
			if( m_pFormat->getDecimals() != 0 )
				m_pUseFormat->setForcePeriod(true);	// Force period if decimals are involved.
	
			if( m_value < 0.f )
				m_pUseFormat->setZeroIsNegative(true);	// Force minus sign if value is negative.
	
			m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat );
	
			_requestRender();
		}
	
		// Check if we lost input focus
	
		if( !state.isFocused() && oldState.isFocused() )
		{
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
	
			Base::msgRouter()->post( new ValueUpdateMsg(this,m_value,fractionalValue(), true) );
	
			m_text.hideCaret();
			m_pUseFormat->setFormat(m_pFormat);
			_regenText();
	
			_requestRender();
		}
	}
	
	
	//____ _setSkin() _______________________________________________________
	
	void ValueEditor::_setSkin( const Skin_p& pSkin )
	{
		Widget::_setSkin(pSkin);
	}
	
	//____ _requestRender() _________________________________________________________
	
	void ValueEditor::_requestRender( Item * pItem )
	{
		_requestRender();
	}
	
	void ValueEditor::_requestRender( Item * pItem, const Rect& rect )
	{
		_requestRender();
	}

	//____ _requestResize() ________________________________________________________
	
	void ValueEditor::_requestResize( Item * pItem )
	{
		_requestResize();
	}

} // namespace wg
