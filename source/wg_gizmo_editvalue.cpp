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


static const char Wdg_Type[] = {"TordJ/EditValue"};

using namespace WgSignal;



//____ WgGizmoEditvalue() _________________________________________________________________

WgGizmoEditvalue::WgGizmoEditvalue()
{
	m_bRegenText	= true;

	m_pMyCursor		= 0;

	m_cursorStyle	= WG_CURSOR_IBEAM;

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
	m_text.setDefaultColor( col );
	m_bRegenText		= true;			// Is this necessary?
	RequestRender();
}

WgColor WgGizmoEditvalue::GetTextColor() const
{
	return m_text.getDefaultColor(WG_MODE_NORMAL);
}

//____ SetTextProp() __________________________________________________________

bool WgGizmoEditvalue::SetTextProp( const WgTextPropPtr& _pProp )
{
	if( _pProp != m_text.getDefaultProperties() )
	{
		m_text.setDefaultProperties(_pProp);
		m_bRegenText		= true;		// Is this necessary?
		RequestRender();
	}

	return true;
}

WgTextPropPtr WgGizmoEditvalue::GetTextProp( ) const
{
	return m_text.getDefaultProperties();
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

WgOrigo	WgGizmoEditvalue::GetTextAlignment( ) const
{
	return m_text.alignment();
}

//____ Clear() ________________________________________________________________

void WgGizmoEditvalue::Clear()
{
	// Force value to zero, emit signals and request render

	m_value = 0;
	ValueModified();

	// Make the inputfield empty

	m_useFormat.integers = 0;
	m_useFormat.bForcePeriod = false;
	m_useFormat.decimals = 0;
}

//____ ValueModified() ________________________________________________________

void WgGizmoEditvalue::ValueModified()
{
	if( m_pHook )
	{
		m_pHook->GetEmitter()->Emit( IntegerChanged(), m_value );
		m_pHook->GetEmitter()->Emit( Fraction(), FractionalValue() );
	}

	m_useFormat	= m_format;

	if( 0 != m_format.noDecimalThreshold && (int)m_value >= m_format.noDecimalThreshold )
	{
		// value is >= noDecimalThreshold so prevent period and decimals
		m_useFormat.bForcePeriod = false;
		m_useFormat.decimals = 0;
	}
	m_bRegenText = true;
	RequestRender();
}

//____ RangeModified() ________________________________________________________

void WgGizmoEditvalue::RangeModified()
{
	if( m_pHook )
		m_pHook->GetEmitter()->Emit( Fraction(), FractionalValue() );
}


//____ OnRefresh() ____________________________________________________________

void WgGizmoEditvalue::OnRefresh( void )
{
	if( m_text.getFontSet() != 0 )
	{
		m_bRegenText = true;
		RequestRender();
	}

}

//____ OnUpdate() _____________________________________________________________

void WgGizmoEditvalue::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_pMyCursor )
	{
		m_pMyCursor->incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor!
	}
}


//____ OnRender() _____________________________________________________________

void WgGizmoEditvalue::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_text.getFontSet() == 0 )
		return;

	// Possibly regenerate the text

	if( m_bRegenText )
	{
		m_text.setScaledValue( m_value, (Uint32) pow(10.f,m_format.decimals), m_useFormat );
		m_bRegenText = false;
	}

	// Make sure cursor is at EOL - length of suffix.

	if( m_pMyCursor )
	{
		int i = 0;
		while( m_format.suffix[i] != 0 && i < 4)
			i++;

		m_pMyCursor->goEOL();
		m_pMyCursor->goLeft( i );
	}



	// Leave if we have nothing to print

	if( m_text.nbLines() == 0 )
		return;

	// Print the text

	if( m_pMyCursor )
		pDevice->ClipPrintTextWithCursor( _clip, &m_text, *m_pMyCursor, _window );
	else
		pDevice->ClipPrintText( _clip, &m_text, _window );
}

//____ OnAction() _____________________________________________________________

