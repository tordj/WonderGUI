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


#include <wg_gizmo_editvalue.h>

#include <math.h>
#include <stdlib.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_cursorinstance.h>
#include <wg_key.h>
#include <wg_textmanager.h>

static const char Wdg_Type[] = {"EditValue"};

using namespace WgSignal;



//____ WgGizmoEditvalue() _________________________________________________________________

WgGizmoEditvalue::WgGizmoEditvalue()
{
	_regenText();
	m_buttonDownOfs = 0;
	m_bSelectAllOnRelease = false;

	m_pointerStyle	= WG_POINTER_IBEAM;

	m_text.SetWrap(false);
}

//____ ~WgGizmoEditvalue() ___________________________________________________________

WgGizmoEditvalue::~WgGizmoEditvalue()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoEditvalue::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoEditvalue::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetTextColor() _________________________________________________________

void WgGizmoEditvalue::SetTextColor( WgColor col )
{
	m_text.setColor( col );
	_regenText();			// Is this necessary?
	RequestRender();
}

WgColor WgGizmoEditvalue::GetTextColor() const
{
	return m_text.getColor(WG_MODE_NORMAL);
}

//____ SetTextProp() __________________________________________________________

bool WgGizmoEditvalue::SetTextProp( const WgTextPropPtr& _pProp )
{
	if( _pProp != m_text.getProperties() )
	{
		m_text.setProperties(_pProp);
		_regenText();		// Is this necessary?
		RequestRender();
	}

	return true;
}

WgTextPropPtr WgGizmoEditvalue::GetTextProp( ) const
{
	return m_text.getProperties();
}


//____ SetFormat() ____________________________________________________________

void WgGizmoEditvalue::SetFormat( const WgValueFormat& format )
{
	m_format		= format;
	m_useFormat		= format;
	RequestRender();
}

//____ SetTextAlignment() _____________________________________________________

void WgGizmoEditvalue::SetTextAlignment( const WgOrigo& origo )
{
	if( m_text.alignment() != origo )
	{
		m_text.setAlignment(origo);
		RequestRender();
	}
}

//____ GetTextAlignment() _____________________________________________________

WgOrigo	WgGizmoEditvalue::GetTextAlignment( ) const
{
	return m_text.alignment();
}

//____ SetTextManager() _______________________________________________________

void WgGizmoEditvalue::SetTextManager(WgTextManager * _pManager)
{
	if( m_text.getManager() != _pManager )
	{
		m_text.setManager( _pManager );
	}
}

//____ GetTextManager() _______________________________________________________

WgTextManager * WgGizmoEditvalue::GetTextManager() const
{
	return m_text.getManager();
}


//____ Clear() ________________________________________________________________

void WgGizmoEditvalue::Clear()
{
	// Force value to zero, emit signals and request render

	m_value = 0;
	_valueModified();

	// Make the inputfield empty

	m_useFormat.integers = 0;
	m_useFormat.bForcePeriod = false;
	m_useFormat.decimals = 0;
}

//____ _valueModified() ________________________________________________________

void WgGizmoEditvalue::_valueModified()
{
	Emit( IntegerChanged(), m_value );
	Emit( Fraction(), FractionalValue() );

	m_useFormat	= m_format;

	if( 0 != m_format.noDecimalThreshold && (int)m_value >= m_format.noDecimalThreshold )
	{
		// value is >= noDecimalThreshold so prevent period and decimals
		m_useFormat.bForcePeriod = false;
		m_useFormat.decimals = 0;
	}
	_regenText();
	RequestRender();
}

//____ _rangeModified() ________________________________________________________

void WgGizmoEditvalue::_rangeModified()
{
	Emit( Fraction(), FractionalValue() );
}


//____ _onRefresh() ____________________________________________________________

void WgGizmoEditvalue::_onRefresh( void )
{
	if( m_text.getFont() != 0 )
	{
		_regenText();
		RequestRender();
	}

}

//____ _onUpdate() _____________________________________________________________

void WgGizmoEditvalue::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_text.GetCursor() )
	{
		m_text.GetCursor()->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor!
	}
}


//____ _onRender() _____________________________________________________________

