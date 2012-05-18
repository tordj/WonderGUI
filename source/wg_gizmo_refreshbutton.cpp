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

static const char	c_gizmoType[] = {"RefreshButton"};

//____ Constructor ____________________________________________________________

WgGizmoRefreshButton::WgGizmoRefreshButton()
{
	m_pRefreshAnim		= 0;
	m_animTarget		= BUTTON_CENTERED;
	m_refreshMode		= SPINNING;
	m_bAutoRefresh		= true;
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
		_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshMode( RefreshMode mode )
{
	m_refreshMode = mode;
	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetAnimTarget( AnimTarget target )
{
	m_animTarget = target;
	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshText( const WgCharSeq& text )
{
	m_refreshText.setText( text );

	// HACK!
	m_refreshText.setAlignment(m_text.alignment());

	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshTextProperties( const WgTextPropPtr& pProp )
{
	m_refreshText.setProperties(pProp);

	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRestartable( bool bRestartable )
{
	m_bRestartable = bRestartable;
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StartRefresh()
{
	if( m_pRefreshAnim && (!m_bRefreshing || m_bRestartable) )
	{
		m_bRefreshing = true;
		m_bStopping = false;
		m_refreshProgress = 0.f;
		m_animTimer = 0;
		m_pRefreshAnim->SetPlayMode( WG_FORWARD_LOOPING );		//UGLY! Should change once the animation system has been updated.
#ifdef WG_TNG
		_startReceiveTicks();
#endif
		_requestRender();
	}
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StopRefresh()
{
	if( m_pRefreshAnim && m_pRefreshAnim->DurationScaled())
	{
		m_bStopping = true;
		m_animTimer = m_animTimer % m_pRefreshAnim->DurationScaled();	// So it doesn't stop immmediately.
		m_pRefreshAnim->SetPlayMode( WG_FORWARD_ONCE );	//UGLY! Should change once the animation system has been updated.
	}
	else
	{
#ifdef WG_TNG
		StopRefreshNow();
#endif
	}
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::StopRefreshNow()
{
	m_refreshProgress = 1.f;
	m_bRefreshing = false;
#ifdef WG_TNG
	_stopReceiveTicks();
#endif
	_requestRender();
}

//_____________________________________________________________________________
void WgGizmoRefreshButton::SetRefreshProgress( float fraction )
{
	if( m_refreshMode == PROGRESS )
	{
		m_refreshProgress = fraction;

		if( m_pRefreshAnim )
		{
			WgGfxFrame * pOldFrame = m_pRefreshAnim->GetFrame( m_animTimer );
			m_animTimer = (Uint32) (fraction * m_pRefreshAnim->Duration());
			WgGfxFrame * pNewFrame = m_pRefreshAnim->GetFrame( m_animTimer );

			if( pOldFrame != pNewFrame )
				_requestRender();
		}

	}
}


//____ _onNewSize() ____________________________________________________________

void WgGizmoRefreshButton::_onNewSize( const WgSize& size )
{
	Uint32 w = size.w;

	if( m_pBgGfx )
		w -= m_pBgGfx->ContentBorders().Width();
	m_refreshText.setLineWidth(w);

	WgGizmoButton::_onNewSize( size );
}


//____ _onEvent() _____________________________________________________________

#ifdef WG_TNG
void WgGizmoRefreshButton::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( m_bRefreshing && m_pRefreshAnim )
			{
				if( m_refreshMode != PROGRESS )
				{
					const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);

					WgGfxFrame * pOldFrame = m_pRefreshAnim->GetFrame( m_animTimer );
					m_animTimer += pTick->Millisec();
					WgGfxFrame * pNewFrame = m_pRefreshAnim->GetFrame( m_animTimer );

					// RequestRender if animation has moved.

					if( pOldFrame != pNewFrame )
						_requestRender();

					// Check if animation has ended.

					if( m_bStopping && pNewFrame == m_pRefreshAnim->GetLastFrame() )		//UGLY! Change when we have updated WgAnim!
					{
						m_bRefreshing = false;
						m_bStopping = false;
						_stopReceiveTicks();
						_requestRender();
					}
				}
			}
			break;
		}

		case WG_EVENT_KEY_RELEASE:
		{
			const WgEvent::KeyRelease * pKeyRelease = static_cast<const WgEvent::KeyRelease*>(pEvent);

			if( m_bAutoRefresh && pKeyRelease->TranslatedKeyCode() == WG_KEY_RETURN )
				StartRefresh();

			break;
		}

		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			const WgEvent::MouseButtonRelease * pBtnRelease = static_cast<const WgEvent::MouseButtonRelease*>(pEvent);

			if( m_bAutoRefresh && m_bPressed && pBtnRelease->Button() == 1 )
				StartRefresh();

			break;
		}

		default:
			break;
	}

	WgGizmoButton::_onEvent( pEvent, pHandler );
}
#endif

//_____________________________________________________________________________
void WgGizmoRefreshButton::_onUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_bRefreshing && m_pRefreshAnim )
	{
		if( m_refreshMode != PROGRESS )
		{
			WgGfxFrame * pOldFrame = m_pRefreshAnim->GetFrame( m_animTimer );
			m_animTimer += _updateInfo.msDiff;
			WgGfxFrame * pNewFrame = m_pRefreshAnim->GetFrame( m_animTimer );

			// RequestRender if animation has moved.

			if( pOldFrame != pNewFrame )
				_requestRender();

			// Check if animation has ended.

			if( m_bStopping && pNewFrame == m_pRefreshAnim->GetLastFrame() )		//UGLY! Change when we have updated WgAnim!
			{
				m_bRefreshing = false;
				m_bStopping = false;
				_requestRender();
			}
		}
	}
}


//____ _onRender() _____________________________________________________________

void WgGizmoRefreshButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render background or animation

	WgBlock	bgBlock;
	if( m_pBgGfx )
		bgBlock = m_pBgGfx->GetBlock(m_mode,_canvas.Size());

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
	{
		WgBlock animBlock = m_pRefreshAnim->GetBlock( m_animTimer );

		switch( m_animTarget )
		{
			case BUTTON_CENTERED:
			{
				WgRect dest = (	_canvas.x + (_canvas.w - animBlock.Width())/2,
								_canvas.y + (_canvas.h - animBlock.Height())/2,
								animBlock.Size() );

				pDevice->ClipBlitBlock( _clip, animBlock, dest );
			}
			break;

			case BUTTON_STRETCHED:
			{
				pDevice->ClipBlitBlock( _clip, animBlock, _canvas );
			}
			break;

			default:
				break;
		}
	}
	else
	{
		pDevice->ClipBlitBlock( _clip, bgBlock, _canvas );
	}

	// Get content rect with displacement.

	WgRect contentRect = bgBlock.ContentRect( _canvas );

	// Get icon and text rect from content rect

	WgSize iconSize;
	if( m_pIconGfx )
		iconSize = m_pIconGfx->Size();
	else if( m_animTarget == ICON && m_pRefreshAnim )
		iconSize = m_pRefreshAnim->Size();

	WgRect iconRect = _getIconRect( contentRect, iconSize );
	WgRect textRect = _getTextRect( contentRect, iconRect );


	// Render icon or animation

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
	{
		// Render animation

		WgBlock animBlock = m_pRefreshAnim->GetBlock( m_animTimer );

		pDevice->ClipBlitBlock( _clip, animBlock, iconRect );
	}
	else if( m_pIconGfx )
		pDevice->ClipBlitBlock( _clip, m_pIconGfx->GetBlock(m_mode, iconRect.Size()), iconRect );

	// Print text

	WgText * pText;

	if( m_bRefreshing )
		pText = &m_refreshText;
	else
		pText = &m_text;

 	if( !pText->IsEmpty() )
	{
		pText->setMode(m_mode);

		if( m_pBgGfx )
			pText->SetBgBlockColors( m_pBgGfx->TextColors() );

		WgRect clip(textRect,_clip);
		pDevice->PrintText( clip, pText, textRect );
	}
}

