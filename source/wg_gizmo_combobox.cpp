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
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"ComboBox"};


//____ WgGizmoCombobox() _________________________________________________________________

WgGizmoCombobox::WgGizmoCombobox( void )
{
	m_pText = &m_text;
	m_text.setAlignment( WgOrigo::midLeft() );
	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());	
//	m_textColor = m_text.color();
//	m_textDisabledColor	= 0xFFFF;

	m_mode = WG_MODE_NORMAL;

	m_editMode = WG_TEXT_STATIC;
	m_bResetCursorOnFocus = true;
	m_bPressInInputRect = false;
	m_pMenu		= 0;
	m_pSelectedItem = 0;
	m_textFormat = "%1";
	m_viewOfs		= 0;
	m_maxCharacters = 0;
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

//____ DefaultSize() __________________________________________________________

WgSize WgGizmoCombobox::DefaultSize() const
{
	//TODO: Implement!!!

	return WgSize(1,1);
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

//____ SetEditMode() __________________________________________________________

void WgGizmoCombobox::SetEditMode(WgTextEditMode mode)
{
	m_editMode = mode;

	if( _isEditable() )
	{
		m_text.CreateCursor();
		m_text.hideCursor();
	}
	else
		m_text.DestroyCursor();

}

//____ SetTextFormat() ________________________________________________________

void WgGizmoCombobox::SetTextFormat( const WgCharSeq& str )
{
	m_textFormat = str;
	if( m_pMenu )
		EntrySelected(m_pMenu->GetSelectedItem());
}

//____ SetPlaceholderText() ___________________________________________________

void WgGizmoCombobox::SetPlaceholderText( const WgCharSeq& str )
{
	m_placeholderText = str;
	if( m_text.IsEmpty() && !m_text.isCursorShowing() )
		RequestRender();
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoCombobox::InsertTextAtCursor( const WgCharSeq& str )
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

	WgEventHandler * pHandler = EventHandler();		
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );

	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	_adjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgGizmoCombobox::InsertCharAtCursor( Uint16 c )
{
	if( !_isEditable() )
		return 0;

	if( !m_bFocused )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( m_maxCharacters != 0 && m_maxCharacters < m_pText->nbChars() )
		return false;

	m_pText->putChar( c );

	WgEventHandler * pHandler = EventHandler();		
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );

	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	_adjustViewOfs();
	return true;
}

//____ goBOL() ________________________________________________________________

void WgGizmoCombobox::goBOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________

void WgGizmoCombobox::goEOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goEOL();
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
								m_pText->setSelectionMode(true);

							m_pText->goBOL();
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
		RequestRender();
	}

}


//____ _onUpdate() _____________________________________________________________

void WgGizmoCombobox::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( _isEditable() && m_bFocused )
	{
		m_text.incTime( _updateInfo.msDiff );
		RequestRender();					//TODO: Should only render the cursor and selection!
	}
}

//____ _onRender() ________________________________________________________

void WgGizmoCombobox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
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
		r.Shrink( m_pTextBoxBg->ContentBorders() );

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

void WgGizmoCombobox::_onRefresh( void )
{
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoCombobox::_onCloneContent( const WgGizmo * _pOrg )
{
}



//____ _adjustViewOfs() ________________________________________________________

void WgGizmoCombobox::_adjustViewOfs()
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

		int cursAdvance	= pCursor->advance(m_pText->cursorMode() );
		int cursBearing	= pCursor->bearingX(m_pText->cursorMode() );
		int cursWidth	= pCursor->width(m_pText->cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.

		int geoWidth = Size().w;
		if( m_pTextBoxBg )
			geoWidth -= m_pTextBoxBg->ContentBorders().Width();

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

bool WgGizmoCombobox::_onAlphaTest( const WgCoord& ofs )
{
	if( !m_pTextBoxBg )
		return false;

	WgSize sz = Size();

	return WgUtil::MarkTestBlock( ofs, m_pTextBoxBg->GetBlock(m_mode,sz), WgRect( WgCoord(0,0), sz ) );
}

//____ _onGotInputFocus() ______________________________________________

void WgGizmoCombobox::_onGotInputFocus()
{
	m_bFocused = true;

	if( _isEditable() )
	{
		m_text.showCursor();
		if( m_bResetCursorOnFocus )
		{
			m_text.GetCursor()->goEOL();
			m_text.selectAll();
		}
	}
}

//____ _onLostInputFocus() ______________________________________________

void WgGizmoCombobox::_onLostInputFocus()
{
	m_bFocused = false;
	m_bResetCursorOnFocus = true;
	m_text.hideCursor();
	m_text.clearSelection();
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


//____ _textModified() _________________________________________________________

void WgGizmoCombobox::_textModified()
{
	m_bResetCursorOnFocus = true;
	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	RequestRender();
//	_adjustViewOfs();
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
		SetText( &buff );
		_adjustViewOfs();
	}
}
