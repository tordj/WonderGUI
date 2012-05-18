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

#include <wg_gizmo_editline.h>

#include	<wg_key.h>
#include	<wg_font.h>
#include 	<wg_gfxdevice.h>
#include 	<wg_pen.h>
#ifdef WG_TNG
#	include 	<wg_eventhandler.h>
#endif

static const char	c_gizmoType[] = {"Editline"};

//____ Constructor ____________________________________________________________

WgGizmoEditline::WgGizmoEditline()
{
	m_pText			= &m_text;
	m_text.setHolder( this );
	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());
	m_text.SetEditMode( WG_TEXT_EDITABLE );
	m_bPasswordMode = false;
	m_pwGlyph		= '*';
	m_viewOfs		= 0;
	m_pointerStyle	= WG_POINTER_IBEAM;
	m_bResetCursorOnFocus = true;
}

//____ Destructor _____________________________________________________________

WgGizmoEditline::~WgGizmoEditline()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoEditline::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoEditline::GetMyType()
{
	return c_gizmoType;
}

//____ SetEditMode() __________________________________________________________

void WgGizmoEditline::SetEditMode(WgTextEditMode mode)
{
	m_text.SetEditMode( mode );

	if( _isSelectable() )
	{
		m_pointerStyle = WG_POINTER_IBEAM;
	}
	else
	{
		m_pointerStyle = WG_POINTER_DEFAULT;
	}
}

//____ goBOL() ________________________________________________________________

void WgGizmoEditline::goBOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goBOL();
}

//____ goEOL() ________________________________________________________________

void WgGizmoEditline::goEOL()
{
	if( _isEditable() && m_bFocused )
		m_pText->goEOL();
}

//____ SetPasswordGlyph() _____________________________________________________

void WgGizmoEditline::SetPasswordGlyph( Uint16 glyph )
{
	assert(glyph);
	if(glyph)
	{
		m_pwGlyph = glyph;
	}
}

//____ InsertTextAtCursor() ___________________________________________________

Uint32 WgGizmoEditline::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !_isEditable() )
		return 0;

	if( !m_bFocused )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	Uint32 retVal = m_pText->putText( str );

#ifdef WG_TNG
	WgEventHandler * pHandler = EventHandler();
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );
#endif

	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	_adjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgGizmoEditline::InsertCharAtCursor( Uint16 c )
{
	if( !_isEditable() )
		return 0;

	if( !m_bFocused )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( !m_pText->putChar( c ) )
		return false;
#ifdef WG_TNG
	WgEventHandler * pHandler = EventHandler();
	if( pHandler )
		pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );
#endif

	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	_adjustViewOfs();
	return true;
}


//____ SetTextWrap() __________________________________________________________

bool WgGizmoEditline::SetTextWrap(bool bWrap)
{
	return !bWrap;
}

//____ DefaultSize() __________________________________________________________

WgSize WgGizmoEditline::DefaultSize() const
{
	//TODO: Implement!

	return WgSize(1,1);
}


//____ _onUpdate() _____________________________________________________________

void WgGizmoEditline::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( _isSelectable() && m_bFocused )
	{
		m_pText->incTime( _updateInfo.msDiff );
		_requestRender();					//TODO: Should only render the cursor and selection!
	}
}



//____ _onCloneContent() _______________________________________________________

void WgGizmoEditline::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoEditline * pOrg = (WgGizmoEditline*) _pOrg;

	Wg_Interface_TextHolder::_onCloneContent( pOrg );

	//TODO: Implement!
}

//____ _onRender() _____________________________________________________________

