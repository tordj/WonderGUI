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


#include <wg_combobox.h>
#include <wg_menu.h>
#include <wg_geo.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"Combobox"};


//____ WgCombobox() _________________________________________________________________

WgCombobox::WgCombobox( void )
{
	m_pText = &m_text;
	m_text.setAlignment( WG_WEST );
	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());	
//	m_textColor = m_text.color();
//	m_textDisabledColor	= 0xFFFF;

	m_mode = WG_MODE_NORMAL;

	m_bResetCursorOnFocus = true;
	m_bPressInInputRect = false;
	m_pMenu		= 0;
	m_pSelectedItem = 0;
	m_textFormat = "%1";
	m_viewOfs		= 0;
	m_maxCharacters = 0;
}

//____ Destructor _____________________________________________________________

WgCombobox::~WgCombobox()
{
	delete m_pMenu;
}


//____ Type() _________________________________________________________________

const char * WgCombobox::Type( void ) const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgCombobox::GetClass( void )
{
	return c_gizmoType;
}


//____ SetSource() ____________________________________________________________

void WgCombobox::SetSource( const WgBlocksetPtr& pTextBox )
{
	m_pTextBoxBg	= pTextBox;

	_requestRender();
}

//____ DefaultSize() __________________________________________________________

WgSize WgCombobox::DefaultSize() const
{
	//TODO: Implement!!!

	return WgSize(40,20);
}


//____ SetMenu() ______________________________________________________________

void WgCombobox::SetMenu( WgMenu * pMenu )
{
	if( pMenu == m_pMenu )
		return;
/*
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
*/
	m_pMenu = pMenu;
	_entrySelected(m_pMenu->GetSelectedItem());
}

//____ GetMenu() __________________________________________________________

WgMenu* WgCombobox::GetMenu() const
{
	return m_pMenu;
}

//____ SetEditMode() __________________________________________________________

void WgCombobox::SetEditMode(WgTextEditMode mode)
{
	m_text.SetEditMode(mode);
}

//____ SetTextFormat() ________________________________________________________

void WgCombobox::SetTextFormat( const WgCharSeq& str )
{
	m_textFormat = str;
	if( m_pMenu )
		_entrySelected(m_pMenu->GetSelectedItem());
}

//____ SetPlaceholderText() ___________________________________________________

void WgCombobox::SetPlaceholderText( const WgCharSeq& str )
{
	m_placeholderText = str;
	if( m_text.IsEmpty() && !m_text.isCursorShowing() )
		_requestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgCombobox::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !_isEditable() )
		return 0;

	if( !m_bFocused )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = 0;

	if( m_maxCharacters == 0 || str.Length() < m_maxCharacters - m_pText->nbChars() )
	{
		m_pText->putText( str );
		retVal = str.Length();
	}
	else
	{
		retVal = m_maxCharacters - m_pText->nbChars();
		m_pText->putText( WgCharSeq( str, 0, retVal ) );
	}

	WgEventHandler * pHandler = _eventHandler();		
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );

	_adjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgCombobox::InsertCharAtCursor( Uint16 c )
{
	if( !_isEditable() )
		return 0;

	if( !m_bFocused )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( m_maxCharacters != 0 && m_maxCharacters < m_pText->nbChars() )
		return false;

	m_pText->putChar( c );

	WgEventHandler * pHandler = _eventHandler();		
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );

	_adjustViewOfs();
	return true;
}

//____ goBOL() ________________________________________________________________

void WgCombobox::goBOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________

void WgCombobox::goEOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goEOL();
}


//____ _onEvent() _____________________________________________________________

void WgCombobox::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
	}
}

