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

#include <memory.h>
#include <wg_scrollbar.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_scrollbartarget.h>
#include <wg_eventhandler.h>

using namespace WgUtil;

const char WgScrollbar::CLASSNAME[] = {"Scrollbar"};


//____ WgScrollbar() ____________________________________________________

WgScrollbar::WgScrollbar()
{
	m_pScrollbarTargetInterface = 0;

	m_handleSize 		= 1.0;
	m_handlePos 		= 0.0;

	m_bHorizontal		= false;
	m_bgPressMode		= JUMP_PAGE;
	m_bPressOnHandle	= false;
	m_handlePressOfs	= 0;

	m_btnLayout			= DEFAULT;
	m_headerLen			= 0;
	m_footerLen			= 0;

	m_lastCursorDownPos = WgCoord(-4096, -4096);

	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS; i++ )
		m_states[i] = WG_STATE_NORMAL;
}

//____ ~WgScrollbar() _________________________________________________________

WgScrollbar::~WgScrollbar( void )
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgScrollbar::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgScrollbar::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgScrollbarPtr WgScrollbar::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgScrollbarPtr( static_cast<WgScrollbar*>(pObject.RawPtr()) );

	return 0;
}

//____ SetOrientation() _______________________________________________________

void WgScrollbar::SetOrientation( WgOrientation orientation )
{
	bool bHorizontal = (orientation == WG_HORIZONTAL);

	if( m_bHorizontal != bHorizontal )
	{
		m_bHorizontal = bHorizontal;
		_headerFooterChanged();
		_updateMinSize();
		_requestRender();
	}
}


//____ SetBackgroundPressMode() _______________________________________________________

void WgScrollbar::SetBackgroundPressMode( BgPressMode mode )
{
	m_bgPressMode = mode;
}



//____ SetHandle() ____________________________________________________________

bool WgScrollbar::SetHandle( float _pos, float _size )
{
	WG_LIMIT( _size, 0.0001f, 1.f );
	WG_LIMIT( _pos, 0.f, 1.f );

	if( m_handlePos == _pos && m_handleSize == _size )
		return true;

	m_handlePos		= _pos;
	m_handleSize 	= _size;

	_requestRender();
	return	true;
}

//____ SetHandlePos() _________________________________________________________

bool WgScrollbar::SetHandlePos( float pos )
{
	WG_LIMIT( pos, 0.f, 1.f );

	if( pos > m_handlePos-0.000001 && pos < m_handlePos+0.000001 )
		return true;

	m_handlePos = pos;

	_requestRender();
	return	true;
}

//____ SetHandlePosPxlOfs() ___________________________________________________

bool WgScrollbar::SetHandlePosPxlOfs( int x )
{
	int		handlePos, handleLen;
	_viewToPosLen( &handlePos, &handleLen );

	int		length;
	if( m_bHorizontal )
	{
		length = Size().w;
		if( m_pSkin )
			length -= m_pSkin->ContentPadding().w;
	}
	else
	{
		length = Size().h;
		if( m_pSkin )
			length -= m_pSkin->ContentPadding().h;
	}

	length -= m_headerLen + m_footerLen;

	float scrollhandlePos = 0.f;
	if( m_handleSize < 1.f)
		scrollhandlePos = ((float)(x - (handleLen >> 1))) / (length - handleLen);

	return SetHandlePos(scrollhandlePos);
}

//____ SetHandleSize() ________________________________________________________

bool WgScrollbar::SetHandleSize( float _size )
{
	WG_LIMIT( _size, 0.0001f, 1.f );

	if( _size == m_handleSize )
		return true;

	m_handleSize = _size;

	_requestRender();
	return	true;
}


//____ SetSkins() ____________________________________________________________

void WgScrollbar::SetSkins( const WgSkinPtr& pBackgroundSkin, const WgSkinPtr& pHandleSkin, 
							const WgSkinPtr& pBwdButtonSkin, const WgSkinPtr& pFwdButtonSkin )
{
	m_pBgSkin		= pBackgroundSkin;
	m_pHandleSkin	= pHandleSkin;
	m_pBtnFwdSkin	= pFwdButtonSkin;
	m_pBtnBwdSkin	= pBwdButtonSkin;

	_headerFooterChanged();
	_updateMinSize();
	_requestRender();
}

