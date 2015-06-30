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
#include 	<wg_msgrouter.h>
#include	<wg_base.h>



const char WgLineEditor::CLASSNAME[] = {"LineEditor"};

//____ Constructor ____________________________________________________________

WgLineEditor::WgLineEditor() : m_text(this), text(&m_text)
{
	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());
	m_text.SetEditMode( WG_TEXT_EDITABLE );
	m_bPasswordMode = false;
	m_pwGlyph		= '*';
	m_viewOfs		= 0;
	m_pointerStyle	= WG_POINTER_IBEAM;
	m_bResetCursorOnFocus = true;
	m_tickRouteId = 0;
}

//____ Destructor _____________________________________________________________

WgLineEditor::~WgLineEditor()
{
	if( m_tickRouteId )
		WgBase::MsgRouter()->DeleteRoute( m_tickRouteId );
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
		return WgLineEditorPtr( static_cast<WgLineEditor*>(pObject.RawPtr()) );

	return 0;
}

//____ SetEditMode() __________________________________________________________

void WgLineEditor::SetEditMode(WgTextEditMode mode)
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

	int retVal = m_text.putText( str );

	WgBase::MsgRouter()->Post( new WgTextEditMsg(text.Ptr(),false) );

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

	if( !m_text.putChar( c ) )
		return false;

	WgBase::MsgRouter()->Post( new WgTextEditMsg(text.Ptr(),false) );

	_adjustViewOfs();
	return true;
}

//____ PreferredSize() __________________________________________________________

WgSize WgLineEditor::PreferredSize() const
{
	WgTextAttr attr;
	m_text.GetBaseAttr( attr );
	int width = WgTextTool::lineWidth( attr, "MMMMMMMMMM" );		// Default line editor should fit 10 letter M in textfield
	WgSize contentSize( m_text.Height(), width );
	
	if( m_pSkin )
		return m_pSkin->SizeForContent( contentSize );
	else
		return contentSize;
}

//____ _onCloneContent() _______________________________________________________

void WgLineEditor::_onCloneContent( const WgWidget * _pOrg )
{
	WgLineEditor * pOrg = (WgLineEditor*) _pOrg;

	m_text			= pOrg->m_text;
	m_bPasswordMode = pOrg->m_bPasswordMode;
	m_pwGlyph		= pOrg->m_pwGlyph;
}

//____ _onRender() _____________________________________________________________

void WgLineEditor::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgLegacyTextField * pText = &m_text;

//TODO: Get password mode working again!
/*
	if( m_bPasswordMode )
	{
		int nChars = m_text.Length();
		Uint16 * pContent = new Uint16[nChars+1];
		for( int i = 0 ; i < nChars ; i++ )
			pContent[i] = m_pwGlyph;
		pContent[nChars] = 0;
		pText = new WgLegacyTextField( pContent );
		delete [] pContent;

		pText->SetWrap(false);
		pText->SetAutoEllipsis(false);
		pText->SetAlignment(m_text.Alignment());
		pText->SetProperties(m_text.Properties());
		pText->SetSelectionProperties(m_text.SelectionProperties());
		pText->setState(m_text.State());

		pText->SetEditMode(m_text.EditMode());
		pText->showCursor();
		pText->gotoSoftPos( m_text.line(), m_text.column() );
		pText->incTime( m_text.time() );

		int sl, sc, el, ec;
		if( m_text.getSelection(sl, sc, el, ec) )
			pText->selectText(sl, sc, el, ec);
	}
*/

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

	if( pText != &m_text )
		delete pText;
}

//____ _onMsg() ______________________________________________________________

