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
#include <wg_msgrouter.h>
#include <wg_internal.h>

namespace wg
{

	const TypeInfo RefreshButton::TYPEINFO = { "RefreshButton", &Button::TYPEINFO };

	//____ constructor ____________________________________________________________

	RefreshButton::RefreshButton() : refreshText(this)
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

	//____ typeInfo() _________________________________________________________

	const TypeInfo& RefreshButton::typeInfo(void) const
	{
		return TYPEINFO;
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


	//____ _resize() ____________________________________________________________

	void RefreshButton::_resize( const Size& size )
	{
		Size contentSize = m_pSkin ? size - m_pSkin->contentPaddingSize() : size;
		OO(refreshText)._setSize(contentSize);

		Button::_resize( size );
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
						TickMsg_p pTick = static_cast<TickMsg*>(pMsg);

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
				KeyReleaseMsg_p pKeyRelease = static_cast<KeyReleaseMsg*>(pMsg);

				if( m_bAutoRefresh && pKeyRelease->translatedKeyCode() == Key::Return )
					startRefresh();

				break;
			}

			case MsgType::MouseRelease:
			{
				MouseReleaseMsg_p pBtnRelease = static_cast<MouseReleaseMsg*>(pMsg);

				if( m_bAutoRefresh && m_bPressed && pBtnRelease->button() == MouseButton::Left )
					startRefresh();

				break;
			}

			default:
				break;
		}
	}


	//____ _render() _____________________________________________________________

	void RefreshButton::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		// Render background or animation

		if( m_bRefreshing && m_pRefreshAnim && m_animTarget != ICON )
		{
			GfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );

			RectI canvas = _canvas.px();

			switch( m_animTarget )
			{
				case BUTTON_CENTERED:
				{
					RectI dest = {	canvas.x + (canvas.w - pAnimFrame->rect.w)/2,
									canvas.y + (canvas.h - pAnimFrame->rect.h)/2,
									pAnimFrame->rect.size() };

					pDevice->setBlitSource(pAnimFrame->pSurf);
					pDevice->stretchBlit( dest, pAnimFrame->rect );
				}
				break;

				case BUTTON_STRETCHED:
				{
					pDevice->setBlitSource(pAnimFrame->pSurf);
					pDevice->stretchBlit( canvas, pAnimFrame->rect );
				}
				break;

				default:
					break;
			}
		}
		else if( m_pSkin )
		{
			m_pSkin->render( pDevice, _canvas, m_state );
		}

		// Get content rect with displacement.

		Rect contentRect = _canvas;
		if( m_pSkin )
			contentRect = m_pSkin->contentRect( _canvas, m_state );

		// Get icon and text rect from content rect

		Size iconSize;
		if( !_icon().isEmpty() )
			iconSize = _icon().skin()->preferredSize();
		else if( m_animTarget == ICON && m_pRefreshAnim )
			iconSize = m_pRefreshAnim->size();

		Rect iconRect = _icon()._getIconRect( contentRect, iconSize );
		Rect textRect = _icon()._getTextRect( contentRect, iconRect );


		// Render icon or animation

		if( m_bRefreshing && m_pRefreshAnim && m_animTarget == ICON )
		{
			// Render animation

			GfxFrame * pAnimFrame = m_pRefreshAnim->getFrame( m_animTimer );
			pDevice->setBlitSource(pAnimFrame->pSurf);
			pDevice->stretchBlit( iconRect, pAnimFrame->rect );
		}
		else if( !_icon().isEmpty() )
			_icon().skin()->render( pDevice, iconRect, m_state );

		// Print text

		if (m_bRefreshing )
		{
			if( !OO(refreshText).isEmpty() )
			{
				OO(refreshText)._setState(m_state);		//TODO: Should be done when state actually is set.
				OO(refreshText)._render(pDevice, textRect);
			}
		}
		else
		{
			if (!OO(text).isEmpty())
			{
				OO(text)._setState(m_state);		//TODO: Should be done when state actually is set.
				OO(text)._render(pDevice, textRect);
			}
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