//____ _onAction() _____________________________________________________________
/*
void WgCombobox::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
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

		case	WgInput::POINTER_OVER:
		{
			WgRect gizmoRect( Local2abs(WgCoord(0,0)), Size() );
			WgRect inputRect = gizmoRect - m_pTextBoxBg->ContentBorders();

			if( _isSelectable() && inputRect.Contains(info.x,info.y) )
			{
				m_pointerStyle = WG_POINTER_IBEAM;
			}
			else
			{
				m_pointerStyle = WG_POINTER_DEFAULT;
			}
			break;
		}



		case WgInput::BUTTON_PRESS:
			if( button_key == 1 )
			{
				WgRect gizmoRect( Local2abs(WgCoord(0,0)), Size() );
				WgRect inputRect = gizmoRect;
				if( m_pTextBoxBg )
					inputRect -= m_pTextBoxBg->ContentBorders();

				if( _isEditable() && inputRect.Contains(info.x,info.y) )
				{
					if( !m_bFocused )
					{
						GrabFocus();
						if( m_bFocused )
							m_bFocusPress = true;		// Current button press brought focus.
					}

					m_bPressInInputRect = true;

					if( m_bFocused )
					{
						if( _isSelectable() && (info.modifier & WG_MODKEY_SHIFT) )
						{
							m_pText->setSelectionMode(true);
						}

						WgCoord ofs = Abs2local(WgCoord(info.x,0));

						int x = ofs.x;
						int y = ofs.y;
						x += m_viewOfs;

						m_pText->CursorGotoCoord( WgCoord(x, 0), WgRect(inputRect.x - gizmoRect.x,0,1000000,1000000) );
				
						if(_isSelectable() && !(info.modifier & WG_MODKEY_SHIFT))
						{
							m_pText->clearSelection();
							m_pText->setSelectionMode(true);
						}
						_adjustViewOfs();
					}

				}
				else if( m_pMenu )
				{
					m_bPressInInputRect = false;
					if( m_pHook )
					{
						WgWidget * pRoot = m_pHook->GetRoot();
						int yPos;

						if( pRoot->Height() < gizmoRect.y + gizmoRect.h + m_pMenu->Height() )
							yPos = gizmoRect.y-m_pMenu->Height();
						else
							yPos = gizmoRect.y+gizmoRect.h;

						m_pMenu->Open( (Wdg_Root*) pRoot, gizmoRect.x, yPos, gizmoRect.w, m_pSelectedItem, &gizmoRect );
					}
					newMode = WG_MODE_SELECTED;
				}
			}
			break;

		case WgInput::BUTTON_DOWN:
			if( button_key == 1 && m_bFocused && m_bPressInInputRect )
			{
				if( _isSelectable() && (info.modifier & WG_MODKEY_SHIFT) )
				{
					m_pText->setSelectionMode(true);
				}

				WgCoord ofs = Abs2local(WgCoord(info.x,0));

				int x = ofs.x;
				int y = ofs.y;
				x += m_viewOfs;

				int leftBorder = 0;
				if( m_pTextBoxBg )
					leftBorder = m_pTextBoxBg->ContentBorders().left;

				m_pText->CursorGotoCoord( WgCoord(x, 0), WgRect(leftBorder,0,1000000,1000000) );
				_adjustViewOfs();
			}

			break;

		case WgInput::BUTTON_RELEASE:
		case WgInput::BUTTON_RELEASE_OUTSIDE:
		{
			if( m_bFocused && button_key == 1 )
			{
				m_pText->setSelectionMode(false);
				if( m_bFocusPress )
				{
					m_bFocusPress = false;
					if( !m_pText->hasSelection() )
						m_pText->selectAll();
				}
			}
			break;
		}

		case WgInput::WHEEL_ROLL:
		if( !m_bFocused && m_pMenu && m_pMenu->GetItemCount() != 0 )
		{
			WgMenuItem * pItem = m_pSelectedItem;
			int distance = info.rolldistance;

			if( !pItem )
			{
				if( distance > 0 )
				{
					pItem = m_pMenu->GetLastItem();
					distance = 0; //distance--;			//TODO: Put back to distance-- once we have fixed Intobet client rolls a large distance for each step.
				}
				if( distance < 0 )
				{
					pItem = m_pMenu->GetFirstItem();
					distance = 0; //distance++;			//TODO: Put back to distance++ once we have fixed Intobet client rolls a large distance for each step.
				}
			}

			if( distance > 0 && pItem->Prev() )		//TODO: This used to be a while loop, but Intobet client rolls a large distance for each step.
			{
				pItem = pItem->Prev();
				distance--;
			}

			if( distance < 0 && pItem->Next() )		//TODO: This used to be a while loop, but Intobet client rolls a large distance for each step.
			{
				pItem = pItem->Next();
				distance++;
			}

			m_pMenu->SelectItem( pItem );
		}
		break;

		case WgInput::CHARACTER:
		{
			if( _isEditable() && m_bFocused && button_key >= 32 && button_key != 127)
			{

				if(m_pText->hasSelection())
					m_pText->delSelection();
				m_pText->setSelectionMode(false);

				// by default - no max limit
				if( m_maxCharacters == 0 || m_maxCharacters > m_pText->nbChars() )
					m_pText->putChar( button_key );

				Emit( WgSignal::TextChanged() );
				_adjustViewOfs();
			}
			break;
		}

		case WgInput::KEY_RELEASE:
			if( m_bFocused )
			{
				switch( button_key )
				{
					case WG_KEY_SHIFT:
						if(!inputObj.isButtonDown(1))
							m_pText->setSelectionMode(false);
					break;
				}
			}
		break;

		case WgInput::KEY_PRESS:
		case WgInput::KEY_REPEAT:
			if( _isEditable() && m_bFocused )
			{
				switch( button_key )
				{
					case WG_KEY_LEFT:
						if( info.modifier & WG_MODKEY_SHIFT )
							m_pText->setSelectionMode(true);

						if( info.modifier & WG_MODKEY_CTRL )
								m_pText->gotoPrevWord();
						else
							m_pText->goLeft();
						break;
					case WG_KEY_RIGHT:
						if( info.modifier & WG_MODKEY_SHIFT )
							m_pText->setSelectionMode(true);

						if( info.modifier & WG_MODKEY_CTRL )
								m_pText->gotoNextWord();
						else
							m_pText->goRight();
						break;

					case WG_KEY_BACKSPACE:
						if(m_pText->hasSelection())
							m_pText->delSelection();
						else if( info.modifier & WG_MODKEY_CTRL )
							m_pText->delPrevWord();
						else
							m_pText->delPrevChar();
						Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
						break;

					case WG_KEY_DELETE:
						if(m_pText->hasSelection())
							m_pText->delSelection();
						else if( info.modifier & WG_MODKEY_CTRL )
							m_pText->delNextWord();
						else
							m_pText->delNextChar();
						Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
						break;

					case WG_KEY_HOME:

						//
						//	I am not sure if this is the proper way to this, but in my opinion, the default
						//	"actions" has to be separated from any modifier key action combination
						//
						switch( info.modifier )
						{

						case WG_MODKEY_CTRL:
							break;

						default: // no modifier key was pressed
							if( info.modifier & WG_MODKEY_SHIFT )
								m_pText->setSelectionMode(true);

							m_pText->goBOL();
							break;
						}

						break;

					case WG_KEY_END:

						//
			 			//	I am not sure if this is the proper way to this, but in my opinion, the default
		 				//	"actions" has to be separated from any modifier key action combination
						//
						switch( info.modifier )
						{

						case WG_MODKEY_CTRL:
							break;

						default: // no modifier key was pressed
							if( info.modifier & WG_MODKEY_SHIFT )
								m_pText->setSelectionMode(true);

							m_pText->goEOL();
							break;
						}

						break;

					default:
						break;
				}
				_adjustViewOfs();
			}
		break;

        default:
            break;

	}

	if( newMode != m_mode )
	{
		m_mode = newMode;
		_requestRender();
	}

}
*/