//____ _onAction() _____________________________________________________________

void WgGizmoRefreshButton::_onAction( WgInput::UserAction action, int button, const WgActionDetails& info, const WgInput& inputObj )
{
	switch( action )
	{
		case	WgInput::KEY_RELEASE:
		{
			if( m_bAutoRefresh && button == WG_KEY_RETURN )
				StartRefresh();

			break;
		}

		case WgInput::BUTTON_RELEASE:

			if( m_bAutoRefresh && button == 1 && m_bPressed )
				StartRefresh();
        default:
            break;
	}

	WgGizmoButton::_onAction( action, button, info, inputObj );
}

//_____ _getRenderMode() ________________________________________________________

WgMode WgGizmoRefreshButton::_getRenderMode()
{
	if( !IsEnabled() )
		return WG_MODE_DISABLED;

	if( m_bRefreshing && !m_bRestartable )
		return WG_MODE_SPECIAL;					// Not restartable, so we shouldn't show any MARKED/SELECTED modes.


	if( m_bReturnPressed || (m_bPressed && (m_bPointerInside || m_bDownOutside)) )
		return WG_MODE_SELECTED;

	if( m_bPointerInside )
		return WG_MODE_MARKED;

	if( m_bRefreshing )
		return WG_MODE_SPECIAL;
	else
		return WG_MODE_NORMAL;
}


//____ _onCloneContent() _______________________________________________________

void WgGizmoRefreshButton::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoRefreshButton * pOrg = (WgGizmoRefreshButton *) _pOrg;

	WgGizmoButton::_onCloneContent( _pOrg );

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

void WgGizmoRefreshButton::SetTextManager( WgTextManager * pManager )
{
	m_refreshText.setManager( pManager );
	WgGizmoButton::SetTextManager(pManager);
}
