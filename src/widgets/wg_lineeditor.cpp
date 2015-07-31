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

namespace wg 
{
	
	
	
	const char WgLineEditor::CLASSNAME[] = {"LineEditor"};
	
	//____ Constructor ____________________________________________________________
	
	WgLineEditor::WgLineEditor() : m_text(this), text(&m_text)
	{
		m_text.setWrap(false);
		m_text.setAutoEllipsis(isAutoEllipsisDefault());
		m_text.setEditMode( WG_TEXT_EDITABLE );
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
			WgBase::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgLineEditor::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgLineEditor::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgLineEditor_p WgLineEditor::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgLineEditor_p( static_cast<WgLineEditor*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setEditMode() __________________________________________________________
	
	void WgLineEditor::setEditMode(WgTextEditMode mode)
	{
		m_text.setEditMode( mode );
	
		if( _isSelectable() )
		{
			m_pointerStyle = WG_POINTER_IBEAM;
		}
		else
		{
			m_pointerStyle = WG_POINTER_DEFAULT;
		}
	}
	
	//____ setPasswordGlyph() _____________________________________________________
	
	void WgLineEditor::setPasswordGlyph( Uint16 glyph )
	{
		assert(glyph);
		if(glyph)
		{
			m_pwGlyph = glyph;
		}
	}
	
	//____ insertTextAtCursor() ___________________________________________________
	
	int WgLineEditor::insertTextAtCursor( const WgCharSeq& str )
	{
		if( !_isEditable() )
			return 0;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return 0;				// Couldn't get input focus...
	
		int retVal = m_text.putText( str );
	
		WgBase::msgRouter()->post( new WgTextEditMsg(text.ptr(),false) );
	
		_adjustViewOfs();
	
		return retVal;
	}
	
	//____ insertCharAtCursor() ___________________________________________________
	
	bool WgLineEditor::insertCharAtCursor( Uint16 c )
	{
		if( !_isEditable() )
			return false;
	
		if( !m_state.isFocused() )
			if( !grabFocus() )
				return false;				// Couldn't get input focus...
	
		if( !m_text.putChar( c ) )
			return false;
	
		WgBase::msgRouter()->post( new WgTextEditMsg(text.ptr(),false) );
	
		_adjustViewOfs();
		return true;
	}
	
	//____ preferredSize() __________________________________________________________
	
	WgSize WgLineEditor::preferredSize() const
	{
		WgTextAttr attr;
		m_text.getBaseAttr( attr );
		int width = WgTextTool::lineWidth( attr, "MMMMMMMMMM" );		// Default line editor should fit 10 letter M in textfield
		WgSize contentSize( m_text.height(), width );
		
		if( m_pSkin )
			return m_pSkin->sizeForContent( contentSize );
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
			int nChars = m_text.length();
			Uint16 * pContent = new Uint16[nChars+1];
			for( int i = 0 ; i < nChars ; i++ )
				pContent[i] = m_pwGlyph;
			pContent[nChars] = 0;
			pText = new WgLegacyTextField( pContent );
			delete [] pContent;
	
			pText->setWrap(false);
			pText->setAutoEllipsis(false);
			pText->setAlignment(m_text.alignment());
			pText->setProperties(m_text.properties());
			pText->setSelectionProperties(m_text.selectionProperties());
			pText->setState(m_text.state());
	
			pText->setEditMode(m_text.editMode());
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
			canvas = m_pSkin->sizeForContent(_canvas);
		else
			canvas = _canvas;
	
		WgRect	textClip( _clip, canvas );
	
		canvas.x -= m_viewOfs;
		canvas.w += m_viewOfs;
	
		if( m_state.isFocused() && _isEditable() )
			pText->showCursor();
		else
			pText->hideCursor();
	
		pDevice->printText( textClip, pText, canvas );
	
		if( pText != &m_text )
			delete pText;
	}
	
	//____ _onMsg() ______________________________________________________________
	
	void WgLineEditor::_onMsg( const WgMsg_p& pMsg )
	{
		WgWidget::_onMsg(pMsg);
	
		WgMsgRouter_p	pHandler = WgBase::msgRouter();
		WgMsgType event = pMsg->type();
	
		if( event == WG_MSG_TICK )
		{
			if( _isSelectable() && m_state.isFocused() )
			{
				m_text.incTime( WgTickMsg::cast(pMsg)->millisec() );
				_requestRender();					//TODO: Should only render the cursor and selection!
			}
			return;
		}
	
		if( (event == WG_MSG_MOUSE_PRESS || event == WG_MSG_MOUSE_DRAG) && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
		{
			if( !m_state.isFocused() )
				grabFocus();
	
			if( m_state.isFocused() )
			{
				if( _isSelectable() && (pMsg->modKeys() & WG_MODKEY_SHIFT) )
				{
					m_text.setSelectionMode(true);
				}
	
				WgCoord ofs = pMsg->pointerPos() - globalPos();
				int x = ofs.x + m_viewOfs;
				int y = 0;
	
				if( m_bPasswordMode )
				{
					WgTextAttr	attr;
					m_text.getBaseAttr( attr );
	
					WgPen	pen;
					pen.setAttributes( attr );
					pen.setChar(m_pwGlyph);
					pen.advancePos();
	
					int spacing = pen.getPosX();
					int height = pen.getLineSpacing();
	
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
					m_text.cursorGotoCoord( WgCoord(x, 0), WgRect(0,0,1000000,1000000) );
				}
	
				if(_isSelectable() && event == WG_MSG_MOUSE_PRESS && !(pMsg->modKeys() & WG_MODKEY_SHIFT))
				{
					m_text.clearSelection();
					m_text.setSelectionMode(true);
				}
			}
			_adjustViewOfs();
		}
	
		if( event == WG_MSG_MOUSE_RELEASE )
		{
			if( m_state.isFocused() && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
				m_text.setSelectionMode(false);
		}		
	
		if( event == WG_MSG_CHARACTER )
		{
			int ch = WgCharacterMsg::cast(pMsg)->character();
	
			if( _isEditable() && m_state.isFocused() && ch >= 32 && ch != 127)
			{
	
				if(m_text.hasSelection())
					m_text.delSelection();
				m_text.setSelectionMode(false);
	
				if( m_text.putChar( ch ) )
				{
					if( pHandler )
						pHandler->post( new WgTextEditMsg(text.ptr(),false) );
	
					_adjustViewOfs();
				}
			}
		}
	
		if( event == WG_MSG_KEY_RELEASE && m_state.isFocused() )
		{
			int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
			switch( key )
			{
				case WG_KEY_SHIFT:
					if(!pHandler->isMouseButtonPressed(1))
						m_text.setSelectionMode(false);
				break;
			}
		}
	
		if( (event == WG_MSG_KEY_PRESS || event == WG_MSG_KEY_REPEAT) && _isEditable() && m_state.isFocused() )
		{
			int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
			switch( key )
			{
				case WG_KEY_LEFT:
					if( pMsg->modKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( pMsg->modKeys() & WG_MODKEY_CTRL )
					{
						if( m_bPasswordMode )
							m_text.goBol();
						else
							m_text.gotoPrevWord();
					}
					else
					{
						m_text.goLeft();
					}
					break;
				case WG_KEY_RIGHT:
					if( pMsg->modKeys() & WG_MODKEY_SHIFT )
						m_text.setSelectionMode(true);
	
					if( pMsg->modKeys() & WG_MODKEY_CTRL )
					{
						if( m_bPasswordMode )
							m_text.goEol();
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
					else if( (pMsg->modKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
						m_text.delPrevWord();
					else
						m_text.delPrevChar();
	
					if( pHandler )
						pHandler->post( new WgTextEditMsg(text.ptr(),false) );
					break;
				}
	
				case WG_KEY_DELETE:
				{
					if(m_text.hasSelection())
						m_text.delSelection();
					else if( (pMsg->modKeys() & WG_MODKEY_CTRL) && !m_bPasswordMode)
						m_text.delNextWord();
					else
						m_text.delNextChar();
	
					if( pHandler )
						pHandler->post( new WgTextEditMsg(text.ptr(),false) );
					break;
				}
	
				case WG_KEY_HOME:
	
					/*
					 *	I am not sure if this is the proper way to this, but in my opinion, the default
					 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( pMsg->modKeys() )
					{
	
					case WG_MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if(pMsg->modKeys() & WG_MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goBol();
						break;
					}
	
					break;
	
				case WG_KEY_END:
	
					/*
				 	 *	I am not sure if this is the proper way to this, but in my opinion, the default
			 		 *	"actions" has to be separated from any modifier key action combination
					 */
					switch( pMsg->modKeys() )
					{
	
					case WG_MODKEY_CTRL:
						break;
	
					default: // no modifier key was pressed
						if( pMsg->modKeys() & WG_MODKEY_SHIFT )
							m_text.setSelectionMode(true);
	
						m_text.goEol();
						break;
					}
	
					break;
	
				default:
					break;
			}
			_adjustViewOfs();
		}
	
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtonMsg() )
		{
			if( WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
				pMsg->swallow();
		}
		else if( pMsg->isKeyMsg() )
		{
			int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
			if( WgKeyMsg::cast(pMsg)->isMovementKey() == true ||
				key == WG_KEY_DELETE || key == WG_KEY_BACKSPACE )
					pMsg->swallow();
			
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
	
		if( m_state.isFocused() && m_text.properties() && m_text.properties()->font() )
		{
			WgCaret_p pCursor = WgTextTool::getCursor( &m_text );
			if( !pCursor )
				return;
	
			int cursCol	= m_text.column();
	
			WgTextAttr	attr;
			m_text.getBaseAttr( attr );
	
			WgPen	pen;
			pen.setAttributes( attr );
			pen.setChar(m_pwGlyph);
			pen.advancePos();
	
			int pwAdvance	= pen.getPosX();
			int cursAdvance	= pCursor->advance(m_text.cursorMode() );
			int cursBearing	= pCursor->bearingX(m_text.cursorMode() );
			int cursWidth	= pCursor->width(m_text.cursorMode() );
	
			int cursOfs;		// Cursor offset from beginning of line in pixels.
			int maxOfs;			// Max allowed view offset in pixels.
			int minOfs;			// Min allowed view offset in pixels.
	
			int geoWidth = size().w;
			if( m_pSkin )
				geoWidth -= m_pSkin->contentPadding().w;
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
	
		if( m_state.isFocused() && !oldState.isFocused() )
		{
			if( _isEditable() )
			{
				m_tickRouteId = WgBase::msgRouter()->addRoute( WG_MSG_TICK, this );
				if( m_bResetCursorOnFocus )
					m_text.goEol();
				_requestRender(); // render with cursor on
			}
		}
	
		if( !m_state.isFocused() && oldState.isFocused() )
		{
			if( _isSelectable() )
			{
				m_text.clearSelection();
				m_text.setSelectionMode(false);
				m_bResetCursorOnFocus = false;
			}
	
			if( _isEditable() || m_viewOfs != 0 )
			{
				WgBase::msgRouter()->deleteRoute( m_tickRouteId );
				m_tickRouteId = 0;
				WgBase::msgRouter()->post( new WgTextEditMsg(text.ptr(),true) );
	
				m_viewOfs = 0;
				_requestRender();
			}
		}
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void WgLineEditor::_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin )
	{
		WgWidget::_onSkinChanged(pOldSkin,pNewSkin);
		m_text.setColorSkin(pNewSkin);
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

} // namespace wg