//____ _onUpdate() _____________________________________________________________
/*
void WgCombobox::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( _isEditable() && m_bFocused )
	{
		m_text.incTime( _updateInfo.msDiff );
		_requestRender();					//TODO: Should only render the cursor and selection!
	}
}
*/

//____ _onRender() ________________________________________________________

void WgCombobox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render the textbox
	if(m_pTextBoxBg)
	{
		const WgBlock&	block = m_pTextBoxBg->GetBlock( m_mode, _canvas.Size() );
		WgRect			dest( _canvas );
		pDevice->ClipBlitBlock( _clip, block, dest );
	}

	// Print the text

	WgRect r( _canvas );
	if( m_pTextBoxBg )
		r.Shrink( m_pTextBoxBg->Padding() );

	WgRect	textClip( r, _clip );

	if(m_pTextBoxBg)
		m_text.SetBgBlockColors(m_pTextBoxBg->TextColors());

	bool bPlaceholder = false;
	if( !m_placeholderText.IsEmpty() && m_text.IsEmpty() && !m_text.isCursorShowing() )
	{
		bPlaceholder = true;
		m_text.setText( m_placeholderText );
	}

	m_text.setMode( m_mode );

	r.x -= m_viewOfs;
	r.w += m_viewOfs;
	pDevice->PrintText( textClip, &m_text, r );

	if( bPlaceholder )
		m_text.clear();
}

//____ _onRefresh() _______________________________________________________

void WgCombobox::_onRefresh( void )
{
}

//____ _onCloneContent() _______________________________________________________

void WgCombobox::_onCloneContent( const WgGizmo * _pOrg )
{
	const WgCombobox * pOrg = WgCast<WgCombobox>(_pOrg);
	if( pOrg )
	{
		m_textFormat = pOrg->m_textFormat;
		m_placeholderText = pOrg->m_placeholderText;
		m_pTextBoxBg = pOrg->m_pTextBoxBg;
		m_pMenu = pOrg->m_pMenu;
		m_pSelectedItem = pOrg->m_pSelectedItem;
		m_text = pOrg->m_text;
		m_maxCharacters = pOrg->m_maxCharacters;
	}


}



