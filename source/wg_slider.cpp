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
#include <wg_slider.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_slidertarget.h>
#include <wg_eventhandler.h>

using namespace WgUtil;

static const char	c_widgetType[] = {"Unspecified type derived from Slider"};
static const char c_widgetTypeH[] = {"HSlider"};
static const char c_widgetTypeV[] = {"VSlider"};


//____ WgWidgetSlider() ____________________________________________________

WgWidgetSlider::WgWidgetSlider()
{
	m_pSliderTargetInterface = 0;

	m_sliderSize 		= 1.0;
	m_sliderPos 		= 0.0;

	m_bgPressMode		= SKIP_PAGE;
	m_bPressOnDragBar	= false;
	m_dragBarPressOfs	= 0;

	m_btnLayout			= DEFAULT;
	m_headerLen			= 0;
	m_footerLen			= 0;

	m_lastCursorDownPos = WgCoord(-4096, -4096);

	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS; i++ )
		m_mode[i] = WG_MODE_NORMAL;
}

//____ ~WgWidgetSlider() _________________________________________________________

WgWidgetSlider::~WgWidgetSlider( void )
{
}


//____ Type() _________________________________________________________________

const char * WgWidgetSlider::Type( void ) const
{
	return GetClass();
}

const char * WgWidgetSlider::GetClass( void )
{
	return c_widgetType;
}

//____ SetBgPressMode() _______________________________________________________

void WgWidgetSlider::SetBgPressMode( BgPressMode mode )
{
	m_bgPressMode = mode;
}



//____ SetSlider() ____________________________________________________________

bool WgWidgetSlider::SetSlider( float _pos, float _size )
{
	LIMIT( _size, 0.0001f, 1.f );
	LIMIT( _pos, 0.f, 1.f );

	if( m_sliderPos == _pos && m_sliderSize == _size )
		return true;

	m_sliderPos		= _pos;
	m_sliderSize 	= _size;

	_requestRender();
	return	true;
}

//____ SetSliderPos() _________________________________________________________

bool WgWidgetSlider::SetSliderPos( float pos )
{
	LIMIT( pos, 0.f, 1.f );

	if( pos > m_sliderPos-0.000001 && pos < m_sliderPos+0.000001 )
		return true;

	m_sliderPos = pos;

	_requestRender();
	return	true;
}

bool WgWidgetSlider::SetSliderPosPxlOfs( int x )
{
	int		barPos, barLen;
	_viewToPosLen( &barPos, &barLen );

	int		length;
	if( m_bHorizontal )
	{
		length = Size().w;
	}
	else
	{
		length = Size().h;
	}

	length -= m_headerLen + m_footerLen;

	float sliderPos = 0.f;
	if( m_sliderSize < 1.f)
		sliderPos = ((float)(x - (barLen >> 1))) / (length - barLen);

	return SetSliderPos(sliderPos);
}

//____ SetSliderSize() ________________________________________________________

bool WgWidgetSlider::SetSliderSize( float _size )
{
	LIMIT( _size, 0.0001f, 1.f );

	if( _size == m_sliderSize )
		return true;

	m_sliderSize = _size;

	_requestRender();
	return	true;
}


//____ SetSource() ____________________________________________________________

bool WgWidgetSlider::SetSource( WgBlocksetPtr pBgGfx, WgBlocksetPtr pBarGfx,
							 WgBlocksetPtr pBtnBwdGfx, WgBlocksetPtr pBtnFwdGfx )
{
	m_pBgGfx		= pBgGfx;
	m_pBarGfx		= pBarGfx;
	m_pBtnFwdGfx	= pBtnFwdGfx;
	m_pBtnBwdGfx	= pBtnBwdGfx;

	_headerFooterChanged();
	_updateMinSize();
	_requestRender();
	return true;
}

//____ SetButtonLayout() ______________________________________________________

bool WgWidgetSlider::SetButtonLayout(  ButtonLayout layout )
{
	m_btnLayout = layout;

	_headerFooterChanged();
	return true;
}

//____ SetSliderTarget() _______________________________________________________

bool WgWidgetSlider::SetSliderTarget( WgSliderTarget * pTarget )
{
	// Release previous target (if any)

	if( m_pSliderTargetWidget )
		m_pSliderTargetInterface->m_pSlider = 0;

	// Set new target

	if( pTarget == 0 )
	{
		m_pSliderTargetInterface	= 0;
		m_pSliderTargetWidget		= 0;
	}
	else
	{
		if( pTarget->m_pSlider )
			return false;									// Target is already controlled by another slider.

		m_pSliderTargetInterface 	= pTarget;
		m_pSliderTargetWidget		= pTarget->_getWidget();
		m_pSliderTargetInterface->m_pSlider = this;
		_setSlider( pTarget->_getSliderPosition(), pTarget->_getSliderSize() );
	}
	return true;
}

