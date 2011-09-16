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


#include <wg_gizmo_combobox.h>
#include <wdg_menu.h>
#include <wg_geo.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

static const char	c_gizmoType[] = {"ComboBox"};


//____ WgGizmoCombobox() _________________________________________________________________

WgGizmoCombobox::WgGizmoCombobox( void )
{
	m_pText = &m_text;
	m_text.setAlignment( WgOrigo::midLeft() );
	m_text.SetWrap(false);
//	m_textColor = m_text.color();
//	m_textDisabledColor	= 0xFFFF;

	m_mode = WG_MODE_NORMAL;

	m_bEditable = false;
	m_pMenu		= 0;
	m_pSelectedItem = 0;
	m_textFormat = "%1";
}

//____ Destructor _____________________________________________________________

WgGizmoCombobox::~WgGizmoCombobox()
{
	delete m_pMenu;
}


//____ Type() _________________________________________________________________

const char * WgGizmoCombobox::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoCombobox::GetMyType( void )
{
	return c_gizmoType;
}


//____ SetSource() ____________________________________________________________

void WgGizmoCombobox::SetSource( const WgBlockSetPtr& pTextBox )
{
	m_pTextBoxBg	= pTextBox;

	RequestRender();
}

//____ SetMenu() ______________________________________________________________

void WgGizmoCombobox::SetMenu( Wdg_Menu * pMenu )
{
	if( pMenu == m_pMenu )
		return;

	if( m_pMenu )
	{
		m_pMenu->RemoveCallback( WgSignal::MenuEntrySelected(), (void*) cbEntrySelected, this );
		m_pMenu->RemoveCallback( WgSignal::MenuClosed(), (void*) cbMenuClosed, this );
		delete m_pMenu;
	}

	if( pMenu )
	{
		pMenu->AddCallback( WgSignal::MenuEntrySelected(), cbEntrySelected, this );
		pMenu->AddCallback( WgSignal::MenuClosed(), cbMenuClosed, this );
	}
	m_pMenu = pMenu;
	EntrySelected(m_pMenu->GetSelectedItem());
}

//____ GetMenu() __________________________________________________________

Wdg_Menu* WgGizmoCombobox::GetMenu() const
{
	return m_pMenu;
}

//____ SetEditable() __________________________________________________________

void WgGizmoCombobox::SetEditable( bool bEditable )
{
	// Editable NOT supported yet.

//	m_bEditable = bEditable;
}

//_____________________________________________________________________________
void WgGizmoCombobox::SetTextFormat( const WgCharSeq& str )
{
	m_textFormat = str;
	if( m_pMenu )
		EntrySelected(m_pMenu->GetSelectedItem());
}

//____ _onAction() _____________________________________________________________

void WgGizmoCombobox::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	WgMode	newMode = m_mode;

	switch( action )
	{
		case	WgInput::POINTER_ENTER:
			if( m_mode == WG_MODE_SELECTED )
				break;

			newMode = WG_MODE_MARKED;
			break;

		case	WgInput::POINTER_EXIT:
			if( m_mode == WG_MODE_SELECTED )
				break;

			newMode = WG_MODE_NORMAL;
			break;

		case WgInput::BUTTON_PRESS:
			if( button_key == 1 )
			{
				if( m_pMenu )
				{
					WgRect r( Local2abs(WgCord(0,0)), Size() );

					if( m_pHook )
					{
						WgWidget * pRoot = m_pHook->GetRoot();
						int yPos;

						if( pRoot->Height() < r.y + r.h + m_pMenu->Height() )
							yPos = r.y-m_pMenu->Height();
						else
							yPos = r.y+r.h;

						m_pMenu->Open( (Wdg_Root*) pRoot, r.x, yPos, r.w, m_pSelectedItem, &r );
					}
				}

				newMode = WG_MODE_SELECTED;
			}
			break;

		case WgInput::WHEEL_ROLL:
			if( m_pSelectedItem )
			{
				WgMenuItem * pItem = m_pSelectedItem;
				int distance = info.rolldistance;

				while( distance < 0 && pItem->Prev() )
				{
					pItem = pItem->Prev();
					distance++;
				}

				while( distance > 0 && pItem->Next() )
				{
					pItem = pItem->Next();
					distance--;
				}

				m_pMenu->SelectItem( pItem );
			}
			break;

		case WgInput::KEY_PRESS:
		case WgInput::KEY_REPEAT:
			break;

        default:
            break;

	}

	if( newMode != m_mode )
	{
		m_mode = newMode;
		RequestRender();
	}

}