//____ _adjustViewOfs() ________________________________________________________

void WgCombobox::_adjustViewOfs()
{
	// Possibly move viewOfs so that:
	//	1 Cursor remains inside view.
	//  2 At least one character is displayed before the cursor
	//  3 At least one character is displayed after the cursor (if there is one).

	if( m_bFocused && m_pText->getFont() )
	{
		WgCursor * pCursor = WgTextTool::GetCursor( m_pText );
		if( !pCursor )
			return;

		int cursCol	= m_pText->column();

		WgTextAttr	attr;
		m_pText->GetBaseAttr( attr );

		int cursAdvance	= pCursor->Advance(m_pText->cursorMode() );
		int cursBearing	= pCursor->BearingX(m_pText->cursorMode() );
		int cursWidth	= pCursor->Width(m_pText->cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.

		int geoWidth = Size().w;
		if( m_pTextBoxBg )
			geoWidth -= m_pTextBoxBg->Padding().Width();

		int	lineWidth = m_pText->getSoftLineWidth( 0 ) + cursBearing+cursWidth;

		// Calculate cursOfs

		cursOfs	= m_pText->getSoftLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
		{
			maxOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol-1 );

			if( lineWidth < maxOfs + geoWidth )
				maxOfs = WgMax( lineWidth - geoWidth, 0 );
		}
		else
			maxOfs = cursOfs;


		// Calculate minOfs

		if( cursCol < m_pText->getLine(0)->nChars )
			minOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
		else
			minOfs = cursOfs + cursBearing + cursWidth - geoWidth;

		// Check boundaries and update

		if( m_viewOfs > maxOfs )
			m_viewOfs = maxOfs;

		if( m_viewOfs < minOfs )
			m_viewOfs = minOfs;


	}
	else
		m_viewOfs = 0;				// Show beginning of line when cursor disappears.
}

//____ _onAlphaTest() ______________________________________________________

bool WgCombobox::_onAlphaTest( const WgCoord& ofs )
{
	if( !m_pTextBoxBg )
		return false;

	WgSize sz = Size();

	return WgUtil::MarkTestBlock( ofs, m_pTextBoxBg->GetBlock(m_mode,sz), WgRect( WgCoord(0,0), sz ) );
}

//____ _onGotInputFocus() ______________________________________________

void WgCombobox::_onGotInputFocus()
{
	m_bFocused = true;

	if( _isEditable() )
	{
		m_text.showCursor();
		if( m_bResetCursorOnFocus )
		{
			m_text.goEOL();
			m_text.selectAll();
		}
	}
}

//____ _onLostInputFocus() ______________________________________________

void WgCombobox::_onLostInputFocus()
{
	m_bFocused = false;
	m_bResetCursorOnFocus = true;
	m_text.hideCursor();
	m_text.clearSelection();
}


//____ _onEnable() _____________________________________________________________

void WgCombobox::_onEnable( void )
{
	WgMode newMode;
	WgMode oldMode = m_mode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( m_pTextBoxBg && !m_pTextBoxBg->SameBlock(newMode, oldMode) )
		_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgCombobox::_onDisable( void )
{
	WgMode newMode;
	WgMode oldMode = m_mode;

	if( m_bEnabled )
		newMode = WG_MODE_NORMAL;
	else
		newMode = WG_MODE_DISABLED;

	m_mode = newMode;
	if( m_pTextBoxBg && !m_pTextBoxBg->SameBlock(newMode, oldMode) )
		_requestRender();
}


//____ _textModified() _________________________________________________________

void WgCombobox::_textModified()
{
	m_bResetCursorOnFocus = true;
	WgEventHandler * pHandler = _eventHandler();		
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );
	_requestRender();
//	_adjustViewOfs();
}

//____ _menuClosed() ___________________________________________________________

void WgCombobox::_menuClosed()
{
	m_mode = WG_MODE_NORMAL;
	_requestRender();
}


//____ _entrySelected() ________________________________________________________

void WgCombobox::_entrySelected(WgMenuItem * pItem)
{
	m_pSelectedItem = pItem;
	if(pItem && pItem->GetType() != SEPARATOR)
	{
		WgCharBuffer	buff;

		buff.PushBack(m_textFormat);

		int ofs = buff.FindFirst( "%1" );
		if( ofs >= 0 )
		{
			WgTextpropPtr pProp = buff[ofs].Properties();

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
		SetText( &buff );
		_adjustViewOfs();
	}
}