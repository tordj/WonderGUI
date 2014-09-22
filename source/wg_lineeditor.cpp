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

#include <wg_lineeditor.h>

#include	<wg_key.h>
#include	<wg_font.h>
#include 	<wg_gfxdevice.h>
#include 	<wg_pen.h>
#include 	<wg_eventhandler.h>

const char WgLineEditor::CLASSNAME[] = {"LineEditor"};

//____ Constructor ____________________________________________________________

WgLineEditor::WgLineEditor()
{
	text._setHolder( this );
	text.SetWrap(false);
	text.SetAutoEllipsis(IsAutoEllipsisDefault());
	text.SetEditMode( WG_TEXT_EDITABLE );
	m_bPasswordMode = false;
	m_pwGlyph		= '*';
	m_viewOfs		= 0;
	m_pointerStyle	= WG_POINTER_IBEAM;
	m_bResetCursorOnFocus = true;
}

//____ Destructor _____________________________________________________________

WgLineEditor::~WgLineEditor()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgLineEditor::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgLineEditor::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgLineEditorPtr WgLineEditor::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgLineEditorPtr( static_cast<WgLineEditor*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetEditMode() __________________________________________________________

void WgLineEditor::SetEditMode(WgTextEditMode mode)
{
	text.SetEditMode( mode );

	if( _isSelectable() )
	{
		m_pointerStyle = WG_POINTER_IBEAM;
	}
	else
	{
		m_pointerStyle = WG_POINTER_DEFAULT;
	}
}

//____ SetPasswordGlyph() _____________________________________________________

void WgLineEditor::SetPasswordGlyph( Uint16 glyph )
{
	assert(glyph);
	if(glyph)
	{
		m_pwGlyph = glyph;
	}
}

//____ InsertTextAtCursor() ___________________________________________________

int WgLineEditor::InsertTextAtCursor( const WgCharSeq& str )
{
	if( !_isEditable() )
		return 0;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return 0;				// Couldn't get input focus...

	int retVal = text.putText( str );

	_queueEvent( new WgTextEditEvent(text.Ptr(),false) );

	_adjustViewOfs();

	return retVal;
}

//____ InsertCharAtCursor() ___________________________________________________

bool WgLineEditor::InsertCharAtCursor( Uint16 c )
{
	if( !_isEditable() )
		return false;

	if( !m_state.IsFocused() )
		if( !GrabFocus() )
			return false;				// Couldn't get input focus...

	if( !text.putChar( c ) )
		return false;

	_queueEvent( new WgTextEditEvent(text.Ptr(),false) );

	_adjustViewOfs();
	return true;
}

//____ PreferredSize() __________________________________________________________

WgSize WgLineEditor::PreferredSize() const
{
	WgTextAttr attr;
	text.GetBaseAttr( attr );
	int width = WgTextTool::lineWidth( text.getNode(), attr, "MMMMMMMMMM" );		// Default line editor should fit 10 letter M in textfield
	WgSize contentSize( text.Height(), width );
	
	if( m_pSkin )
		return m_pSkin->SizeForContent( contentSize );
	else
		return contentSize;
}

//____ _onCloneContent() _______________________________________________________

void WgLineEditor::_onCloneContent( const WgWidget * _pOrg )
{
	WgLineEditor * pOrg = (WgLineEditor*) _pOrg;

	text			= pOrg->text;
	m_bPasswordMode = pOrg->m_bPasswordMode;
	m_pwGlyph		= pOrg->m_pwGlyph;
}

//____ _onRender() _____________________________________________________________

void WgLineEditor::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgTextField * pText = &text;
	if( m_bPasswordMode )
	{
		int nChars = text.Length();
		Uint16 * pContent = new Uint16[nChars+1];
		for( int i = 0 ; i < nChars ; i++ )
			pContent[i] = m_pwGlyph;
		pContent[nChars] = 0;
		pText = new WgTextField( pContent );
		delete [] pContent;

		pText->SetWrap(false);
		pText->SetAutoEllipsis(false);
		pText->SetAlignment(text.Alignment());
		pText->SetProperties(text.Properties());
		pText->SetSelectionProperties(text.SelectionProperties());
		pText->setState(text.State());

		pText->SetEditMode(text.EditMode());
		pText->showCursor();
		pText->gotoSoftPos( text.line(), text.column() );
		pText->incTime( text.time() );

		int sl, sc, el, ec;
		if( text.getSelection(sl, sc, el, ec) )
			pText->selectText(sl, sc, el, ec);

	}

	WgRect canvas;
	if( m_pSkin )
		canvas = m_pSkin->SizeForContent(_canvas);
	else
		canvas = _canvas;

	WgRect	textClip( _clip, canvas );

	canvas.x -= m_viewOfs;
	canvas.w += m_viewOfs;

	if( m_state.IsFocused() && _isEditable() )
		pText->showCursor();
	else
		pText->hideCursor();

	pDevice->PrintText( textClip, pText, canvas );

	if( pText != &text )
		delete pText;
}

