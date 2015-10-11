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
	
		m_pointerStyle	= WG_POINTER_IBEAM;
	
		m_text.setWrap(false);
		m_text.setAutoEllipsis(false);
		m_text.setEditMode( WG_TEXT_EDITABLE );
	
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
	
		// Make the inputfield empty
	
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
			sz.w = attr.pFont->getGlyphset(attr.style,attr.size)->getMaxGlyphAdvance(attr.size)*5;	// By default fit at least 5 characters
	
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
	
	//____ _onRefresh() ____________________________________________________________
	
	void ValueEditor::_onRefresh( void )
	{
		if( m_text.properties() && m_text.properties()->font() )
			_regenText();
	
		Widget::_onRefresh();
	}
	
	
	//____ _onRender() _____________________________________________________________
	
	void ValueEditor::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		// Adjust text canvas
	
		Rect	canvas = _canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
	
		Rect textClip( canvas, _clip );
	
		if( m_text.isCursorShowing() )
		{
			int textW = m_text.width();
			if( textW > canvas.w )
				canvas.w = textW;
	
			CaretInstance * pCursor = m_text.getCursor();
			m_viewOfs = m_text.focusWindowOnRange( canvas.size(), Rect(m_viewOfs,_canvas.size()), Range( pCursor->column(),0 ) );
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
	
	//____ _onMsg() ______________________________________________________________
	
	void ValueEditor::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
		MsgRouter_p	pHandler = Base::msgRouter();
		MsgType type = pMsg->type();
	
		if( type == WG_MSG_TICK )
		{
			if( m_text.getCursor() )
			{
				m_text.getCursor()->incTime( TickMsg::cast(pMsg)->timediff() );
				_requestRender();					//TODO: Should only render the cursor (and only when updated)!
			}
			return;
		}
	
	
		bool	bTextChanged = false;
		int		mousebutton = 0;
		WgModifierKeys	modKeys = WG_MODKEY_NONE;
		Coord	pointerPos;
		
		if( pMsg->isInstanceOf( InputMsg::CLASSNAME ))
		{
			InputMsg_p p = InputMsg::cast(pMsg);
			modKeys = p->modKeys();
			pointerPos = p->pointerPos();

			if( pMsg->isMouseButtonMsg() )
				mousebutton = MouseButtonMsg::cast(pMsg)->button();
		}
	
	
		Coord ofs = pointerPos - globalPos();
	
		if( type == WG_MSG_MOUSE_PRESS && mousebutton == 1 )
		{
			if( !m_state.isFocused() )
			{
				grabFocus();
				m_bSelectAllOnRelease = true;
			}
			else
				m_bSelectAllOnRelease = false;
	
			if( modKeys & WG_MODKEY_SHIFT )
			{
				m_text.setSelectionMode(true);
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCursor();
			}
			else
			{
				m_text.setSelectionMode(false);
				m_text.clearSelection();
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCursor();
				m_text.setSelectionMode(true);
			}
	
			m_buttonDownOfs = ofs.x;
		}
	
		if( type == WG_MSG_MOUSE_DRAG && mousebutton == 1 )
		{
			if( m_state.isFocused() && ofs.x != m_buttonDownOfs )
			{
				m_text.cursorGotoCoord( ofs, Rect(0,0,size()) );
				_limitCursor();
				m_buttonDownOfs = ofs.x;
				m_bSelectAllOnRelease = false;
			}
		}
	
		if( type == WG_MSG_MOUSE_RELEASE )
		{
			if( m_state.isFocused() && mousebutton == 1 )
			{
				m_text.setSelectionMode(false);
	
				if( m_bSelectAllOnRelease )
					_selectAll();
			}
		}
	
	
		if( type == WG_MSG_MOUSE_DOUBLE_CLICK && mousebutton == 1 )
		{
			_selectAll();
			m_text.setSelectionMode(true);
		}
	
	
		if( type == WG_MSG_KEY_PRESS || type == WG_MSG_KEY_REPEAT )
		{
			int key = KeyMsg::cast(pMsg)->translatedKeyCode();
			switch( key )
			{
				case WG_KEY_RETURN:
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
						_limitCursor();
					}
					else
					{
						releaseFocus();
					}
	
				}
				break;
	
				case WG_KEY_LEFT:
					if( modKeys & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);
					else
						m_text.setSelectionMode(false);
	
					if( modKeys & WG_MODKEY_CTRL )
						m_text.gotoPrevWord();
					else
						m_text.goLeft();
					_limitCursor();
					break;
				case WG_KEY_RIGHT:
					if( modKeys & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);
					else
						m_text.setSelectionMode(false);
	
					if( modKeys & WG_MODKEY_CTRL )
						m_text.gotoNextWord();
					else
						m_text.goRight();
					_limitCursor();
					break;
	
				case WG_KEY_BACKSPACE:
					if(m_text.hasSelection())
					{
						m_text.delSelection();
						bTextChanged = true;
					}
					else if( modKeys & WG_MODKEY_CTRL )
					{
						int ofs1 = m_text.column();
						m_text.gotoPrevWord();
						_limitCursor();
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
	
				case WG_KEY_DELETE:
					if(m_text.hasSelection())
					{
						m_text.delSelection();
						bTextChanged = true;
					}
					else if( modKeys & WG_MODKEY_CTRL )
					{
						int ofs1 = m_text.column();
						m_text.gotoNextWord();
						_limitCursor();
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
	
				case WG_KEY_HOME:
	
					/*
					 *	I am not sure if this is the proper way to this, but in my opinion, the default
					 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( modKeys )
					{
	
					case WG_MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if( modKeys & WG_MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goBol();
						_limitCursor();
						break;
					}
	
					break;
	
				case WG_KEY_END:
	
					/*
				 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
			 		 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( modKeys )
					{
	
					case WG_MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if( modKeys & WG_MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goEol();
						_limitCursor();
						break;
					}
	
					break;
	
	
	
				default:
					break;
			}
		}
	
		if( type == WG_MSG_TEXT_INPUT )
		{
			String str = TextInputMsg::cast(pMsg)->text();
	
			// Period is only allowed if it isn't displayed yet and value has decimals.
			// It's not allowed before a minus sign.
			// A zero is inserted (always INSERTED even if mode is replace) if there is no number before it.
	
			CaretInstance * pCursor = m_text.getCursor();
			if( pCursor )
			{
				for( int i = 0 ; i < str.length() ; i++ )
				{
					uint16_t character = str.chars()[i].getGlyph();
					if( character == m_pFormat->getPeriod() )
					{
						if( m_pFormat->getDecimals() > 0 && m_text.getBuffer()->findFirst( m_pFormat->getPeriod() ) == -1 &&
							(pCursor->column() != 0 || (*m_text.getBuffer())[0].getGlyph() != '-' ) )
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
							{
								if( pCursor->column() == 0 || (pCursor->column() == 1 && (*m_text.getBuffer())[0].getGlyph() == '-' ) )
								{
									m_text.insertChar( 0, Char('0') );
									pCursor->goRight();
								}
		
								pCursor->putChar( m_pFormat->getPeriod() );
							}
							bTextChanged = true;
						}
					}
		
					// Handle minus
					// Only allow minus at start of text and only if range allows negative values.
		
					if( character == '-' )
					{
						if( pCursor->column() == m_pFormat->getPrefix().length() && m_text.getBuffer()->findFirst( m_pFormat->getPeriod() ) == -1 &&
							m_rangeMin < 0 )
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
								pCursor->putChar( '-' );
							bTextChanged = true;
						}
					}
		
					// Take care of typed numbers 0 through 9.
		
					if( character >= '0' && character <= '9' )
					{
						if( pCursor->column() == 0 && (*m_text.getBuffer())[0].getGlyph() == '-' )
						{
						}
						else
						{
							if(m_text.hasSelection())
								m_text.delSelection();
							m_text.setSelectionMode(false);
		
							if( m_text.length() < m_maxInputChars )
								pCursor->putChar( character );
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
	
		if( pMsg->isMouseButtonMsg() )
		{
			if( MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
				pMsg->swallow();
		}
		else if( pMsg->isKeyMsg() )
		{
			int key = KeyMsg::cast(pMsg)->translatedKeyCode();
			if( KeyMsg::cast(pMsg)->isMovementKey() == true ||
				key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE )
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
	
	//____ _limitCursor() _________________________________________________________
	
	void ValueEditor::_limitCursor()
	{
		int min = m_pFormat->getPrefix().length();
		int max = m_text.length() - m_pFormat->getSuffix().length();
	
		CaretInstance * pCursor = m_text.getCursor();
	
		// Save selection (might get destroyed by moving cursor)
	
	//	bool bSelectionMode = pCursor->getSelectionMode();
	//	Range sel = pCursor->getSelection();
	
		// Move cursor to within prefix/suffix
	
		if( pCursor->column() < min )
			pCursor->gotoColumn( min );
		if( pCursor->column() > max )
			pCursor->gotoColumn( max );
	
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
	
	//____ _onCloneContent() _______________________________________________________
	
	void ValueEditor::_onCloneContent( const Widget * _pOrg )
	{
		ValueEditor * pOrg = (ValueEditor *) _pOrg;
	
		_Interface_ValueHolder::_onCloneContent( pOrg );
	
		m_maxInputChars = pOrg->m_maxInputChars;
		m_pFormat		= pOrg->m_pFormat;
		m_pUseFormat->setFormat(pOrg->m_pFormat);
		m_text.clone(&pOrg->m_text);
	}
	
	//____ onStateChanged() _______________________________________________________
	
	void ValueEditor::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		// Update text
	
		m_text.setState(m_state);
		_requestRender();				//TODO: Only render if text has been affected
	
		// Check if we got input focus
	
		if( m_state.isFocused() && !oldState.isFocused() )
		{
			m_tickRouteId = Base::msgRouter()->addRoute( WG_MSG_TICK, this );
			m_text.showCursor();
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
	
		if( !m_state.isFocused() && oldState.isFocused() )
		{
			Base::msgRouter()->deleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
	
			Base::msgRouter()->post( new ValueUpdateMsg(this,m_value,fractionalValue(), true) );
	
			m_text.hideCursor();
			m_pUseFormat->setFormat(m_pFormat);
			_regenText();
	
			_requestRender();
		}
	}
	
	
	//____ _onSkinChanged() _______________________________________________________
	
	void ValueEditor::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Widget::_onSkinChanged(pOldSkin,pNewSkin);
		m_text.setColorSkin(pNewSkin);
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void ValueEditor::_onFieldDirty( Field * pField )
	{
		_requestRender();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void ValueEditor::_onFieldResize( Field * pField )
	{
		_requestResize();
	}

} // namespace wg