void WgLineEditor::_onMsg( const WgMsgPtr& pMsg )
{
	WgWidget::_onMsg(pMsg);

	WgMsgRouterPtr	pHandler = WgBase::MsgRouter();
	WgMsgType event = pMsg->Type();

	if( event == WG_MSG_TICK )
	{
		if( _isSelectable() && m_state.IsFocused() )
		{
			m_text.incTime( WgTickMsg::Cast(pMsg)->Millisec() );
			_requestRender();					//TODO: Should only render the cursor and selection!
		}
		return;
	}

	if( (event == WG_MSG_MOUSE_PRESS || event == WG_MSG_MOUSE_DRAG) && WgMouseButtonMsg::Cast(pMsg)->Button() == WG_BUTTON_LEFT )
	{
		if( !m_state.IsFocused() )
			GrabFocus();

		if( m_state.IsFocused() )
		{
			if( _isSelectable() && (pMsg->ModKeys() & WG_MODKEY_SHIFT) )
			{
				m_text.setSelectionMode(true);
			}

			WgCoord ofs = pMsg->PointerPos() - GlobalPos();
			int x = ofs.x + m_viewOfs;
			int y = 0;

			if( m_bPasswordMode )
			{
				WgTextAttr	attr;
				m_text.GetBaseAttr( attr );

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
				m_text.gotoSoftPos(line,col);
			}
			else
			{
				m_text.CursorGotoCoord( WgCoord(x, 0), WgRect(0,0,1000000,1000000) );
			}

			if(_isSelectable() && event == WG_MSG_MOUSE_PRESS && !(pMsg->ModKeys() & WG_MODKEY_SHIFT))
			{
				m_text.ClearSelection();
				m_text.setSelectionMode(true);
			}
		}
		_adjustViewOfs();
	}

	if( event == WG_MSG_MOUSE_RELEASE )
	{
		if( m_state.IsFocused() && WgMouseButtonMsg::Cast(pMsg)->Button() == WG_BUTTON_LEFT )
			m_text.setSelectionMode(false);
	}		

	if( event == WG_MSG_CHARACTER )
	{
		int ch = WgCharacterMsg::Cast(pMsg)->Char();

		if( _isEditable() && m_state.IsFocused() && ch >= 32 && ch != 127)
		{

			if(m_text.hasSelection())
				m_text.delSelection();
			m_text.setSelectionMode(false);

			if( m_text.putChar( ch ) )
			{
				if( pHandler )
					pHandler->Post( new WgTextEditMsg(text.Ptr(),false) );

				_adjustViewOfs();
			}
		}
	}

	if( event == WG_MSG_KEY_RELEASE && m_state.IsFocused() )
	{
		int key = WgKeyMsg::Cast(pMsg)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_SHIFT:
				if(!pHandler->IsMouseButtonPressed(1))
					m_text.setSelectionMode(false);
			break;
		}
	}

	if( (event == WG_MSG_KEY_PRESS || event == WG_MSG_KEY_REPEAT) && _isEditable() && m_state.IsFocused() )
	{
		int key = WgKeyMsg::Cast(pMsg)->TranslatedKeyCode();
		switch( key )
		{
			case WG_KEY_LEFT:
				if( pMsg->ModKeys() & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( pMsg->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_text.GoBOL();
					else
						m_text.gotoPrevWord();
				}
				else
				{
					m_text.goLeft();
				}
				break;
			case WG_KEY_RIGHT:
				if( pMsg->ModKeys() & WG_MODKEY_SHIFT )
					m_text.setSelectionMode(true);

				if( pMsg->ModKeys() & WG_MODKEY_CTRL )
				{
					if( m_bPasswordMode )
						m_text.GoEOL();
					else
						m_text.gotoNextWord();
				}
				else
				{
					m_text.goRight();
				}
				break;

			case WG_KEY_BACKSPACE:
			{
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( (pMsg->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_text.delPrevWord();
				else
					m_text.delPrevChar();

				if( pHandler )
					pHandler->Post( new WgTextEditMsg(text.Ptr(),false) );
				break;
			}

			case WG_KEY_DELETE:
			{
				if(m_text.hasSelection())
					m_text.delSelection();
				else if( (pMsg->ModKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
					m_text.delNextWord();
				else
					m_text.delNextChar();

				if( pHandler )
					pHandler->Post( new WgTextEditMsg(text.Ptr(),false) );
				break;
			}

			case WG_KEY_HOME:

				/*
				 *	I am not sure if this is the proper way to this, but in my opinion, the default
				 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( pMsg->ModKeys() )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if(pMsg->ModKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.GoBOL();
					break;
				}

				break;

			case WG_KEY_END:

				/*
			 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
		 		 *	"actions" has to be separated from any modifier key action combination
				 */
				switch( pMsg->ModKeys() )
				{

				case WG_MODKEY_CTRL:
					break;

				default: // no modifier key was pressed
					if( pMsg->ModKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);

					m_text.GoEOL();
					break;
				}

				break;

			default:
				break;
		}
		_adjustViewOfs();
	}

	// Swallow message depending on rules.

	if( pMsg->IsMouseButtonMsg() )
	{
		if( WgMouseButtonMsg::Cast(pMsg)->Button() == WG_BUTTON_LEFT )
			pMsg->Swallow();
	}
	else if( pMsg->IsKeyMsg() )
	{
		int key = WgKeyMsg::Cast(pMsg)->TranslatedKeyCode();
		if( WgKeyMsg::Cast(pMsg)->IsMovementKey() == true ||
			key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE )
				pMsg->Swallow();
		
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

	if( m_state.IsFocused() && m_text.Properties() && m_text.Properties()->Font() )
	{
		WgCaretPtr pCursor = WgTextTool::GetCursor( &m_text );
		if( !pCursor )
			return;

		int cursCol	= m_text.column();

		WgTextAttr	attr;
		m_text.GetBaseAttr( attr );

		WgPen	pen;
		pen.SetAttributes( attr );
		pen.SetChar(m_pwGlyph);
		pen.AdvancePos();

		int pwAdvance	= pen.GetPosX();
		int cursAdvance	= pCursor->Advance(m_text.cursorMode() );
		int cursBearing	= pCursor->BearingX(m_text.cursorMode() );
		int cursWidth	= pCursor->Width(m_text.cursorMode() );

		int cursOfs;		// Cursor offset from beginning of line in pixels.
		int maxOfs;			// Max allowed view offset in pixels.
		int minOfs;			// Min allowed view offset in pixels.

		int geoWidth = Size().w;
		if( m_pSkin )
			geoWidth -= m_pSkin->ContentPadding().w;
		int	lineWidth = m_text.getSoftLineWidth( 0 ) + cursBearing+cursWidth;

		// Calculate cursOfs

		if( m_bPasswordMode )
			cursOfs = cursCol * pwAdvance;
		else
			cursOfs	= m_text.getSoftLineWidthPart( 0, 0, cursCol );

		// Calculate maxOfs

		if( cursCol > 0 )
		{
			if( m_bPasswordMode )
				maxOfs = (cursCol-1) * pwAdvance;
			else
				maxOfs = m_text.getSoftLineWidthPart( 0, 0, cursCol-1 );

			if( lineWidth < maxOfs + geoWidth )
				maxOfs = WgMax( lineWidth - geoWidth, 0 );
		}
		else
			maxOfs = cursOfs;


		// Calculate minOfs

		if( cursCol < m_text.getLine(0)->nChars )
		{
			if( m_bPasswordMode )
				minOfs = (cursCol+1) * pwAdvance + cursAdvance - geoWidth;
			else
				minOfs = m_text.getSoftLineWidthPart( 0, 0, cursCol+1 ) + cursAdvance - geoWidth;	// Not 100% right, cursor might affect linewidth different from its own width.
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
	
	m_text.setState(m_state);
	_requestRender();				//TODO: Only request render if text appearance has changed.

	if( m_state.IsFocused() && !oldState.IsFocused() )
	{
		if( _isEditable() )
		{
			m_tickRouteId = WgBase::MsgRouter()->AddRoute( WG_MSG_TICK, this );
			if( m_bResetCursorOnFocus )
				m_text.GoEOL();
			_requestRender(); // render with cursor on
		}
	}

	if( !m_state.IsFocused() && oldState.IsFocused() )
	{
		if( _isSelectable() )
		{
			m_text.ClearSelection();
			m_text.setSelectionMode(false);
			m_bResetCursorOnFocus = false;
		}

		if( _isEditable() || m_viewOfs != 0 )
		{
			WgBase::MsgRouter()->DeleteRoute( m_tickRouteId );
			m_tickRouteId = 0;
			WgBase::MsgRouter()->Post( new WgTextEditMsg(text.Ptr(),true) );

			m_viewOfs = 0;
			_requestRender();
		}
	}
}

//____ _onSkinChanged() _______________________________________________________

void WgLineEditor::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
	m_text.SetColorSkin(pNewSkin);
}


//____ _onNewSize() ____________________________________________________________

void WgLineEditor::_onNewSize( const WgSize& size )
{
	_adjustViewOfs();
	_requestRender();
}

//____ _onFieldDirty() _________________________________________________________

void WgLineEditor::_onFieldDirty( WgField * pField )
{
	_requestRender();
}

//____ _onFieldResize() ________________________________________________________

void WgLineEditor::_onFieldResize( WgField * pField )
{
	m_bResetCursorOnFocus = true;
	_requestResize();
	_requestRender();
	_adjustViewOfs();
}
