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



#include <wg_gizmo_refreshbutton.h>
#include <wg_gfxdevice.h>
#include <wg_gfxanim.h>
#include <wg_charseq.h>
#include <wg_util.h>
#include <wg_key.h>

static const char	c_gizmoType[] = {"TordJ/RefreshButton"};

//____ Constructor ____________________________________________________________

WgGizmoRefreshButton::WgGizmoRefreshButton()
{
	m_pRefreshAnim		= 0;
	m_animTarget		= BUTTON_CENTERED;
	m_refreshMode		= SPINNING;
	m_bRestartable		= false;

	m_bRefreshing		= false;
	m_animTimer			= 0;
	m_refreshProgress	= 0.f;
	m_bStopping			= false;

}

//____ Destructor _____________________________________________________________

WgGizmoRefreshButton::~WgGizmoRefreshButton()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoRefreshButton::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoRefreshButton::GetMyType()
{
	return c_gizmoType;
}


//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshAnimation( WgGfxAnim * pAnimation )
{
	m_pRefreshAnim		= pAnimation;

	if( m_bRefreshing )
		RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshMode( RefreshMode mode )
{
	m_refreshMode = mode;
	if( m_bRefreshing )
		RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetAnimTarget( AnimTarget target )
{
	m_animTarget = target;
	if( m_bRefreshing )
		RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshText( const WgCharSeq& text )
{
	//TODO: Change once WgText support WgCharSeq correctly.

	WgCharSeq::WgCharBasket basket = text.GetWgChars();
	m_refreshText.addText( basket.ptr, basket.length );

	if( m_bRefreshing )
		RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshTextProperties( const WgTextPropPtr& pProp )
{
	m_refreshText.setDefaultProperties(pProp);

	if( m_bRefreshing )
		RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRestartable( bool bRestartable )
{
	m_bRestartable = bRestartable;
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StartRefresh()
{
	if( !m_bRefreshing || m_bRestartable )
	{
		m_bRefreshing = true;
		m_bStopping = false;
		m_refreshProgress = 0.f;
		m_animTimer = 0;
		m_pRefreshAnim->setPlayMode( WgAnim::FORWARD_LOOPING );		//UGLY! Should change once the animation system has been updated.

		RequestRender();
	}
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StopRefresh()
{
	if( m_pRefreshAnim )
	{
		m_bStopping = true;
		m_animTimer = m_animTimer % m_pRefreshAnim->durationScaled();	// So it doesn't stop immmediately.
		m_pRefreshAnim->setPlayMode( WgAnim::FORWARD_ONCE );	//UGLY! Should change once the animation system has been updated.
	}
	else
		StopRefreshNow();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StopRefreshNow()
{
	m_refreshProgress = 1.f;
	m_bRefreshing = false;
	RequestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshProgress( float fraction )
{
	if( m_refreshMode == PROGRESS )
	{
		m_refreshProgress = fraction;

		if( m_pRefreshAnim )
		{
			WgGfxFrame * pOldFrame = m_pRefreshAnim->getFrame( m_animTimer );
			m_animTimer = (Uint32) (fraction * m_pRefreshAnim->duration());
			WgGfxFrame * pNewFrame = m_pRefreshAnim->getFrame( m_animTimer );

			if( pOldFrame != pNewFrame )
				RequestRender();
		}

	}
}


//____ OnNewSize() ____________________________________________________________

void WgGizmoRefreshButton::OnNewSize( const WgSize& size )
{
	Uint32 w = size.w;

	if( m_pBgGfx )
		w -= m_pBgGfx->GetContentBorders().GetWidth();
	m_refreshText.setLineWidth(w);

	WgGizmoButton::OnNewSize( size );
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_bRefreshing && m_pRefreshAnim )
	{
		if( m_refreshMode != PROGRESS )
		{
			WgGfxFrame * pOldFrame = m_pRefreshAnim->getFrame( m_animTimer );
			m_animTimer += _updateInfo.msDiff;
			WgGfxFrame * pNewFrame = m_pRefreshAnim->getFrame( m_animTimer );

			// RequestRender if animation has moved.

			if( pOldFrame != pNewFrame )
				RequestRender();

			// Check if animation has ended.

			if( m_bStopping && pNewFrame == m_pRefreshAnim->getLastFrame() )		//UGLY! Change when we have updated WgAnim!
			{
				m_bRefreshing = false;
				m_bStopping = false;
				RequestRender();
			}
		}
	}
}


//____ OnRender() _____________________________________________________________

void WgGizmoRefreshButton::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render background

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
	{
		WgGfxFrame * pFrame = m_pRefreshAnim->getFrame( m_animTimer );
		WgRect src( pFrame->ofs.x, pFrame->ofs.y, m_pRefreshAnim->width(), m_pRefreshAnim->height() );

		switch( m_animTarget )
		{
			case BUTTON_CENTERED:
			{
				int dx = (_window.w - src.w)/2;
				int dy = (_window.h - src.h)/2;

				pDevice->ClipBlit( _clip, pFrame->pSurf, src, _window.x + dx, _window.y + dy );
			}
			break;

			case BUTTON_STRETCHED:
			{
				pDevice->ClipStretchBlit( _clip, pFrame->pSurf, src, _window );
			}
			break;
		}

		// Render animation
	}
	else if( m_pBgGfx )
		pDevice->ClipBlitBlock( _clip, m_pBgGfx->GetBlock(m_mode), _window );

	// Get displacement offset

    int   xOfs = m_aDisplace[m_mode].x;
    int   yOfs = m_aDisplace[m_mode].y;

	// Render icon (with displacement).

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
	{
		// Render animation

		int w = m_pRefreshAnim->width();;
		int h = m_pRefreshAnim->height();

		int dx = (int)( m_iconOrigo.anchorX() * _window.w - m_iconOrigo.hotspotX() * w + m_iconOfs.x + xOfs );
		int dy = (int)( m_iconOrigo.anchorY() * _window.h - m_iconOrigo.hotspotY() * h + m_iconOfs.y + yOfs );

		WgGfxFrame * pFrame = m_pRefreshAnim->getFrame( m_animTimer );
		pDevice->ClipBlit( _clip, pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, w, h ), _window.x + dx, _window.y + dy );
	}
	else if( m_pIconGfx )
	{
		int w = m_pIconGfx->GetWidth();
		int h = m_pIconGfx->GetHeight();

		int dx = (int)( m_iconOrigo.anchorX() * _window.w - m_iconOrigo.hotspotX() * w + m_iconOfs.x + xOfs );
		int dy = (int)( m_iconOrigo.anchorY() * _window.h - m_iconOrigo.hotspotY() * h + m_iconOfs.y + yOfs );

		WgRect dest( _window.x + dx, _window.y + dy, w, h );
		pDevice->ClipBlitBlock( _clip, m_pIconGfx->GetBlock(m_mode), dest );
	}

	// Print text (with displacement).

	WgText * pText;
	
	if( m_bRefreshing )
		pText = &m_refreshText;
	else
		pText = &m_text;

 	if( pText->nbLines()!= 0 )
	{
		pText->setMode(m_mode);

		WgRect printWindow( _window.x + xOfs, _window.y + yOfs, _window.w, _window.h );
		if( m_pBgGfx )
			printWindow.Shrink( m_pBgGfx->GetContentBorders() );
		pDevice->ClipPrintText( _clip, pText, printWindow );
	}
}

//____ OnAction() _____________________________________________________________

void WgGizmoRefreshButton::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button, const WgActionDetails& info, const WgInput& inputObj )
{
	switch( action )
	{
		case	WgInput::KEY_RELEASE:
		{
			if( button == WGKEY_RETURN )
				StartRefresh();
				
			break;
		}

		case WgInput::BUTTON_RELEASE:

			if( m_bPressedInside[button-1] == true )
				StartRefresh();
        default:
            break;
	}

	WgGizmoButton::OnAction( pEmitter, action, button, info, inputObj );
}

//_____ GetRenderMode() ________________________________________________________

WgMode WgGizmoRefreshButton::GetRenderMode()
{
	if( !IsEnabled() )
		return WG_MODE_DISABLED;

	if( m_bRefreshing && !m_bRestartable )
		return WG_MODE_SPECIAL;					// Not restartable, so we shouldn't show any MARKED/SELECTED modes.

	if( m_bReturnPressed )
		return WG_MODE_SELECTED;

	if( m_bPointerInside || m_bDownOutside )
	{
		for( int i = 0 ; i < WG_MAX_BUTTONS ; i++ )
		{
			if( m_bRenderDown[i] && m_bPressedInside[i] )
				return WG_MODE_SELECTED;
		}
	}

	if( m_bPointerInside )
		return WG_MODE_MARKED;

	if( m_bRefreshing )
		return WG_MODE_SPECIAL;
	else
		return WG_MODE_NORMAL;
}


//____ OnCloneContent() _______________________________________________________

void WgGizmoRefreshButton::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoRefreshButton * pOrg = (WgGizmoRefreshButton *) _pOrg;

	WgGizmoButton::OnCloneContent( _pOrg );

	m_pRefreshAnim		= pOrg->m_pRefreshAnim;
	m_animTarget		= pOrg->m_animTarget;
	m_refreshMode		= pOrg->m_refreshMode;
	m_refreshText.setText(&pOrg->m_refreshText);
	m_bRestartable		= pOrg->m_bRestartable;

	m_bRefreshing		= pOrg->m_bRefreshing;
	m_animTimer			= pOrg->m_animTimer;
	m_refreshProgress	= pOrg->m_refreshProgress;
	m_bStopping			= pOrg->m_bStopping;	
}
