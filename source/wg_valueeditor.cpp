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
#include <wg_cursorinstance.h>
#include <wg_key.h>
#include <wg_textmanager.h>
#include <wg_event.h>
#include <wg_eventhandler.h>

const char WgValueEditor::CLASSNAME[] = {"ValueEditor"};


//____ WgValueEditor() _________________________________________________________________

WgValueEditor::WgValueEditor()
{
	m_text.setHolder(this);
	_regenText();
	m_buttonDownOfs = 0;
	m_bSelectAllOnRelease = false;
	m_maxInputChars = 256;
	m_viewOfs = WgCoord(0,0);

	m_pointerStyle	= WG_POINTER_IBEAM;

	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(false);
	m_text.SetEditMode( WG_TEXT_EDITABLE );

	m_pFormat = WgValueFormat::Create();
	m_pUseFormat = WgValueFormat::Create();
}

//____ ~WgValueEditor() ___________________________________________________________

WgValueEditor::~WgValueEditor()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgValueEditor::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgValueEditor::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgValueEditorPtr WgValueEditor::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgValueEditorPtr( static_cast<WgValueEditor*>(pObject.GetRealPtr()) );

	return 0;
}



//____ SetFormat() ____________________________________________________________

void WgValueEditor::SetFormat( const WgValueFormatPtr& pFormat )
{
	m_pFormat		= pFormat;
	m_pUseFormat	= pFormat;

	_regenText();
	_requestRender();
}



//____ Clear() ________________________________________________________________

void WgValueEditor::Clear()
{
	// Force value to zero, emit signals and request render

	m_value = 0;
	_valueModified();

	// Make the inputfield empty

	m_pUseFormat->setIntegers(0);
	m_pUseFormat->setForcePeriod(false);
	m_pUseFormat->setDecimals(0);
}

//____ SetMaxInputChars() _____________________________________________________

bool WgValueEditor::SetMaxInputChars( int max )
{
	if( max <= 0 )
		return false;
	m_maxInputChars = max;
	return true;
}


//____ PreferredSize() __________________________________________________________

WgSize WgValueEditor::PreferredSize() const
{
	WgSize	sz;

	sz.h = m_text.Height();
	sz.w = 0;

	WgTextAttr	attr;
	m_text.GetBaseAttr( attr );
	if( attr.pFont )
		sz.w = attr.pFont->GetGlyphset(attr.style,attr.size)->GetMaxGlyphAdvance(attr.size)*5;	// By default fit at least 5 characters

	if( sz.h == 0 )
		return WgWidget::PreferredSize();

	if( m_pSkin )
		return m_pSkin->SizeForContent(sz);
	else
		return sz;
}


//____ _valueModified() ________________________________________________________

void WgValueEditor::_valueModified()
{
	_queueEvent( new WgValueUpdateEvent(this,m_value,FractionalValue(),false) );

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

void WgValueEditor::_rangeModified()
{
		_queueEvent( new WgValueUpdateEvent(this,m_value,FractionalValue(), false) );
}

//____ _fieldModified() ________________________________________________________

void WgValueEditor::_fieldModified( WgTextField * pField )
{
	_requestRender();
}


//____ _onRefresh() ____________________________________________________________

void WgValueEditor::_onRefresh( void )
{
	if( m_text.Font() )
	{
		_regenText();
		_requestRender();
	}
}


//____ _onRender() _____________________________________________________________

void WgValueEditor::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	// Adjust text canvas

	WgRect	canvas = _canvas;
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(_canvas,m_state);

	WgRect textClip( canvas, _clip );

	if( m_text.isCursorShowing() )
	{
		int textW = m_text.Width();
		if( textW > canvas.w )
			canvas.w = textW;

		WgCursorInstance * pCursor = m_text.GetCursor();
		m_viewOfs = m_text.FocusWindowOnRange( canvas.Size(), WgRect(m_viewOfs,_canvas.Size()), WgRange( pCursor->column(),0 ) );
	}
	else
		m_viewOfs = WgCoord(0,0);

	// Print the text

	pDevice->PrintText( textClip, &m_text, canvas - m_viewOfs );
}

//____ _regenText() ____________________________________________________________

void WgValueEditor::_regenText()
{
	m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat.GetRealPtr() );
	m_text.GoEOL();
}

//____ _parseValueFromInput() __________________________________________________

