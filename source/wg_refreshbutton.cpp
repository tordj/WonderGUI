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



#include <wg_refreshbutton.h>
#include <wg_gfxdevice.h>
#include <wg_gfxanim.h>
#include <wg_charseq.h>
#include <wg_util.h>
#include <wg_key.h>

const char WgRefreshButton::CLASSNAME[] = {"RefreshButton"};

//____ Constructor ____________________________________________________________

WgRefreshButton::WgRefreshButton()
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

	m_refreshText._setHolder(this);

}

//____ Destructor _____________________________________________________________

WgRefreshButton::~WgRefreshButton()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgRefreshButton::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgButton::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgRefreshButton::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgRefreshButtonPtr WgRefreshButton::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgRefreshButtonPtr( static_cast<WgRefreshButton*>(pObject.GetRealPtr()) );

	return 0;
}


//_____________________________________________________________________________
void WgRefreshButton::SetRefreshAnimation( const WgGfxAnimPtr& pAnimation )
{
	m_pRefreshAnim		= pAnimation;

	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgRefreshButton::SetRefreshMode( RefreshMode mode )
{
	m_refreshMode = mode;
	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgRefreshButton::SetAnimTarget( AnimTarget target )
{
	m_animTarget = target;
	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgRefreshButton::SetRefreshText( const WgCharSeq& text )
{
	m_refreshText.Set( text );

	if( m_bRefreshing )
		_requestRender();
}

//_____________________________________________________________________________
void WgRefreshButton::SetRestartable( bool bRestartable )
{
	m_bRestartable = bRestartable;
}

//_____________________________________________________________________________
void WgRefreshButton::StartRefresh()
{
	if( m_pRefreshAnim && (!m_bRefreshing || m_bRestartable) )
	{
		m_bRefreshing = true;
		m_bStopping = false;
		m_refreshProgress = 0.f;
		m_animTimer = 0;
		m_pRefreshAnim->SetPlayMode( WG_FORWARD_LOOPING );		//UGLY! Should change once the animation system has been updated.
		_startReceiveTicks();
		_requestRender();
	}
}

//_____________________________________________________________________________
void WgRefreshButton::StopRefresh()
{
	if( m_pRefreshAnim && m_pRefreshAnim->DurationScaled())
	{
		m_bStopping = true;
		m_animTimer = m_animTimer % m_pRefreshAnim->DurationScaled();	// So it doesn't stop immmediately.
		m_pRefreshAnim->SetPlayMode( WG_FORWARD_ONCE );	//UGLY! Should change once the animation system has been updated.
	}
	else
	{
		StopRefreshNow();
	}
}

//_____________________________________________________________________________
void WgRefreshButton::StopRefreshNow()
{
	m_refreshProgress = 1.f;
	m_bRefreshing = false;
	_stopReceiveTicks();
	_requestRender();
}

//_____________________________________________________________________________
void WgRefreshButton::SetRefreshProgress( float fraction )
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

void WgRefreshButton::_onNewSize( const WgSize& size )
{
	Uint32 w = size.w;

	if( m_pSkin )
		w -= m_pSkin->ContentPadding().w;
	m_refreshText.setLineWidth(w);

	WgButton::_onNewSize( size );
}

//____ _fieldModified() ________________________________________________________

void WgRefreshButton::_fieldModified( WgTextField * pField )
{
	//TODO: Make conditional

	_requestResize();
	_requestRender();
}



//____ _onEvent() _____________________________________________________________

void WgRefreshButton::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( m_bRefreshing && m_pRefreshAnim )
			{
				if( m_refreshMode != PROGRESS )
				{
					WgTickEventPtr pTick = WgTickEvent::Cast(pEvent);

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
			WgKeyReleaseEventPtr pKeyRelease = WgKeyReleaseEvent::Cast(pEvent);

			if( m_bAutoRefresh && pKeyRelease->TranslatedKeyCode() == WG_KEY_RETURN )
				StartRefresh();

			break;
		}

		case WG_EVENT_MOUSE_RELEASE:
		{
			WgMouseReleaseEventPtr pBtnRelease = WgMouseReleaseEvent::Cast(pEvent);

			if( m_bAutoRefresh && m_bPressed && pBtnRelease->Button() == WG_BUTTON_LEFT )
				StartRefresh();

			break;
		}

		default:
			break;
	}

	WgButton::_onEvent( pEvent, pHandler );
}


//____ _onRender() _____________________________________________________________

void WgRefreshButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	// Render background or animation

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
	{
		WgGfxFrame * pAnimFrame = m_pRefreshAnim->GetFrame( m_animTimer );

		switch( m_animTarget )
		{
			case BUTTON_CENTERED:
			{
				WgRect dest = (	_canvas.x + (_canvas.w - pAnimFrame->rect.w)/2,
								_canvas.y + (_canvas.h - pAnimFrame->rect.h)/2,
								pAnimFrame->rect.Size() );

				pDevice->ClipStretchBlit( _clip, pAnimFrame->pSurf, (float) pAnimFrame->rect.x, (float) pAnimFrame->rect.y, 
										  (float) pAnimFrame->rect.w, (float) pAnimFrame->rect.h,
										  (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, false );
			}
			break;

			case BUTTON_STRETCHED:
			{
				pDevice->ClipStretchBlit( _clip, pAnimFrame->pSurf, (float) pAnimFrame->rect.x, (float) pAnimFrame->rect.y, (float) pAnimFrame->rect.w, (float) pAnimFrame->rect.h,
										  (float) _canvas.x, (float) _canvas.y, (float) _canvas.w, (float) _canvas.h, false );
			}
			break;

			default:
				break;
		}
	}
	else if( m_pSkin )
	{
		m_pSkin->Render( pDevice, _canvas, m_state, _clip );
	}

	// Get content rect with displacement.

	WgRect contentRect = _canvas;
	if( m_pSkin )
		contentRect = m_pSkin->ContentRect( _canvas, m_state );

	// Get icon and text rect from content rect

	WgSize iconSize;
	if( !icon.IsEmpty() )
		iconSize = icon.Skin()->PreferredSize();
	else if( m_animTarget == ICON && m_pRefreshAnim )
		iconSize = m_pRefreshAnim->Size();

	WgRect iconRect = icon.GetIconRect( contentRect, iconSize );
	WgRect textRect = icon.GetTextRect( contentRect, iconRect );


	// Render icon or animation

	if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
	{
		// Render animation

		WgGfxFrame * pAnimFrame = m_pRefreshAnim->GetFrame( m_animTimer );

		pDevice->ClipStretchBlit( _clip, pAnimFrame->pSurf, pAnimFrame->rect, iconRect );
	}
	else if( !icon.IsEmpty() )
		icon.Skin()->Render( pDevice, iconRect, m_state, _clip );

	// Print text

	WgTextField * pText;

	if( m_bRefreshing )
		pText = &m_refreshText;
	else
		pText = &m_text;

 	if( !pText->IsEmpty() )
	{
		pText->setState(m_state);

		if( m_pSkin )
			pText->SetColorSkin( m_pSkin );

		WgRect clip(textRect,_clip);
		pDevice->PrintText( clip, pText, textRect );
	}
}



//____ _onCloneContent() _______________________________________________________

void WgRefreshButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgRefreshButton * pOrg = (WgRefreshButton *) _pOrg;

	WgButton::_onCloneContent( _pOrg );

	m_pRefreshAnim		= pOrg->m_pRefreshAnim;
	m_animTarget		= pOrg->m_animTarget;
	m_refreshMode		= pOrg->m_refreshMode;
	m_refreshText.clone(&pOrg->m_refreshText);
	m_bRestartable		= pOrg->m_bRestartable;

	m_bRefreshing		= pOrg->m_bRefreshing;
	m_animTimer			= pOrg->m_animTimer;
	m_refreshProgress	= pOrg->m_refreshProgress;
	m_bStopping			= pOrg->m_bStopping;
}

void WgRefreshButton::SetTextManager( WgTextManager * pManager )
{
	m_refreshText.SetManager( pManager );
	m_text.SetManager(pManager);
}
