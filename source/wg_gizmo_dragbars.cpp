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
#include <wg_gizmo_dragbars.h>
#include <wg_signals.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

using namespace WgSignal;
using namespace WgUtil;

static const char	Wdg_Type[] = {"Unspecified type derived from TordJ/DragBar"};
static const char Wdg_TypeH[] = {"TordJ/HDrag"};
static const char Wdg_TypeV[] = {"TordJ/VDrag"};


//____ WgGizmoDragbar() ____________________________________________________

WgGizmoDragbar::WgGizmoDragbar()
{
	m_sliderSize 		= 1.0;
	m_sliderPos 		= 0.0;

	m_bPressOnDragBar	= false;
	m_dragBarPressOfs	= 0;

	m_btnLayout			= DEFAULT;
	m_headerLen			= 0;
	m_footerLen			= 0;

	m_lastCursorDownPos = WgCord(-4096, -4096);

	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS; i++ )
		m_mode[i] = WG_MODE_NORMAL;
}

//____ ~WgGizmoDragbar() _________________________________________________________

WgGizmoDragbar::~WgGizmoDragbar( void )
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoDragbar::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoDragbar::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetSlider() ____________________________________________________________

bool WgGizmoDragbar::SetSlider( float _pos, float _size )
{
	LIMIT( _size, 0.0001f, 1.f );
	LIMIT( _pos, 0.f, 1.f );

	if( m_sliderPos == _pos && m_sliderSize == _size )
		return true;

	m_sliderPos		= _pos;
	m_sliderSize 	= _size;

	if( m_pHook )
		m_pHook->GetEmitter()->Emit( SliderPos(), m_sliderPos );

	RequestRender();
	return	true;
}

//____ SetSliderPos() _________________________________________________________

bool WgGizmoDragbar::SetSliderPos( float pos )
{
	LIMIT( pos, 0.f, 1.f );

	if( pos > m_sliderPos-0.000001 && pos < m_sliderPos+0.000001 )
		return true;

	m_sliderPos = pos;

	if( m_pHook )
		m_pHook->GetEmitter()->Emit( SliderPos(), m_sliderPos );

	RequestRender();
	return	true;
}

bool WgGizmoDragbar::SetSliderPosPxlOfs( int x )
{
	int		barPos, barLen;
	ViewToPosLen( &barPos, &barLen );

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

bool WgGizmoDragbar::SetSliderSize( float _size )
{
	LIMIT( _size, 0.0001f, 1.f );

	if( _size == m_sliderSize )
		return true;

	m_sliderSize = _size;

	RequestRender();
	return	true;
}


//____ SetSource() ____________________________________________________________

bool WgGizmoDragbar::SetSource( WgBlockSetPtr pBgGfx, WgBlockSetPtr pBarGfx,
							 WgBlockSetPtr pBtnBwdGfx, WgBlockSetPtr pBtnFwdGfx )
{
	m_pBgGfx		= pBgGfx;
	m_pBarGfx		= pBarGfx;
	m_pBtnFwdGfx	= pBtnFwdGfx;
	m_pBtnBwdGfx	= pBtnBwdGfx;

	HeaderFooterChanged();
	UpdateMinSize();
	RequestRender();
	return true;
}

//____ SetButtonLayout() ______________________________________________________

bool WgGizmoDragbar::SetButtonLayout(  ButtonLayout layout )
{
	m_btnLayout = layout;

	HeaderFooterChanged();
	return true;
}

//____ HeaderFooterChanged() _______________________________________________

void WgGizmoDragbar::HeaderFooterChanged()
{
	int	fwdLen = 0, bwdLen = 0;

	if( m_bHorizontal )
	{
		if( m_pBtnFwdGfx )
			fwdLen = m_pBtnFwdGfx->GetWidth();
		if( m_pBtnBwdGfx )
			bwdLen = m_pBtnBwdGfx->GetWidth();
	}
	else
	{
		if( m_pBtnFwdGfx )
			fwdLen = m_pBtnFwdGfx->GetHeight();
		if( m_pBtnBwdGfx )
			bwdLen = m_pBtnBwdGfx->GetHeight();
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

		UpdateMinSize();
	}

	RequestRender();
}



//____ OnCloneContent() _______________________________________________________

void WgGizmoDragbar::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoDragbar * pOrg = (WgGizmoDragbar *) _pOrg;

	m_pBgGfx			= pOrg->m_pBgGfx;
	m_pBarGfx			= pOrg->m_pBarGfx;
	m_pBtnFwdGfx		= pOrg->m_pBtnFwdGfx;
	m_pBtnBwdGfx		= pOrg->m_pBtnBwdGfx;

  	m_sliderPos			= pOrg->m_sliderPos;
	m_sliderSize		= pOrg->m_sliderSize;

	m_bHorizontal		= pOrg->m_bHorizontal;

	m_bPressOnDragBar	= 0;
	m_dragBarPressOfs	= 0;
  	
	m_btnLayout			= pOrg->m_btnLayout;
	m_headerLen			= pOrg->m_headerLen;
	m_footerLen			= pOrg->m_footerLen;
}