//____ SetButtonLayout() ______________________________________________________

void WgScrollbar::SetButtonLayout(  ButtonLayout layout )
{
	m_btnLayout = layout;

	_headerFooterChanged();
}

//____ SetScrollbarTarget() _______________________________________________________

bool WgScrollbar::SetScrollbarTarget( WgScrollbarTarget * pTarget )
{
	// Release previous target (if any)

	if( m_pScrollbarTargetWidget )
		m_pScrollbarTargetInterface->m_pScrollbar = 0;

	// Set new target

	if( pTarget == 0 )
	{
		m_pScrollbarTargetInterface	= 0;
		m_pScrollbarTargetWidget		= 0;
	}
	else
	{
		if( pTarget->m_pScrollbar )
			return false;									// Target is already controlled by another scrollbar.

		m_pScrollbarTargetInterface 	= pTarget;
		m_pScrollbarTargetWidget		= pTarget->_getWidget();
		m_pScrollbarTargetInterface->m_pScrollbar = this;
		_setHandle( pTarget->_getHandlePosition(), pTarget->_getHandleSize() );
	}
	return true;
}

//____ _headerFooterChanged() _______________________________________________

void WgScrollbar::_headerFooterChanged()
{
	int	fwdLen = 0, bwdLen = 0;

	if( m_bHorizontal )
	{
		if( m_pBtnFwdSkin )
			fwdLen = m_pBtnFwdSkin->PreferredSize().w;
		if( m_pBtnBwdSkin )
			bwdLen = m_pBtnBwdSkin->PreferredSize().w;
	}
	else
	{
		if( m_pBtnFwdSkin )
			fwdLen = m_pBtnFwdSkin->PreferredSize().h;
		if( m_pBtnBwdSkin )
			bwdLen = m_pBtnBwdSkin->PreferredSize().h;
	}

	int	headerLen = 0;
	int footerLen = 0;

	if( (m_btnLayout & HEADER_BWD) )
		headerLen += bwdLen;

	if( (m_btnLayout & HEADER_FWD) )
		headerLen += fwdLen;

	if( (m_btnLayout & FOOTER_BWD) )
		footerLen += bwdLen;

	if( (m_btnLayout & FOOTER_FWD) )
		footerLen += fwdLen;

	if( headerLen != m_headerLen || footerLen != m_footerLen )
	{
		m_headerLen = headerLen;
		m_footerLen = footerLen;

		_updateMinSize();
	}

	_requestRender();
}



//____ _onCloneContent() _______________________________________________________

void WgScrollbar::_onCloneContent( const WgWidget * _pOrg )
{
	WgScrollbar * pOrg = (WgScrollbar *) _pOrg;

	m_pBgSkin			= pOrg->m_pBgSkin;
	m_pHandleSkin		= pOrg->m_pHandleSkin;
	m_pBtnFwdSkin		= pOrg->m_pBtnFwdSkin;
	m_pBtnBwdSkin		= pOrg->m_pBtnBwdSkin;

  	m_handlePos			= pOrg->m_handlePos;
	m_handleSize		= pOrg->m_handleSize;

	m_bHorizontal		= pOrg->m_bHorizontal;
	m_bgPressMode		= pOrg->m_bgPressMode;

	m_bPressOnHandle	= 0;
	m_handlePressOfs	= 0;

	m_btnLayout			= pOrg->m_btnLayout;
	m_headerLen			= pOrg->m_headerLen;
	m_footerLen			= pOrg->m_footerLen;

	m_minSize			= pOrg->m_minSize;
}

//____ _viewToPosLen() _________________________________________________________

