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
#include <wg_msg.h>

namespace wg 
{
	
	const char WgRefreshButton::CLASSNAME[] = {"RefreshButton"};
	
	//____ Constructor ____________________________________________________________
	
	WgRefreshButton::WgRefreshButton() : m_refreshText(this), refreshText(&m_refreshText)
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
		m_tickRouteId		= 0;
	}
	
	//____ Destructor _____________________________________________________________
	
	WgRefreshButton::~WgRefreshButton()
	{
		if( m_tickRouteId )
			WgBase::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgRefreshButton::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgButton::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgRefreshButton::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgRefreshButton_p WgRefreshButton::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgRefreshButton_p( static_cast<WgRefreshButton*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//_____________________________________________________________________________
	void WgRefreshButton::setRefreshAnimation( const WgGfxAnim_p& pAnimation )
	{
		m_pRefreshAnim		= pAnimation;
	
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::setRefreshMode( RefreshMode mode )
	{
		m_refreshMode = mode;
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::setAnimTarget( AnimTarget target )
	{
		m_animTarget = target;
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::setRestartable( bool bRestartable )
	{
		m_bRestartable = bRestartable;
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::startRefresh()
	{
		if( m_pRefreshAnim && (!m_bRefreshing || m_bRestartable) )
		{
			m_bRefreshing = true;
			m_bStopping = false;
			m_refreshProgress = 0.f;
			m_animTimer = 0;
			m_pRefreshAnim->setPlayMode( WG_FORWARD_LOOPING );		//UGLY! Should change once the animation system has been updated.
			m_tickRouteId = WgBase::msgRouter()->addRoute( WG_MSG_TICK, this );
			_requestRender();
		}
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::stopRefresh()
	{
		if( m_pRefreshAnim && m_pRefreshAnim->durationScaled())
		{
			m_bStopping = true;
			m_animTimer = m_animTimer % m_pRefreshAnim->durationScaled();	// So it doesn't stop immmediately.
			m_pRefreshAnim->setPlayMode( WG_FORWARD_ONCE );	//UGLY! Should change once the animation system has been updated.
		}
		else
		{
			stopRefreshNow();
		}
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::stopRefreshNow()
	{
		m_refreshProgress = 1.f;
		m_bRefreshing = false;
		WgBase::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
		_requestRender();
	}
	
	//_____________________________________________________________________________
	void WgRefreshButton::setRefreshProgress( float fraction )
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
					_requestRender();
			}
	
		}
	}
	
	
	//____ _onNewSize() ____________________________________________________________
	
	void WgRefreshButton::_onNewSize( const WgSize& size )
	{
		Uint32 w = size.w;
	
		WgSize contentSize = m_pSkin ? size - m_pSkin->contentPadding() : size;
		m_refreshText.onNewSize(contentSize);
	
		WgButton::_onNewSize( size );
	}
	
	
	//____ _onMsg() _____________________________________________________________
	
	void WgRefreshButton::_onMsg( const WgMsg_p& pMsg )
	{
		WgButton::_onMsg( pMsg );
	
		switch( pMsg->type() )
		{
			case WG_MSG_TICK:
			{
				if( m_bRefreshing && m_pRefreshAnim )
				{
					if( m_refreshMode != PROGRESS )
					{
						WgTickMsg_p pTick = WgTickMsg::cast(pMsg);
	
						WgGfxFrame * pOldFrame = m_pRefreshAnim->getFrame( m_animTimer );
						m_animTimer += pTick->millisec();
						WgGfxFrame * pNewFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
						// RequestRender if animation has moved.
	
						if( pOldFrame != pNewFrame )
							_requestRender();
	
						// Check if animation has ended.
	
						if( m_bStopping && pNewFrame == m_pRefreshAnim->getLastFrame() )		//UGLY! Change when we have updated WgAnim!
						{
							m_bRefreshing = false;
							m_bStopping = false;
							WgBase::msgRouter()->deleteRoute( m_tickRouteId );
							m_tickRouteId = 0;
							_requestRender();
						}
					}
				}
				break;
			}
	
			case WG_MSG_KEY_RELEASE:
			{
				WgKeyReleaseMsg_p pKeyRelease = WgKeyReleaseMsg::cast(pMsg);
	
				if( m_bAutoRefresh && pKeyRelease->translatedKeyCode() == WG_KEY_RETURN )
					startRefresh();
	
				break;
			}
	
			case WG_MSG_MOUSE_RELEASE:
			{
				WgMouseReleaseMsg_p pBtnRelease = WgMouseReleaseMsg::cast(pMsg);
	
				if( m_bAutoRefresh && m_bPressed && pBtnRelease->button() == WG_BUTTON_LEFT )
					startRefresh();
	
				break;
			}
	
			default:
				break;
		}
	}
	
	
	//____ _onRender() _____________________________________________________________
	
	void WgRefreshButton::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		// Render background or animation
	
		if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
		{
			WgGfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
			switch( m_animTarget )
			{
				case BUTTON_CENTERED:
				{
					WgRect dest = (	_canvas.x + (_canvas.w - pAnimFrame->rect.w)/2,
									_canvas.y + (_canvas.h - pAnimFrame->rect.h)/2,
									pAnimFrame->rect.size() );
	
					pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, (float) pAnimFrame->rect.x, (float) pAnimFrame->rect.y, 
											  (float) pAnimFrame->rect.w, (float) pAnimFrame->rect.h,
											  (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, false );
				}
				break;
	
				case BUTTON_STRETCHED:
				{
					pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, (float) pAnimFrame->rect.x, (float) pAnimFrame->rect.y, (float) pAnimFrame->rect.w, (float) pAnimFrame->rect.h,
											  (float) _canvas.x, (float) _canvas.y, (float) _canvas.w, (float) _canvas.h, false );
				}
				break;
	
				default:
					break;
			}
		}
		else if( m_pSkin )
		{
			m_pSkin->render( pDevice, _canvas, m_state, _clip );
		}
	
		// Get content rect with displacement.
	
		WgRect contentRect = _canvas;
		if( m_pSkin )
			contentRect = m_pSkin->contentRect( _canvas, m_state );
	
		// Get icon and text rect from content rect
	
		WgSize iconSize;
		if( !m_icon.isEmpty() )
			iconSize = m_icon.skin()->preferredSize();
		else if( m_animTarget == ICON && m_pRefreshAnim )
			iconSize = m_pRefreshAnim->size();
	
		WgRect iconRect = m_icon.getIconRect( contentRect, iconSize );
		WgRect textRect = m_icon.getTextRect( contentRect, iconRect );
	
	
		// Render icon or animation
	
		if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
		{
			// Render animation
	
			WgGfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
			pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, pAnimFrame->rect, iconRect );
		}
		else if( !m_icon.isEmpty() )
			m_icon.skin()->render( pDevice, iconRect, m_state, _clip );
	
		// Print text
	
		WgTextField * pText;
	
		if( m_bRefreshing )
			pText = &m_refreshText;
		else
			pText = &m_text;
	
	 	if( !pText->isEmpty() )
		{
			pText->setState(m_state);		//TODO: Should be done when state actually is set.
	
			WgRect clip(textRect,_clip);
			pText->onRender(pDevice, textRect, clip );
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
		m_bRestartable		= pOrg->m_bRestartable;
	
		m_bRefreshing		= pOrg->m_bRefreshing;
		m_animTimer			= pOrg->m_animTimer;
		m_refreshProgress	= pOrg->m_refreshProgress;
		m_bStopping			= pOrg->m_bStopping;
	}
	

} // namespace wg
