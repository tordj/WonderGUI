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

static const char	s_type[] = {"TordJ/ComboBox"};


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
	return s_type;
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

//____ OnAction() _____________________________________________________________

void WgGizmoCombobox::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
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

						m_pMenu->Open( (Wdg_Root*) pRoot, r.x, yPos, r.w, &r );
					}
				}

				newMode = WG_MODE_SELECTED;
			}
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

//____ OnRender() ________________________________________________________

void WgGizmoCombobox::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render the textbox
	if(m_pTextBoxBg)
	{
		const WgBlock&	block = m_pTextBoxBg->GetBlock( m_mode );
		WgRect			dest( _window.x, _window.y, _window.w, _window.h );
		pDevice->ClipBlitBlock( _clip, block, dest );
	}

	// Print the text

	WgRect r( _window );
	if( m_pTextBoxBg )
		r.Shrink( m_pTextBoxBg->GetContentBorders() );
	
	WgRect	textClip( r, _clip );

	m_text.setMode( m_mode );
	pDevice->PrintText( textClip, &m_text, r );
}

//____ OnRefresh() _______________________________________________________

void WgGizmoCombobox::OnRefresh( void )
{
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoCombobox::OnCloneContent( const WgGizmo * _pOrg )
{
}

//____ OnMarkTest() ______________________________________________________

bool WgGizmoCombobox::OnMarkTest( const WgCord& ofs )
{
	if( !m_pTextBoxBg )
		return false;

	return WgUtil::MarkTestBlock( ofs.x, ofs.y, m_pTextBoxBg->GetBlock(m_mode), WgRect( WgCord(0,0), Size() ) );
}

//____ OnGotInputFocus() ______________________________________________

void WgGizmoCombobox::OnGotInputFocus()
{
}

//____ OnLostInputFocus() ______________________________________________

void WgGizmoCombobox::OnLostInputFocus()
{
}


//____ OnEnable() _____________________________________________________________

void WgGizmoCombobox::OnEnable( void )
{
	WgMode newMode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( !m_pTextBoxBg->SameBlock(newMode, m_mode) )
		RequestRender();
}

//____ OnDisable() ____________________________________________________________

void WgGizmoCombobox::OnDisable( void )
{
	WgMode newMode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( !m_pTextBoxBg->SameBlock(newMode, m_mode) )
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
			WgTextPropPtr pProp = buff[ofs].GetProperties();

			const WgChar * pEntry = ((WgMenuEntry*)pItem)->GetText().GetChars();
			Uint32 len = WgTextTool::strlen( pEntry );

			buff.Replace( ofs, 2, pEntry );
			
			WgChar * p = buff.BeginWrite() + ofs;

			for( unsigned int i = 0 ; i < len ; i++ )
			{
				if( p[i].GetPropHandle() == 0 )
					p[i].SetProperties( pProp );
			}

			buff.EndWrite();

		}
		SetText( buff.GetChars() );
	}
}