void	WgScrollbar::_viewToPosLen( int * _wpPos, int * _wpLen )
{
	// changes by Viktor.

	// using floats here results in errors.
	//float pos, len;
	int pos, len;

	int maxLen;
	if( m_bHorizontal )
	{
		maxLen = Size().w;
		if( m_pSkin )
			maxLen -= m_pSkin->ContentPadding().w;
	}
	else
	{
		maxLen = Size().h;
		if( m_pSkin )
			maxLen -= m_pSkin->ContentPadding().h;
	}
	maxLen -= m_headerLen + m_footerLen;

	//len = m_handleSize * maxLen;
	len = (int)(m_handleSize * (float)maxLen);

	int		minLen;

	if( m_bHorizontal )
		minLen = m_pHandleSkin->MinSize().w;
	else
		minLen = m_pHandleSkin->MinSize().h;

	if( minLen < 4 )
		minLen = 4;


	if( len < minLen )
	{
		//len = (float) minLen;
		len = minLen;
	}

	//pos = m_handlePos * (maxLen-len);
	pos = (int)(m_handlePos * (float)(maxLen-len));

	if( pos + len > maxLen )
		pos = maxLen - len;

//	pos += m_headerLen;

	//* _wpPos = (int) pos;
	//* _wpLen = (int) len;
	* _wpPos = pos;
	* _wpLen = len;
//	* _wpLen = ((int)(pos + len)) - (int) pos;
}


//____ _onStateChanged() ______________________________________________________

void WgScrollbar::_onStateChanged( WgState oldState )
{
	if( m_state.IsEnabled() != oldState.IsEnabled() )
	{
		for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
			m_states[i].SetEnabled(m_state.IsEnabled());
	}
	_requestRender();
}


//____ _onRefresh() _______________________________________________________

void WgScrollbar::_onRefresh( void )
{
	WgWidget::_onRefresh();
}

//____ PreferredSize() _____________________________________________________________

WgSize WgScrollbar::PreferredSize() const
{
	WgSize sz = m_minSize;

	// Add 50 pixels in the scrollbars direction for best size.

	if( m_bHorizontal )
		sz.w += 50;
	else
		sz.h += 50;

	return sz;
}


//____ _updateMinSize() ________________________________________________________

void WgScrollbar::_updateMinSize()
{
	int	minW = 4;
	int	minH = 4;

	// Check min w/h for BgGfx.

	if( m_pBgSkin )
	{
		WgSize sz = m_pBgSkin->MinSize();

		minW = Max( minW, sz.w );
		minH = Max( minH, sz.h );
	}

	// Check min w/h for BarGfx.

	if( m_pHandleSkin )
	{
		WgSize sz = m_pHandleSkin->MinSize();

		minW = Max( minW, sz.w );
		minH = Max( minH, sz.h );
	}


	// Add header and footer to minW/minH from scrollbar part.

	if( m_bHorizontal )
		minW += m_footerLen + m_headerLen;
	else
		minH += m_footerLen + m_headerLen;


	// Check min w/h for forward button.

	if( m_pBtnFwdSkin && (m_btnLayout & (HEADER_FWD | FOOTER_FWD)) )
	{
		WgSize sz = m_pBtnFwdSkin->PreferredSize();

		minW = Max( minW, sz.w );
		minH = Max( minH, sz.h );
	}

	// Check min w/h for backward button.

	if( m_pBtnBwdSkin && (m_btnLayout & (HEADER_BWD | FOOTER_BWD)) )
	{
		WgSize sz = m_pBtnBwdSkin->PreferredSize();

		minW = Max( minW, sz.w );
		minH = Max( minH, sz.h );
	}

	// Add padding for base skin

	if( m_pSkin )
	{
		minW += m_pSkin->ContentPadding().w;
		minH += m_pSkin->ContentPadding().h;
	}

	// Set if changed.

	if( minW != m_minSize.w || minH != m_minSize.h )
	{
		m_minSize.w = minW;
		m_minSize.h = minH;

		_requestResize();
	}
}

//____ _renderButton() _________________________________________________________

void WgScrollbar::_renderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgSkinPtr& pSkin, WgState state )
{
		if( m_bHorizontal )
			_dest.w = pSkin->PreferredSize().w;
		else
			_dest.h = pSkin->PreferredSize().h;

		pSkin->Render( pDevice, _dest, state, _clip );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;
}

