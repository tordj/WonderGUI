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
	
	const char RefreshButton::CLASSNAME[] = {"RefreshButton"};
	
	//____ Constructor ____________________________________________________________
	
	RefreshButton::RefreshButton() : m_refreshText(this), refreshText(&m_refreshText)
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
	
	RefreshButton::~RefreshButton()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool RefreshButton::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Button::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * RefreshButton::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	RefreshButton_p RefreshButton::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return RefreshButton_p( static_cast<RefreshButton*>(pObject) );
	
		return 0;
	}
	
	
	//_____________________________________________________________________________
	void RefreshButton::setAnimation( GfxAnim * pAnimation )
	{
		m_pRefreshAnim		= pAnimation;
	
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void RefreshButton::setRefreshMode( RefreshMode mode )
	{
		m_refreshMode = mode;
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void RefreshButton::setAnimTarget( AnimTarget target )
	{
		m_animTarget = target;
		if( m_bRefreshing )
			_requestRender();
	}
	
	//_____________________________________________________________________________
	void RefreshButton::setRestartable( bool bRestartable )
	{
		m_bRestartable = bRestartable;
	}
	
	//_____________________________________________________________________________
	void RefreshButton::startRefresh()
	{
		if( m_pRefreshAnim && (!m_bRefreshing || m_bRestartable) )
		{
			m_bRefreshing = true;
			m_bStopping = false;
			m_refreshProgress = 0.f;
			m_animTimer = 0;
			m_pRefreshAnim->setPlayMode( AnimMode::Looping );		//UGLY! Should change once the animation system has been updated.
			m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
			_requestRender();
		}
	}
	
	//_____________________________________________________________________________
	void RefreshButton::stopRefresh()
	{
		if( m_pRefreshAnim && m_pRefreshAnim->durationScaled())
		{
			m_bStopping = true;
			m_animTimer = m_animTimer % m_pRefreshAnim->durationScaled();	// So it doesn't stop immmediately.
			m_pRefreshAnim->setPlayMode( AnimMode::Forward );	//UGLY! Should change once the animation system has been updated.
		}
		else
		{
			stopRefreshNow();
		}
	}
	
	//_____________________________________________________________________________
	void RefreshButton::stopRefreshNow()
	{
		m_refreshProgress = 1.f;
		m_bRefreshing = false;
		Base::msgRouter()->deleteRoute( m_tickRouteId );
		m_tickRouteId = 0;
		_requestRender();
	}
	
	//_____________________________________________________________________________
	void RefreshButton::setRefreshProgress( float fraction )
	{
		if( m_refreshMode == PROGRESS )
		{
			m_refreshProgress = fraction;
	
			if( m_pRefreshAnim )
			{
				GfxFrame * pOldFrame = m_pRefreshAnim->getFrame( m_animTimer );
				m_animTimer = (uint32_t) (fraction * m_pRefreshAnim->duration());
				GfxFrame * pNewFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
				if( pOldFrame != pNewFrame )
					_requestRender();
			}
	
		}
	}
	
	
	//____ _setSize() ____________________________________________________________
	
	void RefreshButton::_setSize( const Size& size )
	{
		Size contentSize = m_pSkin ? size - m_pSkin->contentPadding() : size;
		m_refreshText.setSize(contentSize);
	
		Button::_setSize( size );
	}
	
	
	//____ _receive() _____________________________________________________________
	
	void RefreshButton::_receive( Msg * pMsg )
	{
		Button::_receive( pMsg );
	
		switch( pMsg->type() )
		{
			case MsgType::Tick:
			{
				if( m_bRefreshing && m_pRefreshAnim )
				{
					if( m_refreshMode != PROGRESS )
					{
						TickMsg_p pTick = TickMsg::cast(pMsg);
	
						GfxFrame * pOldFrame = m_pRefreshAnim->getFrame( m_animTimer );
						m_animTimer += pTick->timediff();
						GfxFrame * pNewFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
						// RequestRender if animation has moved.
	
						if( pOldFrame != pNewFrame )
							_requestRender();
	
						// Check if animation has ended.
	
						if( m_bStopping && pNewFrame == m_pRefreshAnim->getLastFrame() )		//UGLY! Change when we have updated Anim!
						{
							m_bRefreshing = false;
							m_bStopping = false;
							Base::msgRouter()->deleteRoute( m_tickRouteId );
							m_tickRouteId = 0;
							_requestRender();
						}
					}
				}
				break;
			}
	
			case MsgType::KeyRelease:
			{
				KeyReleaseMsg_p pKeyRelease = KeyReleaseMsg::cast(pMsg);
	
				if( m_bAutoRefresh && pKeyRelease->translatedKeyCode() == Key::Return )
					startRefresh();
	
				break;
			}
	
			case MsgType::MouseRelease:
			{
				MouseReleaseMsg_p pBtnRelease = MouseReleaseMsg::cast(pMsg);
	
				if( m_bAutoRefresh && m_bPressed && pBtnRelease->button() == MouseButton::Left )
					startRefresh();
	
				break;
			}
	
			default:
				break;
		}
	}
	
	
	//____ _render() _____________________________________________________________
	
	void RefreshButton::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		// Render background or animation
	
		if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
		{
			GfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
			switch( m_animTarget )
			{
				case BUTTON_CENTERED:
				{
					Rect dest = (	_canvas.x + (_canvas.w - pAnimFrame->rect.w)/2,
									_canvas.y + (_canvas.h - pAnimFrame->rect.h)/2,
									pAnimFrame->rect.size() );
	
					pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, pAnimFrame->rect, dest );
				}
				break;
	
				case BUTTON_STRETCHED:
				{
					pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, pAnimFrame->rect, _canvas );
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
	
		Rect contentRect = _canvas;
		if( m_pSkin )
			contentRect = m_pSkin->contentRect( _canvas, m_state );
	
		// Get icon and text rect from content rect
	
		Size iconSize;
		if( !m_icon.isEmpty() )
			iconSize = m_icon.skin()->preferredSize();
		else if( m_animTarget == ICON && m_pRefreshAnim )
			iconSize = m_pRefreshAnim->size();
	
		Rect iconRect = m_icon.getIconRect( contentRect, iconSize );
		Rect textRect = m_icon.getTextRect( contentRect, iconRect );
	
	
		// Render icon or animation
	
		if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
		{
			// Render animation
	
			GfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );
	
			pDevice->clipStretchBlit( _clip, pAnimFrame->pSurf, pAnimFrame->rect, iconRect );
		}
		else if( !m_icon.isEmpty() )
			m_icon.skin()->render( pDevice, iconRect, m_state, _clip );
	
		// Print text
	
		TextItem * pText;
	
		if( m_bRefreshing )
			pText = &m_refreshText;
		else
			pText = &m_text;
	
	 	if( !pText->isEmpty() )
		{
			pText->setState(m_state);		//TODO: Should be done when state actually is set.
	
			Rect clip(textRect,_clip);
			pText->render(pDevice, textRect, clip );
		}
	}
	
	
	
	//____ _cloneContent() _______________________________________________________
	
	void RefreshButton::_cloneContent( const Widget * _pOrg )
	{
		RefreshButton * pOrg = (RefreshButton *) _pOrg;
	
		Button::_cloneContent( _pOrg );
	
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