void WgGizmoEditline::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{

	WgText * pText = &m_text;
	if( m_bPasswordMode )
	{
		int nChars = m_text.nbChars();
		Uint16 * pContent = new Uint16[nChars+1];
		for( int i = 0 ; i < nChars ; i++ )
			pContent[i] = m_pwGlyph;
		pContent[nChars] = 0;
		pText = new WgText( pContent );
		delete [] pContent;

		pText->SetWrap(false);
		pText->SetAutoEllipsis(false);
		pText->setAlignment(m_text.alignment());
		pText->setProperties(m_text.getProperties());
		pText->setSelectionProperties(m_text.getSelectionProperties());
		pText->setMode(m_text.mode());

		pText->SetEditMode(m_text.GetEditMode());
		pText->showCursor();
		pText->gotoSoftPos( m_text.line(), m_text.column() );
		pText->incTime( m_text.time() );

		int sl, sc, el, ec;
		if( m_text.getSelection(sl, sc, el, ec) )
			pText->selectText(sl, sc, el, ec);

	}

	WgRect r = _canvas;
	r.x -= m_viewOfs;
	r.w += m_viewOfs;

	if( m_bFocused && _isEditable() )
		pText->showCursor();
	else
		pText->hideCursor();

	pDevice->PrintText( _clip, pText, r );

	if( pText != &m_text )
		delete pText;
}

//____ _onEvent() ______________________________________________________________

#ifdef WG_TNG
void WgGizmoEditline::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	WgEventType event = pEvent->Type();

	if( event == WG_EVENT_TICK )
	{
		if( _isSelectable() && m_bFocused )
		{
			m_pText->incTime( static_cast<const WgEvent::Tick*>(pEvent)->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}

	if( (event == WG_EVENT_MOUSEBUTTON_PRESS || event == WG_EVENT_MOUSEBUTTON_DRAG) && static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() == 1 )
	{
		if( !m_bFocused )
			GrabFocus();

		if( m_bFocused )
		{
			if( _isSelectable() && (pEvent->ModKeys() & WG_MODKEY_SHIFT) )
			{
				m_pText->setSelectionMode(true);
			}

			WgCoord ofs = pEvent->PointerPos();
			int x = ofs.x + m_viewOfs;
			int y = 0;

			if( m_bPasswordMode )
			{
				WgTextAttr	attr;
				m_pText->GetBaseAttr( attr );

				WgPen	pen;
				pen.SetAttributes( attr );
				pen.SetChar(m_pwGlyph);
				pen.AdvancePos();

				int spacing = pen.GetPosX();
				int height = pen.GetLineSpacing();

				int line = y/height;
				int col = (x+spacing/2)/spacing;
				if(col < 0)
				{
					col = 0;
					line = 0;
				}
				m_pText->gotoSoftPos(line,col);
			}
			else
			{
				m_pText->CursorGotoCoord( WgCoord(x, 0), WgRect(0,0,1000000,1000000) );
			}

			if(_isSelectable() && event == WG_EVENT_MOUSEBUTTON_PRESS && !(pEvent->ModKeys() & WG_MODKEY_SHIFT))
			{
				m_pText->clearSelection();
				m_pText->setSelectionMode(true);
			}
		}
		_adjustViewOfs();
	}

	if( event == WG_EVENT_MOUSEBUTTON_RELEASE )
	{
		if( m_bFocused && static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() == 1 )
			m_pText->setSelectionMode(false);
	}		

	if( event == WG_EVENT_CHARACTER )
	{
		int ch = static_cast<const WgEvent::Character*>(pEvent)->Char();

		if( _isEditable() && m_bFocused && ch >= 32 && ch != 127)
		{

			if(m_pText->hasSelection())
				m_pText->delSelection();
			m_pText->setSelectionMode(false);

			if( m_pText->putChar( ch ) )
			{
				WgEventHandler * pHandler = EventHandler();
				if( pHandler )
					pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );

				Emit( WgSignal::TextChanged() );
				_adjustViewOfs();
			}
		}
	}

	if( event == WG_EVENT_KEY_RELEASE && m_bFocused )
	{
		int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (event == WG_EVENT_KEY_PRESS || event == WG_EVENT_KEY_REPEAT) && _isEditable() && m_bFocused )
	{
		int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_LEFT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_pText->goBOL();
					else
						m_pText->gotoPrevWord();
				}
				else
				{
					m_pText->goLeft();
				}
				break;
			case WG_KEY_RIGHT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_pText->goEOL();
					else
						m_pText->gotoNextWord();
				}
				else
				{
					m_pText->goRight();
				}
				break;

			case WG_KEY_BACKSPACE:
			{
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (pEvent->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();

				WgEventHandler * pHandler = EventHandler();
				if( pHandler )
					pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );
				Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
				break;
			}

			case WG_KEY_DELETE:
			{
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (pEvent->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_pText->delNextWord();
				else
					m_pText->delNextChar();

				WgEventHandler * pHandler = EventHandler();
				if( pHandler )
					pHandler->QueueEvent( new WgEvent::TextModify(this,m_pText) );
				Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
				break;
			}

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
					if(pEvent->ModKeys() & WG_MODKEY_SHIFT )
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
				switch( pEvent->ModKeys() )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
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

	// Forward event depending on rules.

	if( pEvent->IsMouseButtonEvent() )
	{
		if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
			pHandler->ForwardEvent( pEvent );
	}
	else if( pEvent->IsKeyEvent() )
	{
		int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
		if( static_cast<const WgEvent::KeyEvent*>(pEvent)->IsMovementKey() == false &&
			key != WG_KEY_DELETE && key != WG_KEY_BACKSPACE )
				pHandler->ForwardEvent( pEvent );
		
		//TODO: Would be good if we didn't forward any character-creating keys either...
	}
	else if( event != WG_EVENT_CHARACTER )
		pHandler->ForwardEvent( pEvent );
}
#endif