//____ _onRender() ________________________________________________________

void WgScrollbar::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect	dest = _canvas;
	if( m_pSkin )
		dest = m_pSkin->ContentRect(_canvas,m_state);

	// Render header buttons

	if( m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD] );

	if( m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD] );

	// Render background (if any).

	if( m_bHorizontal )
		dest.w = Size().w - m_headerLen - m_footerLen;
	else
		dest.h = Size().h - m_headerLen - m_footerLen;

	if( m_pBgSkin )
		m_pBgSkin->Render( pDevice, dest, m_states[C_BG], _clip );

	// Render the handle

	if( m_pHandleSkin )
	{
		int handlePos;
		int handleLen;
		_viewToPosLen( &handlePos, &handleLen );

		WgRect	handleDest;
		if( m_bHorizontal )
			handleDest = WgRect( dest.x + handlePos, dest.y, handleLen, dest.h );
		else
			handleDest = WgRect( dest.x, dest.y + handlePos, dest.w, handleLen );

		m_pHandleSkin->Render( pDevice, handleDest, m_states[C_HANDLE], _clip );
	}

	// Render footer buttons

	if( m_bHorizontal )
		dest.x += dest.w;
	else
		dest.y += dest.h;

	if( m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD] );

	if( m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD] );
}

//____ _onAlphaTest() ______________________________________________________

bool WgScrollbar::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	if( _findMarkedComponent( ofs ) != C_NONE )
		return true;

	return WgWidget::_onAlphaTest(ofs,sz);
}

//____ _markTestButton() _______________________________________________________

bool WgScrollbar::_markTestButton( WgCoord ofs, WgRect& _dest, const WgSkinPtr& pSkin, WgState state )
{
		if( m_bHorizontal )
			_dest.w = pSkin->PreferredSize().w;
		else
			_dest.h = pSkin->PreferredSize().h;

		bool retVal = pSkin->MarkTest( ofs, _dest, state, m_markOpacity );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;

		return retVal;
}

//____ _findMarkedComponent() __________________________________________________

WgScrollbar::Component WgScrollbar::_findMarkedComponent( WgCoord ofs )
{
	WgRect canvas;

	if( m_pSkin )
		canvas = m_pSkin->ContentRect( WgRect(0,0,Size()),m_state);
	else 
		canvas = WgRect(0,0,Size());

	WgRect dest = canvas;

	// First of all, do a mark test against the header buttons...

	if( m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD]) )
			return C_HEADER_BWD;
	}

	if( m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD]) )
			return C_HEADER_FWD;
	}

	// Then do a mark test against the footer buttons...

	if( m_bHorizontal )
		dest.x = canvas.x + canvas.w - m_footerLen;
	else
		dest.y = canvas.y + canvas.h - m_footerLen;

	if( m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD]) )
			return C_FOOTER_BWD;
	}

	if( m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD]) )
			return C_FOOTER_FWD;
	}

	// Then, do a mark test against the dragbar...

	if( _markTestHandle( ofs ) == true )
		return C_HANDLE;

	// Do a mark test against the dragbar background.

	WgRect	r = canvas;

	if( m_bHorizontal )
	{
		r.x += m_headerLen;
		r.w -= m_headerLen + m_footerLen;
	}
	else
	{
		r.y += m_headerLen;
		r.h -= m_headerLen + m_footerLen;
	}

	if( m_pBgSkin && m_pBgSkin->MarkTest( ofs, r, m_states[C_BG], m_markOpacity ) )
		return C_BG;

	return C_NONE;
}

//____ _unhoverReqRender() ______________________________________________________

void WgScrollbar::_unhoverReqRender()
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_states[i].SetHovered(false);

	_requestRender();
}

//____ _onEvent() ______________________________________________________________