void WgGizmoEditvalue::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	if( action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT )
	{
		switch( button_key )
		{
			case WGKEY_BACKSPACE:
			{
				if( m_useFormat.decimals != 0 )
					m_useFormat.bForcePeriod = true;	// Force period if decimals are involved.


				int value = m_value;

				// Remove decimals, remove period, decrease value or remove minus sign.

				if( m_useFormat.decimals != 0 )
					m_useFormat.decimals--;
				else if( m_useFormat.bForcePeriod )
					m_useFormat.bForcePeriod = false;
				else
				{
					value /= 10;
					if( m_useFormat.integers )
						m_useFormat.integers--;
					else if( m_useFormat.bZeroIsNegative )
						m_useFormat.bZeroIsNegative = false;
				}
				// Mask away decimals not shown.

				int mask = (int) pow( 10.f, m_format.decimals - m_useFormat.decimals );
				value -= value%mask;

				// Set value, possibly emit signals

				if( value != m_value )
				{
					m_value = value;
					pEmitter->Emit( IntegerChanged(), m_value );
					pEmitter->Emit( Fraction(), FractionalValue() );
				}

				// Force render

				m_bRegenText = true;
				RequestRender();

			}
			break;

			default:
				break;
		}
	}

	if( action == WgInput::CHARACTER )
	{
		// Period is only allowed if it isn't displayed yet and format allows periods.

		if( button_key == m_format.period )
		{
			// prevent period if value >= noDecimalThreshold
			if( 0 == m_format.noDecimalThreshold || (int)m_value < m_format.noDecimalThreshold )
			{
				if( m_format.decimals != 0 && !m_useFormat.bForcePeriod )
				{
					m_useFormat.integers = m_format.integers;
					m_useFormat.bForcePeriod = true;
					m_bRegenText = true;
					RequestRender();
				}
			}
		}

		// Handle minus

		if( button_key == '-' )
		{
			if( m_rangeMin < 0 && m_useFormat.integers == 0 &&
				!m_useFormat.bForcePeriod && m_useFormat.decimals == 0 &&
				!m_useFormat.bZeroIsNegative )
			{
				m_useFormat.bZeroIsNegative = true;
				m_bRegenText = true;
				RequestRender();
			}
		}

		// Take care of typed numbers 0 through 9.

		if( button_key >= '0' && button_key <= '9' )
		{
			int number = button_key - '0';

			if( m_useFormat.bForcePeriod )
			{
				// We have a period, so we are typing decimals...

				if( m_useFormat.decimals < m_format.decimals )
				{
					int value = abs(m_value) + number * (int) pow(10.f, m_format.decimals - m_useFormat.decimals -1 );

					// Possibly make value negative

					if( m_useFormat.bZeroIsNegative )
						value = 0-value;


					if( (value >= m_rangeMin || value >= 0) && value <= m_rangeMax )
					{
						m_useFormat.decimals++;

						// Set value, possibly emit signals

						if( value != m_value )
						{
							m_value = value;
							pEmitter->Emit( IntegerChanged(), m_value );
							pEmitter->Emit( Fraction(), FractionalValue() );
						}

						// Force render

						m_bRegenText = true;
						RequestRender();
					}
				}
			}
			else
			{
				// No period displayed, so we are typing integer-part.

				int value = abs(m_value)*10 + number * (int) pow(10.f, m_format.decimals );

				// Possibly make value negative

				if( m_useFormat.bZeroIsNegative )
					value = 0-value;


				if( (value >= m_rangeMin || value >= 0) && value <= m_rangeMax )
				{
					if( m_useFormat.integers < m_format.integers )
						m_useFormat.integers++;

					// Set value, possibly emit signals

					if( value != m_value )
					{
						m_value = value;
						pEmitter->Emit( IntegerChanged(), m_value );
						pEmitter->Emit( Fraction(), FractionalValue() );
					}

					// Force render

					m_bRegenText = true;
					RequestRender();
				}
			}
		}
	}
}


//____ OnCloneContent() _______________________________________________________

void WgGizmoEditvalue::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoEditvalue * pOrg = (WgGizmoEditvalue *) _pOrg;

	Wg_Interface_ValueHolder::OnCloneContent( pOrg );

	m_format		= pOrg->m_format;
	m_text.setText(&pOrg->m_text);
	m_text.setFontSet(pOrg->m_text.getFontSet());
	m_textOrigo	= pOrg->m_textOrigo;
}

//____ OnEnable() _____________________________________________________________

void WgGizmoEditvalue::OnEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ OnDisable() ____________________________________________________________

void WgGizmoEditvalue::OnDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}


//____ OnGotInputFocus() ______________________________________________________

void WgGizmoEditvalue::OnGotInputFocus()
{
	m_pMyCursor = new WgCursorInstance( m_text );
	m_useFormat = m_format;
	m_bRegenText = true;

	if( m_format.decimals != 0 )
		m_useFormat.bForcePeriod = true;	// Force period if decimals are involved.

	if( m_value < 0.f )
		m_useFormat.bZeroIsNegative = true;	// Force minus sign if value is negative.

	RequestRender();
}

//____ OnLostInputFocus() _____________________________________________________

void WgGizmoEditvalue::OnLostInputFocus()
{
	delete m_pMyCursor;
	m_pMyCursor = 0;
	m_useFormat = m_format;
	m_bRegenText = true;

	RequestRender();
}