//____ _headerFooterChanged() _______________________________________________

void WgWidgetSlider::_headerFooterChanged()
{
	int	fwdLen = 0, bwdLen = 0;

	if( m_bHorizontal )
	{
		if( m_pBtnFwdGfx )
			fwdLen = m_pBtnFwdGfx->Width();
		if( m_pBtnBwdGfx )
			bwdLen = m_pBtnBwdGfx->Width();
	}
	else
	{
		if( m_pBtnFwdGfx )
			fwdLen = m_pBtnFwdGfx->Height();
		if( m_pBtnBwdGfx )
			bwdLen = m_pBtnBwdGfx->Height();
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

void WgWidgetSlider::_onCloneContent( const WgWidget * _pOrg )
{
	WgWidgetSlider * pOrg = (WgWidgetSlider *) _pOrg;

	m_pBgGfx			= pOrg->m_pBgGfx;
	m_pBarGfx			= pOrg->m_pBarGfx;
	m_pBtnFwdGfx		= pOrg->m_pBtnFwdGfx;
	m_pBtnBwdGfx		= pOrg->m_pBtnBwdGfx;

  	m_sliderPos			= pOrg->m_sliderPos;
	m_sliderSize		= pOrg->m_sliderSize;

	m_bHorizontal		= pOrg->m_bHorizontal;
	m_bgPressMode		= pOrg->m_bgPressMode;

	m_bPressOnDragBar	= 0;
	m_dragBarPressOfs	= 0;

	m_btnLayout			= pOrg->m_btnLayout;
	m_headerLen			= pOrg->m_headerLen;
	m_footerLen			= pOrg->m_footerLen;

	m_minSize			= pOrg->m_minSize;
}

//____ _viewToPosLen() _________________________________________________________

void	WgWidgetSlider::_viewToPosLen( int * _wpPos, int * _wpLen )
{
	// changes by Viktor.

	// using floats here results in errors.
	//float pos, len;
	int pos, len;

	int maxLen;
	if( m_bHorizontal )
		maxLen = Size().w;
	else
		maxLen = Size().h;

	maxLen -= m_headerLen + m_footerLen;

	//len = m_sliderSize * maxLen;
	len = (int)(m_sliderSize * (float)maxLen);

	int		minLen;

	if( m_bHorizontal )
		minLen = m_pBarGfx->MinWidth();
	else
		minLen = m_pBarGfx->MinHeight();

	if( minLen < 4 )
		minLen = 4;


	if( len < minLen )
	{
		//len = (float) minLen;
		len = minLen;
	}

	//pos = m_sliderPos * (maxLen-len);
	pos = (int)(m_sliderPos * (float)(maxLen-len));

	if( pos + len > maxLen )
		pos = maxLen - len;

//	pos += m_headerLen;

	//* _wpPos = (int) pos;
	//* _wpLen = (int) len;
	* _wpPos = pos;
	* _wpLen = len;
//	* _wpLen = ((int)(pos + len)) - (int) pos;
}

//____ _onEnable() ___________________________________________________

void WgWidgetSlider::_onEnable( void )
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_NORMAL;

	_requestRender();
}

//____ _onDisable() ___________________________________________________

void WgWidgetSlider::_onDisable( void )
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_DISABLED;

	_requestRender();
}

//____ _onRefresh() _______________________________________________________

void WgWidgetSlider::_onRefresh( void )
{
	_requestRender();
}

//____ DefaultSize() _____________________________________________________________

WgSize WgWidgetSlider::DefaultSize() const
{
	WgSize sz = m_minSize;

	// Add 50 pixels in the sliders direction for best size.

	if( m_bHorizontal )
		sz.w += 50;
	else
		sz.h += 50;

	return sz;
}


//____ _updateMinSize() ________________________________________________________