//____ ViewToPosLen() _________________________________________________________

void	WgGizmoDragbar::ViewToPosLen( int * _wpPos, int * _wpLen )
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
		minLen = m_pBarGfx->GetMinWidth();
	else
		minLen = m_pBarGfx->GetMinHeight();

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

//____ OnEnable() ___________________________________________________

void WgGizmoDragbar::OnEnable( void )
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_NORMAL;

	RequestRender();
}

//____ OnDisable() ___________________________________________________

void WgGizmoDragbar::OnDisable( void )
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_DISABLED;

	RequestRender();
}

//____ OnRefresh() _______________________________________________________

void WgGizmoDragbar::OnRefresh( void )
{
	RequestRender();
}


//____ MinSize() ___________________________________________________________

 WgSize WgGizmoDragbar::MinSize() const
{
	return m_minSize;
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmoDragbar::BestSize() const
{
	WgSize sz = m_minSize;

	// Add 50 pixels in the sliders direction for best size.

	if( m_bHorizontal )
		sz.w += 50;
	else
		sz.h += 50;

	return sz;
}


//____ UpdateMinSize() ________________________________________________________

void WgGizmoDragbar::UpdateMinSize()
{
	Sint32	minW = 4;
	Sint32	minH = 4;

	// Check min w/h for BgGfx.

	if( m_pBgGfx )
	{
		minW = Max( minW, m_pBgGfx->GetMinWidth() );
		minH = Max( minH, m_pBgGfx->GetMinHeight() );
	}

	// Check min w/h for BarGfx.

	if( m_pBarGfx )
	{
		minW = Max( minW, m_pBarGfx->GetMinWidth() );
		minH = Max( minH, m_pBarGfx->GetMinHeight() );
	}


	// Add header and footer to minW/minH from slider part.

	if( m_bHorizontal )
		minW += m_footerLen + m_headerLen;
	else
		minH += m_footerLen + m_headerLen;


	// Check min w/h for forward button.

	if( m_pBtnFwdGfx && (m_btnLayout & (HEADER_FWD | FOOTER_FWD)) )
	{
		minW = Max( minW, m_pBtnFwdGfx->GetWidth() );
		minH = Max( minH, m_pBtnFwdGfx->GetHeight() );
	}

	// Check min w/h for backward button.

	if( m_pBtnBwdGfx && (m_btnLayout & (HEADER_BWD | FOOTER_BWD)) )
	{
		minW = Max( minW, m_pBtnBwdGfx->GetWidth() );
		minH = Max( minH, m_pBtnBwdGfx->GetHeight() );
	}

	// Set if changed.

	if( minW != m_minSize.w || minH != m_minSize.h )
	{
		m_minSize.w = minW;
		m_minSize.h = minH;

		RequestResize();
	}
}

//____ RenderButton() _________________________________________________________

void WgGizmoDragbar::RenderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgBlock& _block )
{
		if( m_bHorizontal )
			_dest.w = _block.GetWidth();
		else
			_dest.h = _block.GetHeight();

		pDevice->ClipBlitBlock( _clip, _block, _dest );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;
}

//____ OnRender() ________________________________________________________