//____ _onAction() _____________________________________________________________

void WgGizmoEditline::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
#ifdef WG_LEGACY

	if( (action == WgInput::BUTTON_PRESS || action == WgInput::BUTTON_DOWN) && button_key == 1 )
	{
		if( !m_bFocused )
			GrabFocus();

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

			if( m_bPasswordMode )
			{
				WgTextAttr	attr;
				m_pText->GetBaseAttr( attr );

				WgPen	pen;
				pen.SetAttributes( attr );
				pen.SetChar(m_pwGlyph);
				pen.AdvancePos();

				int spacing = pen.GetPosX();
				int height = pen.GetLineSpacing();

				int line = y/height;
				int col = (x+spacing/2)/spacing;
				if(col < 0)
				{
					col = 0;
					line = 0;
				}
				m_pText->gotoSoftPos(line,col);
			}
			else
			{
				m_pText->CursorGotoCoord( WgCoord(x, 0), WgRect(0,0,1000000,1000000) );
			}

			if(_isSelectable() && action == WgInput::BUTTON_PRESS && !(info.modifier & WG_MODKEY_SHIFT))
			{
				m_pText->clearSelection();
				m_pText->setSelectionMode(true);
			}
		}
		_adjustViewOfs();
	}

	if( action == WgInput::BUTTON_RELEASE || action == WgInput::BUTTON_RELEASE_OUTSIDE )
	{
		if( m_bFocused && button_key == 1 )
			m_pText->setSelectionMode(false);
	}

	if( action == WgInput::CHARACTER )
	{
		if( _isEditable() &&  m_bFocused && button_key >= 32 && button_key != 127)
		{

			if(m_pText->hasSelection())
				m_pText->delSelection();
			m_pText->setSelectionMode(false);

			if( m_pText->putChar( button_key ) )
			{
				Emit( WgSignal::TextChanged() );
				_adjustViewOfs();
			}
		}
	}

	if( action == WgInput::KEY_RELEASE && m_bFocused )
	{
		switch( button_key )
		{
			case WG_KEY_SHIFT:
				if(!inputObj.isButtonDown(1))
					m_pText->setSelectionMode(false);
			break;
		}
	}

	if( (action == WgInput::KEY_PRESS || action == WgInput::KEY_REPEAT) && _isEditable() && m_bFocused )
	{
		switch( button_key )
		{
			case WG_KEY_LEFT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_pText->goBOL();
					else
						m_pText->gotoPrevWord();
				}
				else
				{
					m_pText->goLeft();
				}
				break;
			case WG_KEY_RIGHT:
				if( info.modifier & WG_MODKEY_SHIFT )
					m_pText->setSelectionMode(true);

				if( info.modifier & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_pText->goEOL();
					else
						m_pText->gotoNextWord();
				}
				else
				{
					m_pText->goRight();
				}
				break;

			case WG_KEY_BACKSPACE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (info.modifier & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_pText->delPrevWord();
				else
					m_pText->delPrevChar();
				Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
				break;

			case WG_KEY_DELETE:
				if(m_pText->hasSelection())
					m_pText->delSelection();
				else if( (info.modifier & WG_MODKEY_CTRL) && !m_bPasswordMode)
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

#endif //WG_LEGACY

}


//____ _adjustViewOfs() ________________________________________________________

void WgGizmoEditline::_adjustViewOfs()
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

		WgPen	pen;
		pen.SetAttributes( attr );
		pen.SetChar(m_pwGlyph);
		pen.AdvancePos();

		int pwAdvance	= pen.GetPosX();
		int cursAdvance	= pCursor->Advance(m_pText->cursorMode() );
		int cursBearing	= pCursor->BearingX(m_pText->cursorMode() );
		int cursWidth	= pCursor->Width(m_pText->cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.

		int geoWidth = Size().w;
		int	lineWidth = m_pText->getSoftLineWidth( 0 ) + cursBearing+cursWidth;

		// Calculate cursOfs

		if( m_bPasswordMode )
			cursOfs = cursCol * pwAdvance;
		else
			cursOfs	= m_pText->getSoftLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
		{
			if( m_bPasswordMode )
				maxOfs = (cursCol-1) * pwAdvance;
			else
				maxOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol-1 );

			if( lineWidth < maxOfs + geoWidth )
				maxOfs = WgMax( lineWidth - geoWidth, 0 );
		}
		else
			maxOfs = cursOfs;


		// Calculate minOfs

		if( cursCol < m_pText->getLine(0)->nChars )
		{
			if( m_bPasswordMode )
				minOfs = (cursCol+1) * pwAdvance + cursAdvance - geoWidth;
			else
				minOfs = m_pText->getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
		}
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



//____ _onEnable() _____________________________________________________________

void WgGizmoEditline::_onEnable()
{
	m_text.setMode(WG_MODE_NORMAL);
	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoEditline::_onDisable()
{
	m_text.setMode(WG_MODE_DISABLED);
	_requestRender();
}

//____ _onGotInputFocus() ______________________________________________________

void WgGizmoEditline::_onGotInputFocus()
{
	m_bFocused = true;

	if( _isEditable() )
	{
#ifdef WG_TNG
		_startReceiveTicks();
#endif
		if( m_bResetCursorOnFocus )
			m_pText->goEOL();
		_requestRender(); // render with cursor on
	}
}

//____ _onLostInputFocus() _____________________________________________________

void WgGizmoEditline::_onLostInputFocus()
{
	m_bFocused = false;

	if( _isSelectable() )
	{
		m_pText->clearSelection();
		m_pText->setSelectionMode(false);
		m_bResetCursorOnFocus = false;
	}

	if( _isEditable() || m_viewOfs != 0 )
	{
#ifdef WG_TNG
		_stopReceiveTicks();
		WgEventHandler * pHandler = EventHandler();
		if( pHandler )
			pHandler->QueueEvent( new WgEvent::TextSet(this, m_pText) );
#endif

		m_viewOfs = 0;
		_requestRender();
	}
}

//____ _onNewSize() ____________________________________________________________

void WgGizmoEditline::_onNewSize( const WgSize& size )
{
	_adjustViewOfs();
	_requestRender();
}


//____ _textModified() _________________________________________________________

void WgGizmoEditline::_textModified()
{
	m_bResetCursorOnFocus = true;			// Any change to text while we don't have focus resets the position.
	Emit( WgSignal::TextChanged() );		//TODO: Should only emit if text really has changed
	_requestRender();
	_adjustViewOfs();
}