void WgWidgetSlider::_updateMinSize()
{
	int	minW = 4;
	int	minH = 4;

	// Check min w/h for BgGfx.

	if( m_pBgGfx )
	{
		minW = Max( minW, m_pBgGfx->MinWidth() );
		minH = Max( minH, m_pBgGfx->MinHeight() );
	}

	// Check min w/h for BarGfx.

	if( m_pBarGfx )
	{
		minW = Max( minW, m_pBarGfx->MinWidth() );
		minH = Max( minH, m_pBarGfx->MinHeight() );
	}


	// Add header and footer to minW/minH from slider part.

	if( m_bHorizontal )
		minW += m_footerLen + m_headerLen;
	else
		minH += m_footerLen + m_headerLen;


	// Check min w/h for forward button.

	if( m_pBtnFwdGfx && (m_btnLayout & (HEADER_FWD | FOOTER_FWD)) )
	{
		minW = Max( minW, m_pBtnFwdGfx->Width() );
		minH = Max( minH, m_pBtnFwdGfx->Height() );
	}

	// Check min w/h for backward button.

	if( m_pBtnBwdGfx && (m_btnLayout & (HEADER_BWD | FOOTER_BWD)) )
	{
		minW = Max( minW, m_pBtnBwdGfx->Width() );
		minH = Max( minH, m_pBtnBwdGfx->Height() );
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

void WgWidgetSlider::_renderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgBlock& _block )
{
		if( m_bHorizontal )
			_dest.w = _block.Width();
		else
			_dest.h = _block.Height();

		pDevice->ClipBlitBlock( _clip, _block, _dest );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;
}

//____ _onRender() ________________________________________________________

void WgWidgetSlider::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect	dest = _canvas;

	// Render header buttons

	if( m_pBtnBwdGfx && (m_btnLayout & HEADER_BWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_HEADER_BWD]) );

	if( m_pBtnFwdGfx && (m_btnLayout & HEADER_FWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_HEADER_FWD]) );

	// Render background (if any).

	if( m_bHorizontal )
		dest.w = Size().w - m_headerLen - m_footerLen;
	else
		dest.h = Size().h - m_headerLen - m_footerLen;

	if( m_pBgGfx )
		pDevice->ClipBlitBlock( _clip, m_pBgGfx->GetBlock(m_mode[C_BG]), dest );

	// Render the bar

	if( m_pBarGfx )
	{
		int barPos;
		int barLen;
		_viewToPosLen( &barPos, &barLen );

		WgRect	barDest;
		if( m_bHorizontal )
			barDest = WgRect( dest.x + barPos, dest.y, barLen, dest.h );
		else
			barDest = WgRect( dest.x, dest.y + barPos, dest.w, barLen );

		pDevice->ClipBlitBlock( _clip, m_pBarGfx->GetBlock(m_mode[C_BAR]), barDest );
	}

	// Render footer buttons

	if( m_bHorizontal )
		dest.x += dest.w;
	else
		dest.y += dest.h;

	if( m_pBtnBwdGfx && (m_btnLayout & FOOTER_BWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_FOOTER_BWD]) );

	if( m_pBtnFwdGfx && (m_btnLayout & FOOTER_FWD) )
		_renderButton( pDevice, _clip, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_FOOTER_FWD]) );
}

//____ _onAlphaTest() ______________________________________________________

bool WgWidgetSlider::_onAlphaTest( const WgCoord& ofs )
{
	if( _findMarkedComponent( ofs ) == C_NONE )
		return false;

	return true;
}

//____ _markTestButton() _______________________________________________________

bool WgWidgetSlider::_markTestButton( WgCoord ofs, WgRect& _dest, const WgBlock& _block )
{
		if( m_bHorizontal )
			_dest.w = _block.Width();
		else
			_dest.h = _block.Height();

		bool retVal = WgUtil::MarkTestBlock( ofs, _block, _dest );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;

		return retVal;
}

//____ _findMarkedComponent() __________________________________________________

WgWidgetSlider::Component WgWidgetSlider::_findMarkedComponent( WgCoord ofs )
{
	// First of all, do a mark test against the header buttons...

	WgSize	sz = Size();

	WgRect dest(0,0,sz.w,sz.h);

	if( m_pBtnBwdGfx && (m_btnLayout & HEADER_BWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_HEADER_BWD])) )
			return C_HEADER_BWD;
	}

	if( m_pBtnFwdGfx && (m_btnLayout & HEADER_FWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_HEADER_FWD])) )
			return C_HEADER_FWD;
	}

	// Then do a mark test against the footer buttons...

	if( m_bHorizontal )
		dest.x = sz.w - m_footerLen;
	else
		dest.y = sz.h - m_footerLen;

	if( m_pBtnBwdGfx && (m_btnLayout & FOOTER_BWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_FOOTER_BWD])) )
			return C_FOOTER_BWD;
	}

	if( m_pBtnFwdGfx && (m_btnLayout & FOOTER_FWD) )
	{
		if( _markTestButton( ofs, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_FOOTER_FWD])) )
			return C_FOOTER_FWD;
	}

	// Then, do a mark test against the dragbar...

	if( _markTestSlider( ofs ) == true )
		return C_BAR;

	// Finally, do a mark test against the background.

	WgRect	r(0,0,sz.w,sz.h);

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

	if( m_pBgGfx && WgUtil::MarkTestBlock( ofs, m_pBgGfx->GetBlock(m_mode[C_BG]), r ) )
		return C_BG;

	return C_NONE;
}