void WgGizmoDragbar::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect	dest = _window;

	// Render header buttons

	if( m_pBtnBwdGfx && (m_btnLayout & HEADER_BWD) )
		RenderButton( pDevice, _clip, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_HEADER_BWD]) );

	if( m_pBtnFwdGfx && (m_btnLayout & HEADER_FWD) )
		RenderButton( pDevice, _clip, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_HEADER_FWD]) );

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
		ViewToPosLen( &barPos, &barLen );

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
		RenderButton( pDevice, _clip, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_FOOTER_BWD]) );

	if( m_pBtnFwdGfx && (m_btnLayout & FOOTER_FWD) )
		RenderButton( pDevice, _clip, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_FOOTER_FWD]) );
}

//____ OnMarkTest() ______________________________________________________

bool WgGizmoDragbar::OnMarkTest( const WgCord& ofs )
{
	if( FindMarkedComponent( ofs.x, ofs.y) == C_NONE )
		return false;

	return true;
}

//____ MarkTestButton() _______________________________________________________

bool WgGizmoDragbar::MarkTestButton( int _x, int _y, WgRect& _dest, const WgBlock& _block )
{
		if( m_bHorizontal )
			_dest.w = _block.GetWidth();
		else
			_dest.h = _block.GetHeight();

		bool retVal = WgUtil::MarkTestBlock( _x, _y, _block, _dest );

		if( m_bHorizontal )
			_dest.x += _dest.w;
		else
			_dest.y += _dest.h;

		return retVal;
}

//____ FindMarkedComponent() __________________________________________________

WgGizmoDragbar::Component WgGizmoDragbar::FindMarkedComponent( int _x, int _y )
{
	// First of all, do a mark test against the header buttons...

	WgSize	sz = Size();

	WgRect dest(0,0,sz.w,sz.h);

	if( m_pBtnBwdGfx && (m_btnLayout & HEADER_BWD) )
	{
		if( MarkTestButton(_x,_y, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_HEADER_BWD])) )
			return C_HEADER_BWD;
	}

	if( m_pBtnFwdGfx && (m_btnLayout & HEADER_FWD) )
	{
		if( MarkTestButton(_x,_y, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_HEADER_FWD])) )
			return C_HEADER_FWD;
	}

	// Then do a mark test against the footer buttons...

	if( m_bHorizontal )
		dest.x = sz.w - m_footerLen;
	else
		dest.y = sz.h - m_footerLen;

	if( m_pBtnBwdGfx && (m_btnLayout & FOOTER_BWD) )
	{
		if( MarkTestButton(_x,_y, dest, m_pBtnBwdGfx->GetBlock(m_mode[C_FOOTER_BWD])) )
			return C_FOOTER_BWD;
	}

	if( m_pBtnFwdGfx && (m_btnLayout & FOOTER_FWD) )
	{
		if( MarkTestButton(_x,_y, dest, m_pBtnFwdGfx->GetBlock(m_mode[C_FOOTER_FWD])) )
			return C_FOOTER_FWD;
	}

	// Then, do a mark test against the dragbar...

	if( MarkTestSlider(_x, _y) == true )
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

	if( m_pBgGfx && WgUtil::MarkTestBlock( _x, _y, m_pBgGfx->GetBlock(m_mode[C_BG]), r ) )
		return C_BG;

	return C_NONE;
}

//____ UnmarkReqRender() ______________________________________________________

void WgGizmoDragbar::UnmarkReqRender()
{
	for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
		m_mode[i] = WG_MODE_NORMAL;

	RequestRender();
}


//____ OnAction() _________________________________________________