void WgScrollbar::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	int		handlePos, handleLen;
	_viewToPosLen( &handlePos, &handleLen );

	WgCoord pos = pEvent->PointerPos();

	int		pointerOfs;
	int		length;

	WgRect contentRect;
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect(WgRect(0,0,Size()),m_state);
	else
		contentRect = WgRect(0,0,Size());

	if( m_bHorizontal )
	{
		pointerOfs = pos.x - contentRect.x;
		length = contentRect.w;
	}
	else
	{
		pointerOfs = pos.y - contentRect.y;
		length = contentRect.h;
	}

	length -= m_headerLen + m_footerLen;
	pointerOfs -= m_headerLen;

	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSE_RELEASE:
		{
			if( WgMouseButtonEvent::Cast(pEvent)->Button() != WG_BUTTON_LEFT )
				return;

			// Just put them all to NORMAL and request render.
			// Release is followed by over before render anyway so right one will be highlighted.

			_unhoverReqRender();
			break;
		}

		case WG_EVENT_MOUSE_LEAVE:
		{
			// Turn any MARKED/SELECTED button/bg back to NORMAL.
			// Turn handle back to NORMAL only if MARKED (selected handle should remain selected).
			// Request render only if something changed (which it has unless bar was SELECTED...).

			if( m_states[C_HANDLE].IsPressed() )
				return;

			_unhoverReqRender();
			break;
		}

		case WG_EVENT_MOUSE_ENTER:
		case WG_EVENT_MOUSE_MOVE:
		{
			if( m_states[C_HANDLE].IsPressed() )
				return;

			Component c = _findMarkedComponent(pos);

			if( (c != C_NONE && !m_states[c].IsHovered()) || (c == C_BG && m_states[C_HANDLE].IsHovered()) )
			{
				_unhoverReqRender();
				m_states[c].SetHovered(true);
				if( c == C_HANDLE )
					m_states[C_BG].SetHovered(true);			// Always also mark bg if bar is marked.
			}
			else if( c == C_BG && m_states[C_HANDLE].IsHovered() )
			{
				m_states[C_HANDLE].SetHovered(false);
				m_states[C_BG].SetHovered(false);
				_requestRender();
			}


			break;
		}

		case WG_EVENT_MOUSE_PRESS:
		{
			if( WgMouseButtonEvent::Cast(pEvent)->Button() != WG_BUTTON_LEFT )
				return;

			Component c = _findMarkedComponent(pos);

			_unhoverReqRender();
			m_states[c].SetPressed(true);

			if( c == C_HANDLE )
			{
				m_handlePressOfs = pointerOfs - handlePos;
				m_states[C_BG].SetHovered(true);			// Always mark bg if bar is pressed.
			}
			else if( c == C_BG )
			{
				switch( m_bgPressMode )
				{
				case JUMP_PAGE:

					if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					{
						if( pointerOfs - handlePos < handleLen/2 )
							SetHandlePos( m_pScrollbarTargetInterface->_jumpBwd() );
						else
							SetHandlePos( m_pScrollbarTargetInterface->_jumpFwd() );
					}

					int pxlPos, pxlLen;
					_viewToPosLen( &pxlPos, &pxlLen );
					pHandler->QueueEvent( new WgRangeUpdateEvent(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );

					break;
				case GOTO_POS:
					m_states[C_HANDLE].SetPressed(true);
					m_states[C_BG].SetHovered(true);
					m_handlePressOfs = handleLen/2;
					SetHandlePosPxlOfs( pointerOfs );
					break;
				default:
//					assert( false );
					break;
				}

			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
			{
				if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					SetHandlePos( m_pScrollbarTargetInterface->_stepFwd() );
			}
			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
			{
				if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					SetHandlePos( m_pScrollbarTargetInterface->_stepBwd() );
			}


			int pxlPos, pxlLen;
			_viewToPosLen( &pxlPos, &pxlLen );
			pHandler->QueueEvent( new WgRangeUpdateEvent(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
			break;
		}

		case WG_EVENT_MOUSE_REPEAT:
		{
			if( WgMouseButtonEvent::Cast(pEvent)->Button() != WG_BUTTON_LEFT )
				return;

			if( m_states[C_HANDLE].IsPressed() )
				return;

			Component c = _findMarkedComponent(pos);

			if( c == C_BG )
			{
				if( pointerOfs - handlePos < handleLen/2 )
				{
					if( m_pScrollbarTargetWidget.RawPtr() != 0 )
						SetHandlePos( m_pScrollbarTargetInterface->_jumpBwd() );
				}
				else
				{
					if( m_pScrollbarTargetWidget.RawPtr() != 0 )
						SetHandlePos( m_pScrollbarTargetInterface->_jumpFwd() );
				}
			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
			{
				if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					SetHandlePos( m_pScrollbarTargetInterface->_stepFwd() );
			}
			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
			{
				if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					SetHandlePos( m_pScrollbarTargetInterface->_stepBwd() );
			}

			int pxlPos, pxlLen;
			_viewToPosLen( &pxlPos, &pxlLen );
			pHandler->QueueEvent( new WgRangeUpdateEvent(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
			break;
		}

		case WG_EVENT_MOUSE_DRAG:
		{
			if( WgMouseButtonEvent::Cast(pEvent)->Button() != WG_BUTTON_LEFT )
				return;

			if( m_states[C_HANDLE].IsPressed() )
			{
				float	scrollhandlePos = 0.f;

				if( m_handleSize < 1.f)
					scrollhandlePos = ((float)(pointerOfs - m_handlePressOfs)) / (length - handleLen);

				WG_LIMIT( scrollhandlePos, 0.f, 1.f );

				if( scrollhandlePos != m_handlePos )
				{
					m_handlePos = scrollhandlePos;

					if( m_pScrollbarTargetWidget.RawPtr() != 0 )
						m_handlePos = m_pScrollbarTargetInterface->_setPosition(m_handlePos);

					int pxlPos, pxlLen;
					_viewToPosLen( &pxlPos, &pxlLen );
					pHandler->QueueEvent( new WgRangeUpdateEvent(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
					_requestRender();
				}
			}
			break;
		}
		
		case WG_EVENT_WHEEL_ROLL:
		{
			WgWheelRollEventPtr p = WgWheelRollEvent::Cast(pEvent);

			if( p->Wheel() == 1 )
			{
				int distance = p->Distance();
				if( m_pScrollbarTargetWidget.RawPtr() != 0 )
					SetHandlePos( m_pScrollbarTargetInterface->_wheelRolled(distance) );
				
				int pxlPos, pxlLen;
				_viewToPosLen( &pxlPos, &pxlLen );
				pHandler->QueueEvent( new WgRangeUpdateEvent(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
				pHandler->SwallowEvent(pEvent);
			}
		}
		
        default:
            break;

	}

	// Swallow all button 1 events.

	if( pEvent->IsMouseButtonEvent() && WgMouseButtonEvent::Cast(pEvent)->Button() == WG_BUTTON_LEFT )
			pHandler->SwallowEvent(pEvent);

}


//____ _markTestHandle() _______________________________________________________

bool WgScrollbar::_markTestHandle( WgCoord ofs )
{
	if( !m_pHandleSkin )
		return false;

	int   handlePos, handleLen;
	_viewToPosLen( &handlePos, &handleLen );

	WgRect area(0,0,Size());

	if( m_pSkin )
		area = m_pSkin->ContentRect(area,m_state);

	if( m_bHorizontal )
	{
		area.x = handlePos + m_headerLen;
		area.w = handleLen;
	}
	else
	{
		area.y = handlePos + m_headerLen;
		area.h = handleLen;
	}

	return m_pHandleSkin->MarkTest( ofs, area, m_states[C_HANDLE], m_markOpacity );
}

//____ _setHandle() ____________________________________________________________

bool WgScrollbar::_setHandle( float _pos, float _size )
{
	WG_LIMIT( _size, 0.0001f, 1.f );
	WG_LIMIT( _pos, 0.f, 1.f );

	if( m_handlePos == _pos && m_handleSize == _size )
		return true;

	m_handlePos		= _pos;
	m_handleSize 	= _size;

	_requestRender();
	return	true;
}