void WgGizmoEditvalue::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_text.getFont() == 0 )
		return;

	// Leave if we have nothing to print

	if( m_text.nbLines() == 0 )
		return;

	// Print the text

	pDevice->PrintText( _clip, &m_text, _canvas );
}

//____ _regenText() ____________________________________________________________

void WgGizmoEditvalue::_regenText()
{
		m_text.setScaledValue( m_value, m_format.scale, m_useFormat );
		m_text.goEOL();
}

//____ _parseValueFromInput() __________________________________________________

bool WgGizmoEditvalue::_parseValueFromInput( int64_t * wpResult )
{
	// Parse text as value as a positive or negative integer within boundaries
	// return false if value had to be modified in any way.

	const WgChar * p = m_text.getBuffer()->Chars();
	int		nbChars = m_text.getBuffer()->NbChars();

	int64_t	value = 0;
	bool	bModified = false;
	int		nDecimals = 0;

	if( nbChars > 0 )
	{
		for( int i = 0 ; i < nbChars ; i++ )
		{
			Uint16 glyph = p[i].glyph;

			if( glyph == '-' )
				continue;

			if( glyph == m_format.period )
			{
				nDecimals = nbChars-i-1;
				if( nDecimals > m_format.decimals )
				{
					// Only parse as many decimals as we need, avoid unnecessary overflows.
					nbChars -= nDecimals - m_format.decimals;
					nDecimals = m_format.decimals;
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

		if( nDecimals < m_format.decimals )
		{
			int64_t multiplier = (int64_t) pow(10.0,m_format.decimals-nDecimals);

			if( value <= LLONG_MAX/multiplier )
				value *= multiplier;
			else
			{
				value = LLONG_MAX;
				bModified = true;
			}
		}

		if( p[0].glyph == '-' )
			value = -value;

	}

	* wpResult = value;
	return bModified;
}



//____ _onAction() _____________________________________________________________

void WgGizmoEditvalue::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	bool	bTextChanged = false;

	WgCoord ofs = Abs2local(WgCoord(info.x,info.y));

	if( action == WgInput::BUTTON_PRESS && button_key == 1 )
	{
		if( !m_bFocused )
		{		
			GrabFocus();
			m_bSelectAllOnRelease = true;
		}
		else
			m_bSelectAllOnRelease = false;

		if( info.modifier & WG_MODKEY_SHIFT )
		{
			m_text.setSelectionMode(true);
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
		}
		else
		{		
			m_text.setSelectionMode(false);
			m_text.clearSelection();
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
			m_text.setSelectionMode(true);
		}

		m_buttonDownOfs = ofs.x;
	}

	if( action == WgInput::BUTTON_DOWN && button_key == 1 )
	{
		if( m_bFocused && ofs.x != m_buttonDownOfs )
		{
			m_text.CursorGotoCoord( ofs, WgRect(0,0,Size()) );
			m_buttonDownOfs = ofs.x;
			m_bSelectAllOnRelease = false;
		}
	}

	if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if( m_bFocused && button_key == 1 )
		{
			m_text.setSelectionMode(false);

			if( m_bSelectAllOnRelease )
				m_text.selectAll();
		}
	}


	if( action == WgInput::BUTTON_DOUBLECLICK && button_key == 1 )
	{
		m_text.selectAll();
		m_text.setSelectionMode(true);
	}


	if( action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT )
	{
		switch( button_key )
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
					Emit( IntegerChanged(), m_value );
					Emit( Fraction(), FractionalValue() );

					m_text.setScaledValue( m_value, m_format.scale, m_useFormat );
					m_text.GetCursor()->goEOL();
				}
				else
				{
					ReleaseFocus();
				}

			}
			break;

			case WG_KEY_LEFT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);
				else
					m_text.setSelectionMode(false);

				if( info.modifier & WG_MODKEY_CTRL )
					m_text.gotoPrevWord();
				else
					m_text.goLeft();
				break;
			case WG_KEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);
				else
					m_text.setSelectionMode(false);

				if( info.modifier & WG_MODKEY_CTRL )
					m_text.gotoNextWord();
				else
					m_text.goRight();
				break;

			case WG_KEY_BACKSPACE:
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_text.delPrevWord();
				else
					m_text.delPrevChar();

				bTextChanged = true;
				break;

			case WG_KEY_DELETE:
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( info.modifier & WG_MODKEY_CTRL )
					m_text.delNextWord();
				else
					m_text.delNextChar();

				bTextChanged = true;
				break;

			case WG_KEY_HOME:

				/*
				 *	I am not sure if this is the proper way to this, but in my opinion, the default
				 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( info.modifier )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( info.modifier & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.goBOL();
					break;
				}

				break;

			case WG_KEY_END:

				/*
			 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
		 		 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( info.modifier )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( info.modifier & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.goEOL();
					break;
				}

				break;



			default:
				break;
		}
	}

	if( action == WgInput::CHARACTER )
	{
		// Period is only allowed if it isn't displayed yet and value has decimals.
		// It's not allowed before a minus sign.
		// A zero is inserted (always INSERTED even if mode is replace) if there is no number before it.

		WgCursorInstance * pCursor = m_text.GetCursor();
		if( button_key == m_format.period )
		{
			if( m_format.decimals > 0 && m_text.getBuffer()->FindFirst( m_format.period ) == -1 &&
				(pCursor->column() != 0 || (*m_text.getBuffer())[0].glyph != '-' ) )
			{
				if(m_text.hasSelection())
					m_text.delSelection();
				m_text.setSelectionMode(false);

				if( pCursor->column() == 0 || (pCursor->column() == 1 && (*m_text.getBuffer())[0].glyph == '-' ) )
				{
					m_text.insertChar( 0, WgChar('0') );
					pCursor->goRight();
				}

				pCursor->putChar( m_format.period );
				bTextChanged = true;
			}
		}

		// Handle minus
		// Only allow minus at start of text and only if range allows negative values.

		if( button_key == '-' )
		{
			if( pCursor->column() == 0 && m_text.getBuffer()->FindFirst( m_format.period ) == -1 &&
				m_rangeMin < 0 )
			{
				if(m_text.hasSelection())
					m_text.delSelection();
				m_text.setSelectionMode(false);

				pCursor->putChar( '-' );
				bTextChanged = true;
			}
		}

		// Take care of typed numbers 0 through 9.

		if( button_key >= '0' && button_key <= '9' )
		{
			if( pCursor->column() == 0 && (*m_text.getBuffer())[0].glyph == '-' )
			{
			}
			else
			{
				if(m_text.hasSelection())
					m_text.delSelection();
				m_text.setSelectionMode(false);

				pCursor->putChar( button_key );
				bTextChanged = true;
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
			Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
			Emit( IntegerChanged(), m_value );
			Emit( Fraction(), FractionalValue() );
		}
	}
}


//____ _onCloneContent() _______________________________________________________

void WgGizmoEditvalue::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoEditvalue * pOrg = (WgGizmoEditvalue *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_format		= pOrg->m_format;
	m_text.setText(&pOrg->m_text);
	m_text.setFont(pOrg->m_text.getFont());
	m_textOrigo	= pOrg->m_textOrigo;
}

//____ _onEnable() _____________________________________________________________

void WgGizmoEditvalue::_onEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoEditvalue::_onDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}


//____ _onGotInputFocus() ______________________________________________________

void WgGizmoEditvalue::_onGotInputFocus()
{
	m_bFocused = true;
	m_text.CreateCursor();
	m_text.GetCursor()->goEOL();
	m_useFormat = m_format;

	if( m_format.decimals != 0 )
		m_useFormat.bForcePeriod = true;	// Force period if decimals are involved.

	if( m_value < 0.f )
		m_useFormat.bZeroIsNegative = true;	// Force minus sign if value is negative.

	m_text.setScaledValue( m_value, m_format.scale, m_useFormat );

	RequestRender();
}

//____ _onLostInputFocus() _____________________________________________________

void WgGizmoEditvalue::_onLostInputFocus()
{
	m_bFocused = false;
	m_text.DestroyCursor();
	m_useFormat = m_format;
	_regenText();

	RequestRender();
}


//____ SelectAllText() __________________________________________________________

bool WgGizmoEditvalue::SelectAllText()
{
	if( !m_text.GetCursor() )
	{
		if( !GrabFocus() )
			return false;				// Couldn't get cursor.
	}

	m_text.selectAll();
	return true;
}