//____ _onEvent() ______________________________________________________________

void WgLineEditor::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	WgEventType event = pEvent->Type();

	if( event == WG_EVENT_TICK )
	{
		if( _isSelectable() && m_state.IsFocused() )
		{
			text.incTime( WgTickEvent::Cast(pEvent)->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}

	if( (event == WG_EVENT_MOUSE_PRESS || event == WG_EVENT_MOUSE_DRAG) && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
	{
		if( !m_state.IsFocused() )
			GrabFocus();

		if( m_state.IsFocused() )
		{
			if( _isSelectable() && (pEvent->ModKeys() & WG_MODKEY_SHIFT) )
			{
				text.setSelectionMode(true);
			}

			WgCoord ofs = pEvent->PointerPos();
			int x = ofs.x + m_viewOfs;
			int y = 0;

			if( m_bPasswordMode )
			{
				WgTextAttr	attr;
				text.GetBaseAttr( attr );

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
				text.gotoSoftPos(line,col);
			}
			else
			{
				text.CursorGotoCoord( WgCoord(x, 0), WgRect(0,0,1000000,1000000) );
			}

			if(_isSelectable() && event == WG_EVENT_MOUSE_PRESS && !(pEvent->ModKeys() & WG_MODKEY_SHIFT))
			{
				text.ClearSelection();
				text.setSelectionMode(true);
			}
		}
		_adjustViewOfs();
	}

	if( event == WG_EVENT_MOUSE_RELEASE )
	{
		if( m_state.IsFocused() && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
			text.setSelectionMode(false);
	}		

	if( event == WG_EVENT_CHARACTER )
	{
		int ch = WgCharacterEvent::Cast(pEvent)->Char();

		if( _isEditable() && m_state.IsFocused() && ch >= 32 && ch != 127)
		{

			if(text.hasSelection())
				text.delSelection();
			text.setSelectionMode(false);

			if( text.putChar( ch ) )
			{
				if( pHandler )
					pHandler->QueueEvent( new WgTextEditEvent(text.Ptr(),false) );

				_adjustViewOfs();
			}
		}
	}

	if( event == WG_EVENT_KEY_RELEASE && m_state.IsFocused() )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					text.setSelectionMode(false);
			break;
		}
	}

	if( (event == WG_EVENT_KEY_PRESS || event == WG_EVENT_KEY_REPEAT) && _isEditable() && m_state.IsFocused() )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_LEFT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						text.GoBOL();
					else
						text.gotoPrevWord();
				}
				else
				{
					text.goLeft();
				}
				break;
			case WG_KEY_RIGHT:
				if( pEvent->ModKeys() & WG_MODKEY_SHIFT )
					text.setSelectionMode(true);

				if( pEvent->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						text.GoEOL();
					else
						text.gotoNextWord();
				}
				else
				{
					text.goRight();
				}
				break;

			case WG_KEY_BACKSPACE:
			{
				if(text.hasSelection())
					text.delSelection();
				else if( (pEvent->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					text.delPrevWord();
				else
					text.delPrevChar();

				if( pHandler )
					pHandler->QueueEvent( new WgTextEditEvent(text.Ptr(),false) );
				break;
			}

			case WG_KEY_DELETE:
			{
				if(text.hasSelection())
					text.delSelection();
				else if( (pEvent->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					text.delNextWord();
				else
					text.delNextChar();

				if( pHandler )
					pHandler->QueueEvent( new WgTextEditEvent(text.Ptr(),false) );
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
						text.setSelectionMode(true);

					text.GoBOL();
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
						text.setSelectionMode(true);

					text.GoEOL();
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


//____ _adjustViewOfs() ________________________________________________________

void WgLineEditor::_adjustViewOfs()
{
	// Possibly move viewOfs so that:
	//	1 Cursor remains inside view.
	//  2 At least one character is displayed before the cursor
	//  3 At least one character is displayed after the cursor (if there is one).

	if( m_state.IsFocused() && text.Font() )
	{
		WgCaretPtr pCursor = WgTextTool::GetCursor( &text );
		if( !pCursor )
			return;

		int cursCol	= text.column();

		WgTextAttr	attr;
		text.GetBaseAttr( attr );

		WgPen	pen;
		pen.SetAttributes( attr );
		pen.SetChar(m_pwGlyph);
		pen.AdvancePos();

		int pwAdvance	= pen.GetPosX();
		int cursAdvance	= pCursor->Advance(text.cursorMode() );
		int cursBearing	= pCursor->BearingX(text.cursorMode() );
		int cursWidth	= pCursor->Width(text.cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.

		int geoWidth = Size().w;
		if( m_pSkin )
			geoWidth -= m_pSkin->ContentPadding().w;
		int	lineWidth = text.getSoftLineWidth( 0 ) + cursBearing+cursWidth;

		// Calculate cursOfs

		if( m_bPasswordMode )
			cursOfs = cursCol * pwAdvance;
		else
			cursOfs	= text.getSoftLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
		{
			if( m_bPasswordMode )
				maxOfs = (cursCol-1) * pwAdvance;
			else
				maxOfs = text.getSoftLineWidthPart( 0, 0, cursCol-1 );

			if( lineWidth < maxOfs + geoWidth )
				maxOfs = WgMax( lineWidth - geoWidth, 0 );
		}
		else
			maxOfs = cursOfs;


		// Calculate minOfs

		if( cursCol < text.getLine(0)->nChars )
		{
			if( m_bPasswordMode )
				minOfs = (cursCol+1) * pwAdvance + cursAdvance - geoWidth;
			else
				minOfs = text.getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
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

//____ _onStateChanged() ______________________________________________________

void WgLineEditor::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);
	
	text.setState(m_state);
	_requestRender();				//TODO: Only request render if text appearance has changed.

	if( m_state.IsFocused() && !oldState.IsFocused() )
	{
		if( _isEditable() )
		{
			_startReceiveTicks();
			if( m_bResetCursorOnFocus )
				text.GoEOL();
			_requestRender(); // render with cursor on
		}
	}

	if( !m_state.IsFocused() && oldState.IsFocused() )
	{
		if( _isSelectable() )
		{
			text.ClearSelection();
			text.setSelectionMode(false);
			m_bResetCursorOnFocus = false;
		}

		if( _isEditable() || m_viewOfs != 0 )
		{
			_stopReceiveTicks();
			_queueEvent( new WgTextEditEvent(text.Ptr(),true) );

			m_viewOfs = 0;
			_requestRender();
		}
	}
}

//____ _onSkinChanged() _______________________________________________________

void WgLineEditor::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	text.SetColorSkin(pNewSkin);
}


//____ _onNewSize() ____________________________________________________________

void WgLineEditor::_onNewSize( const WgSize& size )
{
	_adjustViewOfs();
	_requestRender();
}


//____ _fieldModified() _________________________________________________________

void WgLineEditor::_fieldModified( WgTextField * pField )
{
	m_bResetCursorOnFocus = true;			// Any change to text while we don't have focus resets the position.
	_requestRender();
	_adjustViewOfs();
}