void WgGizmoDragbar::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	int		barPos, barLen;
	ViewToPosLen( &barPos, &barLen );

	WgCord pos = Abs2local( WgCord(info.x, info.y) );
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

	switch( action )
	{
		case WgInput::BUTTON_RELEASE:
		case WgInput::BUTTON_RELEASE_OUTSIDE:
		{
			if( button_key != 1 )
				return;

			// Just put them all to NORMAL and request render.
			// Release is followed by over before render anyway so right one will be highlighted.

			UnmarkReqRender();
			break;
		}

		case WgInput::POINTER_EXIT:
		{
			// Turn any MARKED/SELECTED button/bg back to NORMAL.
			// Turn bar back to NORMAL only if MARKED (selected bar should remain selected).
			// Request render only if something changed (which it has unless bar was SELECTED...).

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			UnmarkReqRender();
			break;
		}

		case WgInput::POINTER_ENTER:
		case WgInput::POINTER_OVER:
		{
			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			Component c = FindMarkedComponent(x,y);

			if( c != C_NONE && m_mode[c] == WG_MODE_NORMAL )
			{
				UnmarkReqRender();
				m_mode[c] = WG_MODE_MARKED;
			}

			break;
		}

		case WgInput::BUTTON_PRESS:
		{
			if( button_key != 1 )
				return;

			Component c = FindMarkedComponent(x,y);

			UnmarkReqRender();
			m_mode[c] = WG_MODE_SELECTED;


			if( c == C_BAR )
	  			m_dragBarPressOfs = pointerOfs - barPos;
			else if( c == C_BG )
			{
				if( pointerOfs - barPos < barLen/2 )
					pEmitter->Emit( PrevPage() );
				else
					pEmitter->Emit( NextPage() );
			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
				pEmitter->Emit( Forward() );
			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
				pEmitter->Emit( Back() );

			break;
		}

		case WgInput::BUTTON_REPEAT:
		{
			if( button_key != 1 )
				return;

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
				return;

			Component c = FindMarkedComponent(x,y);

			if( c == C_BG )
			{
				if( pointerOfs - barPos < barLen/2 )
					pEmitter->Emit( PrevPage() );
				else
					pEmitter->Emit( NextPage() );
			}
			else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
				pEmitter->Emit( Forward() );

			else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
				pEmitter->Emit( Back() );

			break;
		}

		
		case WgInput::BUTTON_DOWN:
		{
			if( button_key != 1 )
				return;

			if( m_mode[C_BAR] == WG_MODE_SELECTED )
			{
				// Don't update the scroll position unless the cursor was moved
				// This prevents views from scrolling if content is added while the dragbar is pressed
				// Martin
				WgCord cursorPos = WgCord(info.x, info.y);
				if(cursorPos != m_lastCursorDownPos)
				{
					m_lastCursorDownPos = cursorPos;

					float	sliderPos = 0.f;

					if( m_sliderSize < 1.f)
						sliderPos = ((float)(pointerOfs - m_dragBarPressOfs)) / (length - barLen);

					LIMIT( sliderPos, 0.f, 1.f );

  					if( sliderPos != m_sliderPos )
  					{
  						m_sliderPos = sliderPos;
						RequestRender();
 						pEmitter->Emit( SliderPos(), m_sliderPos );
					}
				}
			}
			break;
		}
        default:
            break;

	}
}

//____ MarkTestSlider() _______________________________________________________

bool WgGizmoDragbar::MarkTestSlider( int _x, int _y )
{
	if( !m_pBarGfx )
		return false;

	int   barPos, barLen;
	ViewToPosLen( &barPos, &barLen );

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

	return WgUtil::MarkTestBlock( _x, _y, m_pBarGfx->GetBlock(m_mode[C_BAR]), area );
}




//=============================================================================
//
//										>>> WgGizmoHDragbar - functions <<<
//
//=============================================================================


//____ WgGizmoHDragbar() ______________________________________________________

WgGizmoHDragbar::WgGizmoHDragbar( void )
{
	m_bHorizontal = true;
}


//____ WgGizmoHDragbar::Type() ______________________________________________________

const char * WgGizmoHDragbar::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoHDragbar::GetMyType( void )
{
	return Wdg_TypeH;
}




//=============================================================================
//
//										>>> WgGizmoVDragbar - functions <<<
//
//=============================================================================


//____ WgGizmoVDragbar() ______________________________________________________

WgGizmoVDragbar::WgGizmoVDragbar( void )
{
	m_bHorizontal = false;
}


//____ WgGizmoVDragbar::Type() ______________________________________________________

const char * WgGizmoVDragbar::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoVDragbar::GetMyType( void )
{
	return Wdg_TypeV;
}