//____ _unmarkReqRender() ______________________________________________________

void WgWidgetSlider::_unmarkReqRender()
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_NORMAL;

	_requestRender();
}

//____ _onEvent() ______________________________________________________________

void WgWidgetSlider::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	int		barPos, barLen;
	_viewToPosLen( &barPos, &barLen );

	WgCoord pos = pEvent->PointerPos();
	int		x = pos.x;
	int		y = pos.y;

	int		pointerOfs;
	int		length;
	if( m_bHorizontal )
	{
		pointerOfs = x;
		length = Size().w;
	}
	else
	{
		pointerOfs = y;
		length = Size().h;
	}

	length -= m_headerLen + m_footerLen;
	pointerOfs -= m_headerLen;

	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
				return;

			// Just put them all to NORMAL and request render.
			// Release is followed by over before render anyway so right one will be highlighted.

			_unmarkReqRender();
			break;
		}

		case WG_EVENT_MOUSE_LEAVE:
		{
			// Turn any MARKED/SELECTED button/bg back to NORMAL.
			// Turn bar back to NORMAL only if MARKED (selected bar should remain selected).
			// Request render only if something changed (which it has unless bar was SELECTED...).

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			_unmarkReqRender();
			break;
		}

		case WG_EVENT_MOUSE_ENTER:
		case WG_EVENT_MOUSE_MOVE:
		{
			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			Component c = _findMarkedComponent(pos);

			if( c != C_NONE && m_mode[c] == WG_MODE_NORMAL )
			{
				_unmarkReqRender();
				m_mode[c] = WG_MODE_MARKED;
				if( c == C_BAR )
					m_mode[C_BG] = WG_MODE_MARKED;			// Always also mark bg if bar is marked.
			}

			break;
		}

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
				return;

			Component c = _findMarkedComponent(pos);

			_unmarkReqRender();
			m_mode[c] = WG_MODE_SELECTED;

			if( c == C_BAR )
			{
				m_dragBarPressOfs = pointerOfs - barPos;
				m_mode[C_BG] = WG_MODE_MARKED;			// Always mark bg if bar is pressed.
			}
			else if( c == C_BG )
			{
				switch( m_bgPressMode )
				{
				case SKIP_PAGE:
					if( pointerOfs - barPos < barLen/2 )
					{
						if( m_pSliderTargetWidget.GetRealPtr() != 0 )
							SetSliderPos( m_pSliderTargetInterface->_jumpBwd() );

						pHandler->QueueEvent( new WgEvent::SliderJumpBwd(this,m_sliderPos,m_sliderSize) );
					}
					else
					{
						if( m_pSliderTargetWidget.GetRealPtr() != 0 )
							SetSliderPos( m_pSliderTargetInterface->_jumpFwd() );

						pHandler->QueueEvent( new WgEvent::SliderJumpFwd(this,m_sliderPos,m_sliderSize) );
					}
					break;
				case GOTO_POS:
					m_mode[C_BAR] = WG_MODE_SELECTED;
					m_mode[C_BG] = WG_MODE_MARKED;
					m_dragBarPressOfs = barLen/2;
					SetSliderPosPxlOfs( pointerOfs );
					break;
				default:
//					assert( false );
					break;
				}

			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
			{
				if( m_pSliderTargetWidget.GetRealPtr() != 0 )
					SetSliderPos( m_pSliderTargetInterface->_stepFwd() );

				pHandler->QueueEvent( new WgEvent::SliderStepFwd(this,m_sliderPos,m_sliderSize) );
			}
			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
			{
				if( m_pSliderTargetWidget.GetRealPtr() != 0 )
					SetSliderPos( m_pSliderTargetInterface->_stepBwd() );

				pHandler->QueueEvent( new WgEvent::SliderStepBwd(this,m_sliderPos,m_sliderSize) );
			}
			break;
		}

		case WG_EVENT_MOUSEBUTTON_REPEAT:
		{
			if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
				return;

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			Component c = _findMarkedComponent(pos);

			if( c == C_BG )
			{
				if( pointerOfs - barPos < barLen/2 )
				{
					if( m_pSliderTargetWidget.GetRealPtr() != 0 )
						SetSliderPos( m_pSliderTargetInterface->_jumpBwd() );

					pHandler->QueueEvent( new WgEvent::SliderJumpBwd(this,m_sliderPos,m_sliderSize) );
				}
				else
				{
					if( m_pSliderTargetWidget.GetRealPtr() != 0 )
						SetSliderPos( m_pSliderTargetInterface->_jumpFwd() );

					pHandler->QueueEvent( new WgEvent::SliderJumpFwd(this,m_sliderPos,m_sliderSize) );
				}
			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
			{
				if( m_pSliderTargetWidget.GetRealPtr() != 0 )
					SetSliderPos( m_pSliderTargetInterface->_stepFwd() );

				pHandler->QueueEvent( new WgEvent::SliderJumpFwd(this,m_sliderPos,m_sliderSize) );
			}
			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
			{
				if( m_pSliderTargetWidget.GetRealPtr() != 0 )
					SetSliderPos( m_pSliderTargetInterface->_stepBwd() );

				pHandler->QueueEvent( new WgEvent::SliderJumpBwd(this,m_sliderPos,m_sliderSize) );
			}

			break;
		}

		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
				return;

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
			{
				float	sliderPos = 0.f;

				if( m_sliderSize < 1.f)
					sliderPos = ((float)(pointerOfs - m_dragBarPressOfs)) / (length - barLen);

				LIMIT( sliderPos, 0.f, 1.f );

				if( sliderPos != m_sliderPos )
				{
					m_sliderPos = sliderPos;

					if( m_pSliderTargetWidget.GetRealPtr() != 0 )
						m_sliderPos = m_pSliderTargetInterface->_setPosition(m_sliderPos);

					pHandler->QueueEvent( new WgEvent::SliderMove(this,m_sliderPos,m_sliderSize) );

					_requestRender();
				}
			}
			break;
		}
		
		case WG_EVENT_MOUSEWHEEL_ROLL:
		{
			const WgEvent::MouseWheelRoll * p = static_cast<const WgEvent::MouseWheelRoll*>(pEvent);

			if( p->Wheel() == 1 )
			{
				int distance = p->Distance();
				if( m_pSliderTargetWidget.GetRealPtr() != 0 )
					SetSliderPos( m_pSliderTargetInterface->_wheelRolled(distance) );
				
				pHandler->QueueEvent( new WgEvent::SliderWheelRolled(this,distance,m_sliderPos,m_sliderSize) );
			}
		}
		
        default:
            break;

	}

	// Forward event depending on rules.

	if( pEvent->IsMouseButtonEvent() )
	{
		if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
			pHandler->ForwardEvent( pEvent );
	}
	else if( pEvent->Type() != WG_EVENT_MOUSEWHEEL_ROLL )
		pHandler->ForwardEvent( pEvent );

}