bool WgValueEditor::_parseValueFromInput( int64_t * wpResult )
{
	// Parse text as value as a positive or negative integer within boundaries
	// return false if value had to be modified in any way.

	const WgChar * p = m_text.getBuffer()->Chars();
	int		nbChars = m_text.getBuffer()->NbChars();

	int64_t	value = 0;
	bool	bModified = false;			// Set when value displayed isn't fully acceptable (outside boundaries or has too many decimals)
	int		nDecimals = 0;

	if( nbChars > 0 )
	{
		for( int i = 0 ; i < nbChars ; i++ )
		{
			Uint16 glyph = p[i].Glyph();

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

		if( p[0].Glyph() == '-' )
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

//____ _onEvent() ______________________________________________________________

void WgValueEditor::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	WgEventType event = pEvent->Type();

	if( event == WG_EVENT_TICK )
	{
		if( m_text.GetCursor() )
		{
			m_text.GetCursor()->incTime( WgTickEvent::Cast(pEvent)->Millisec() );
			_requestRender();					//TODO: Should only render the cursor (and only when updated)!
		}
		return;
	}


	bool	bTextChanged = false;
	int		mousebutton = 0;

	if( pEvent->IsMouseButtonEvent() )
		mousebutton = WgMouseButtonEvent::Cast(pEvent)->Button();

	WgCoord ofs = pEvent->PointerPos();

	if( event == WG_EVENT_MOUSE_PRESS && mousebutton == 1 )
	{
		if( !m_state.IsFocused() )
		{
			GrabFocus();
			m_bSelectAllOnRelease = true;
		}
		else
			m_bSelectAllOnRelease = false;

		if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
		{
			m_text.setSelectionMode(true);
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
			_limitCursor();
		}
		else
		{
			m_text.setSelectionMode(false);
			m_text.ClearSelection();
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
			_limitCursor();
			m_text.setSelectionMode(true);
		}

		m_buttonDownOfs = ofs.x;
	}

	if( event == WG_EVENT_MOUSE_DRAG && mousebutton == 1 )
	{
		if( m_state.IsFocused() && ofs.x != m_buttonDownOfs )
		{
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
			_limitCursor();
			m_buttonDownOfs = ofs.x;
			m_bSelectAllOnRelease = false;
		}
	}

	if( event == WG_EVENT_MOUSE_RELEASE )
	{
		if( m_state.IsFocused() && mousebutton == 1 )
		{
			m_text.setSelectionMode(false);

			if( m_bSelectAllOnRelease )
				_selectAll();
		}
	}


	if( event == WG_EVENT_MOUSE_DOUBLE_CLICK && mousebutton == 1 )
	{
		_selectAll();
		m_text.setSelectionMode(true);
	}


	if( event == WG_EVENT_KEY_PRESS || event == WG_EVENT_KEY_REPEAT )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
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
					_updateScrollbar( FractionalValue(), 0.f );
					_queueEvent( new WgValueUpdateEvent(this, m_value, FractionalValue(), false) );

					m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat.GetRealPtr() );
					m_text.GoEOL();
					_limitCursor();
				}
				else
				{
					ReleaseFocus();
				}

			}
			break;

			case WG_KEY_LEFT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);
				else
					m_text.setSelectionMode(false);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
					m_text.gotoPrevWord();
				else
					m_text.goLeft();
				_limitCursor();
				break;
			case WG_KEY_RIGHT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);
				else
					m_text.setSelectionMode(false);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
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
				else if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					int ofs1 = m_text.column();
					m_text.gotoPrevWord();
					_limitCursor();
					int ofs2 = m_text.column();

					if( ofs2 < ofs1 )
					{
						m_text.Delete( ofs2, ofs1 - ofs2 );
						bTextChanged = true;
					}