//____ _onRender() ________________________________________________________

void WgGizmoCombobox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render the textbox
	if(m_pTextBoxBg)
	{
		const WgBlock&	block = m_pTextBoxBg->GetBlock( m_mode, _canvas.size() );
		WgRect			dest( _canvas );
		pDevice->ClipBlitBlock( _clip, block, dest );
	}

	// Print the text

	WgRect r( _canvas );
	if( m_pTextBoxBg )
		r.shrink( m_pTextBoxBg->GetContentBorders() );

	WgRect	textClip( r, _clip );

	if(m_pTextBoxBg)
		m_text.SetBgBlockColors(m_pTextBoxBg->GetTextColors());

	m_text.setMode( m_mode );
	pDevice->PrintText( textClip, &m_text, r );
}

//____ _onRefresh() _______________________________________________________

void WgGizmoCombobox::_onRefresh( void )
{
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoCombobox::_onCloneContent( const WgGizmo * _pOrg )
{
}

//____ _onAlphaTest() ______________________________________________________

bool WgGizmoCombobox::_onAlphaTest( const WgCord& ofs )
{
	if( !m_pTextBoxBg )
		return false;

	WgSize sz = Size();

	return WgUtil::MarkTestBlock( ofs, m_pTextBoxBg->GetBlock(m_mode,sz), WgRect( WgCord(0,0), sz ) );
}

//____ _onGotInputFocus() ______________________________________________

void WgGizmoCombobox::_onGotInputFocus()
{
	m_bFocused = true;
}

//____ _onLostInputFocus() ______________________________________________

void WgGizmoCombobox::_onLostInputFocus()
{
	m_bFocused = false;
}


//____ _onEnable() _____________________________________________________________

void WgGizmoCombobox::_onEnable( void )
{
	WgMode newMode;
	WgMode oldMode = m_mode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( m_pTextBoxBg && !m_pTextBoxBg->SameBlock(newMode, oldMode) )
		RequestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoCombobox::_onDisable( void )
{
	WgMode newMode;
	WgMode oldMode = m_mode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( m_pTextBoxBg && !m_pTextBoxBg->SameBlock(newMode, oldMode) )
		RequestRender();
}


//____ TextModified() _________________________________________________________

void WgGizmoCombobox::TextModified()
{
	RequestRender();
}

//____ MenuClosed() ___________________________________________________________

void WgGizmoCombobox::MenuClosed()
{
	m_mode = WG_MODE_NORMAL;
	RequestRender();
}


//____ EntrySelected() ________________________________________________________

void WgGizmoCombobox::EntrySelected(WgMenuItem * pItem)
{
	m_pSelectedItem = pItem;
	if(pItem && pItem->GetType() != SEPARATOR)
	{
		WgCharBuffer	buff;

		buff.PushBack(m_textFormat);

		int ofs = buff.FindFirst( "%1" );
		if( ofs >= 0 )
		{
			WgTextPropPtr pProp = buff[ofs].Properties();

			const WgChar * pEntry = ((WgMenuEntry*)pItem)->GetText().Chars();
			Uint32 len = WgTextTool::strlen( pEntry );

			buff.Replace( ofs, 2, pEntry );

			WgChar * p = buff.BeginWrite() + ofs;

			for( unsigned int i = 0 ; i < len ; i++ )
			{
				if( p[i].PropHandle() == 0 )
					p[i].SetProperties( pProp );
			}

			buff.EndWrite();

		}
		SetText( buff.Chars() );
	}
}