//____ _markTestSlider() _______________________________________________________

bool WgWidgetSlider::_markTestSlider( WgCoord ofs )
{
	if( !m_pBarGfx )
		return false;

	int   barPos, barLen;
	_viewToPosLen( &barPos, &barLen );

	WgSize	sz = Size();

	WgRect area(0,0,sz.w,sz.h);

	if( m_bHorizontal )
	{
		area.x = barPos + m_headerLen;
		area.w = barLen;
	}
	else
	{
		area.y = barPos + m_headerLen;
		area.h = barLen;
	}

	return WgUtil::MarkTestBlock( ofs, m_pBarGfx->GetBlock(m_mode[C_BAR]), area );
}

//____ _setSlider() ____________________________________________________________

bool WgWidgetSlider::_setSlider( float _pos, float _size )
{
	LIMIT( _size, 0.0001f, 1.f );
	LIMIT( _pos, 0.f, 1.f );

	if( m_sliderPos == _pos && m_sliderSize == _size )
		return true;

	m_sliderPos		= _pos;
	m_sliderSize 	= _size;

	_requestRender();
	return	true;
}


//=============================================================================
//
//										>>> WgHSlider - functions <<<
//
//=============================================================================


//____ WgHSlider() ______________________________________________________

WgHSlider::WgHSlider( void )
{
	m_bHorizontal = true;
}


//____ WgHSlider::Type() ______________________________________________________

const char * WgHSlider::Type( void ) const
{
	return GetClass();
}

const char * WgHSlider::GetClass( void )
{
	return c_widgetTypeH;
}




//=============================================================================
//
//										>>> WgVSlider - functions <<<
//
//=============================================================================


//____ WgVSlider() ______________________________________________________

WgVSlider::WgVSlider( void )
{
	m_bHorizontal = false;
}


//____ WgVSlider::Type() ______________________________________________________

const char * WgVSlider::Type( void ) const
{
	return GetClass();
}

const char * WgVSlider::GetClass( void )
{
	return c_widgetTypeV;
}