//					m_text.delPrevWord();
				}
				else
				{
					if( m_text.column() > m_pFormat->getPrefix().Length() )
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
				else if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					int ofs1 = m_text.column();
					m_text.gotoNextWord();
					_limitCursor();
					int ofs2 = m_text.column();

					if( ofs2 > ofs1 )
					{
						m_text.Delete( ofs1, ofs2 - ofs1 );
						bTextChanged = true;
					}
//					m_text.delNextWord();
				}
				else
				{
					if( m_text.column() < m_text.Length() - m_pFormat->getSuffix().Length() )
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
				switch( pEvent->ModKeys() )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.GoBOL();
					_limitCursor();
					break;
				}

				break;

			case WG_KEY_END:

				/*
			 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
		 		 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( pEvent->ModKeys() )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.GoEOL();
					_limitCursor();
					break;
				}

				break;



			default:
				break;
		}
	}

	if( event == WG_EVENT_CHARACTER )
	{
		int character = WgCharacterEvent::Cast(pEvent)->Char();

		// Period is only allowed if it isn't displayed yet and value has decimals.
		// It's not allowed before a minus sign.
		// A zero is inserted (always INSERTED even if mode is replace) if there is no number before it.

		WgCursorInstance * pCursor = m_text.GetCursor();
		if( pCursor )
		{
			if( character == m_pFormat->getPeriod() )
			{
				if( m_pFormat->getDecimals() > 0 && m_text.getBuffer()->FindFirst( m_pFormat->getPeriod() ) == -1 &&
					(pCursor->column() != 0 || (*m_text.getBuffer())[0].Glyph() != '-' ) )
				{
					if(m_text.hasSelection())
						m_text.delSelection();
					m_text.setSelectionMode(false);

					if( m_text.Length() < m_maxInputChars )
					{
						if( pCursor->column() == 0 || (pCursor->column() == 1 && (*m_text.getBuffer())[0].Glyph() == '-' ) )
						{
							m_text.insertChar( 0, WgChar('0') );
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
				if( pCursor->column() == m_pFormat->getPrefix().Length() && m_text.getBuffer()->FindFirst( m_pFormat->getPeriod() ) == -1 &&
					m_rangeMin < 0 )
				{
					if(m_text.hasSelection())
						m_text.delSelection();
					m_text.setSelectionMode(false);

					if( m_text.Length() < m_maxInputChars )
						pCursor->putChar( '-' );
					bTextChanged = true;
				}
			}

			// Take care of typed numbers 0 through 9.

			if( character >= '0' && character <= '9' )
			{
				if( pCursor->column() == 0 && (*m_text.getBuffer())[0].Glyph() == '-' )
				{
				}
				else
				{
					if(m_text.hasSelection())
						m_text.delSelection();
					m_text.setSelectionMode(false);

					if( m_text.Length() < m_maxInputChars )
						pCursor->putChar( character );
					bTextChanged = true;
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
				pHandler->QueueEvent( new WgValueUpdateEvent(this,value,FractionalValue(),false) );

			_updateScrollbar( FractionalValue(), 0.f );
		}
	}
	
	// Swallow event depending on rules.

	if( pEvent->IsMouseButtonEvent() )
	{
		if( WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
			pHandler->SwallowEvent(pEvent);
	}
	else if( pEvent->IsKeyEvent() )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
		if( WgKeyEvent::Cast(pEvent)->IsMovementKey() == true ||
			key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE )
			pHandler->SwallowEvent(pEvent);
		
		//TODO: Would be good if we didn't forward any character-creating keys either...
	}
}

//____ _selectAll() ___________________________________________________________

void WgValueEditor::_selectAll()
{
	int min = m_pFormat->getPrefix().Length();
	int max = m_text.Length() - m_pFormat->getSuffix().Length();

	m_text.Select( min, max-min );
}

//____ _limitCursor() _________________________________________________________

void WgValueEditor::_limitCursor()
{
	int min = m_pFormat->getPrefix().Length();
	int max = m_text.Length() - m_pFormat->getSuffix().Length();

	WgCursorInstance * pCursor = m_text.GetCursor();

	// Save selection (might get destroyed by moving cursor)

//	bool bSelectionMode = pCursor->getSelectionMode();
//	WgRange sel = pCursor->getSelection();

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

void WgValueEditor::_onCloneContent( const WgWidget * _pOrg )
{
	WgValueEditor * pOrg = (WgValueEditor *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_maxInputChars = pOrg->m_maxInputChars;
	m_pFormat		= pOrg->m_pFormat;
	m_pUseFormat->setFormat(pOrg->m_pFormat);
	m_text.clone(&pOrg->m_text);
}

//____ onStateChanged() _______________________________________________________

void WgValueEditor::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	// Update text

	m_text.setState(m_state);
	_requestRender();				//TODO: Only render if text has been affected

	// Check if we got input focus

	if( m_state.IsFocused() && !oldState.IsFocused() )
	{
		_startReceiveTicks();
		m_text.showCursor();
		m_text.GoEOL();
		m_pUseFormat = m_pFormat;

		if( m_pFormat->getDecimals() != 0 )
			m_pUseFormat->setForcePeriod(true);	// Force period if decimals are involved.

		if( m_value < 0.f )
			m_pUseFormat->setZeroIsNegative(true);	// Force minus sign if value is negative.

		m_text.setScaledValue( m_value, m_pFormat->_getScale(), m_pUseFormat );

		_requestRender();
	}

	// Check if we lost input focus

	if( !m_state.IsFocused() && oldState.IsFocused() )
	{
		_stopReceiveTicks();
		_queueEvent( new WgValueUpdateEvent(this,m_value,FractionalValue(), true) );

		m_text.hideCursor();
		m_pUseFormat->setFormat(m_pFormat);
		_regenText();

		_requestRender();
	}
}


//____ _onSkinChanged() _______________________________________________________

void WgValueEditor::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}

