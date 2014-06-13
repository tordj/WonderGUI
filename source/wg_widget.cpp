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

#include <wg_widget.h>
#include <wg_types.h>

#	include <wg_rootpanel.h>
#	include <wg_eventhandler.h>

const char WgWidget::CLASSNAME[] = {"Widget"};


//____ Constructor ____________________________________________________________

WgWidget::WgWidget():m_id(0), m_pHook(0), m_pointerStyle(WG_POINTER_DEFAULT),
					m_markOpacity( 1 ), m_bOpaque(false),
					m_bTabLock(false), m_bReceiveTick(false), m_bPressed(false)
{
}

//____ Destructor _____________________________________________________________

WgWidget::~WgWidget()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgWidget::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgWidget::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgWidgetPtr WgWidget::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgWidgetPtr( static_cast<WgWidget*>(pObject.GetRealPtr()) );

	return 0;
}

//____ Parent() _______________________________________________________________

WgContainerPtr WgWidget::Parent() const
{ 
	if( m_pHook ) 
		return m_pHook->Parent(); 
	return 0; 
}

//____ PointerStyle() ________________________________________

WgPointerStyle WgWidget::PointerStyle() const
{
	return m_pointerStyle;
}

//____ SetEnabled() _______________________________________________________________

void WgWidget::SetEnabled( bool bEnabled )
{
	if( m_state.IsEnabled() != bEnabled || IsContainer() )
	{
		WgState old = m_state;
		m_state.SetEnabled(bEnabled);
		_onStateChanged(old);
	}
}

//____ MarkTest() _____________________________________________________________

bool WgWidget::MarkTest( const WgCoord& ofs )
{
	switch( m_markOpacity )
	{
	case 0:
		return true;
	case 256:
		return false;
	default:
		return _onAlphaTest(ofs);
	}
}

//____ SetSkin() ______________________________________________________________

void WgWidget::SetSkin( const WgSkinPtr& pSkin )
{
	WgSkinPtr pOldSkin = m_pSkin;
	m_pSkin = pSkin;
	_onSkinChanged( pOldSkin, m_pSkin );
}



//____ CloneContent() _________________________________________________________

bool WgWidget::CloneContent( const WgWidgetPtr& _pOrg )
{
	if( _pOrg->ClassName() != ClassName() )
		return false;

	WgWidget * pOrg = _pOrg.GetRealPtr();

	m_id			= pOrg->m_id;

	m_pointerStyle 	= pOrg->m_pointerStyle;
	m_pSkin			= pOrg->m_pSkin;

	m_tooltip		= pOrg->m_tooltip;
	m_markOpacity	= pOrg->m_markOpacity;

	m_bOpaque		= pOrg->m_bOpaque;
	m_bTabLock		= pOrg->m_bTabLock;

	// We do not clone state...

	_onCloneContent( pOrg );
	return true;
}

//____ _onNewHook() ___________________________________________________________

void WgWidget::_onNewHook( WgHook * pHook )
{
	m_pHook = pHook;
}

//____ _onNewRoot() ___________________________________________________________

void WgWidget::_onNewRoot( WgRootPanel * pRoot )
{
	if( m_bReceiveTick && pRoot )
		pRoot->EventHandler()->_addTickReceiver(this);
}

//____ _startReceiveTicks() ___________________________________________________

void WgWidget::_startReceiveTicks()
{
	if( !m_bReceiveTick )
	{
		m_bReceiveTick = true;

		if( m_pHook )
		{
			WgRootPanel * pRoot = m_pHook->_root();
			if( pRoot )
				pRoot->EventHandler()->_addTickReceiver(this);
		}
	}
}

//____ _stopReceiveTicks() ____________________________________________________

void WgWidget::_stopReceiveTicks()
{
	m_bReceiveTick = false;
}

//____ Local2abs() ____________________________________________________________

WgCoord WgWidget::Local2abs( const WgCoord& cord ) const
{
	WgCoord c = GlobalPos();
	c.x += cord.x;
	c.y += cord.y;
	return c;
}

//____ Abs2local() ____________________________________________________________

WgCoord WgWidget::Abs2local( const WgCoord& cord ) const
{
	WgCoord c = GlobalPos();
	return WgCoord( cord.x - c.x, cord.y - c.y );
}

//____ _eventHandler() __________________________________________________________

WgEventHandler * WgWidget::_eventHandler() const
{
	if( m_pHook )
	{
		WgRootPanel * pRoot = m_pHook->_root();
		if( pRoot )
			return pRoot->EventHandler().GetRealPtr();
	}
	return 0;
}

//____ HeightForWidth() _______________________________________________________

int WgWidget::HeightForWidth( int width ) const
{
	return PreferredSize().h;		// Default is to stick with best height no matter what width.
}

//____ WidthForHeight() _______________________________________________________

int WgWidget::WidthForHeight( int height ) const
{
	return PreferredSize().w;		// Default is to stick with best width no matter what height.
}

//____ PreferredSize() ________________________________________________________

WgSize WgWidget::PreferredSize() const
{
	if( m_pSkin )
		return m_pSkin->PreferredSize();
	else
		return WgSize(0,0);
}

//____ MinSize() ______________________________________________________________

WgSize WgWidget::MinSize() const
{
	if( m_pSkin )
		return m_pSkin->MinSize();
	else
		return WgSize(0,0);
}

//____ MaxSize() ______________________________________________________________

WgSize WgWidget::MaxSize() const
{
	return WgSize(2<<24,2<<24);
}

//____ _getBlendMode() _________________________________________________________

WgBlendMode WgWidget::_getBlendMode() const
{
	WgContainer * pParent = _parent();
	if( pParent )
		return pParent->_getBlendMode();
	else
		return WG_BLENDMODE_BLEND;		// We always start out with WG_BLENDMODE_BLEND.
}

//____ _queueEvent() __________________________________________________________

void WgWidget::_queueEvent( const WgEventPtr& pEvent )
{
	if( m_pHook )
	{
		WgRootPanel * pRoot = m_pHook->_root();
		if( pRoot )
		{
			pRoot->EventHandler()->QueueEvent(pEvent);
			return;
		}
	}
}


//____ _renderPatches() ________________________________________________________

void WgWidget::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		WgRect clip( _window, *pRect );
		if( clip.w > 0 || clip.h > 0 )
			_onRender( pDevice, _canvas, _window, clip );
	}
}

//____ onCollectPatches()  ____________________________________________________

void WgWidget::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
		container.Add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() _______________________________________________________

void WgWidget::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
	{
		patches.Sub( WgRect( geo, clip ) );
	}
}

//____ _onRender() ____________________________________________________________

void WgWidget::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_pSkin )
		m_pSkin->Render( pDevice, _canvas, m_state, _clip );
}

//____ _onNewSize() ___________________________________________________________

void WgWidget::_onNewSize( const WgSize& size )
{
	_requestRender();
}

//____ _onRefresh() ___________________________________________________________

void WgWidget::_onRefresh()
{
	if( m_pSkin && m_pSkin->IsOpaque(m_state) )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestResize();
	_requestRender();
}

//____ _onSkinChanged() _______________________________________________________

void WgWidget::_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin )
{
	if( !pOldSkin || !pNewSkin || pOldSkin->ContentPadding() != pNewSkin->ContentPadding() ||
		pOldSkin->PreferredSize() != pNewSkin->PreferredSize() ||
		pOldSkin->MinSize() != pNewSkin->MinSize() )
	{
		_requestResize();
	}

	if( pNewSkin && pNewSkin->IsOpaque(m_state) )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
}

//____ _onStateChanged() ______________________________________________________

void WgWidget::_onStateChanged( WgState oldState )
{
	if( m_pSkin && !m_pSkin->IsStateIdentical(m_state, oldState) )
	{
		m_bOpaque = m_pSkin->IsOpaque(m_state);
		_requestRender();
	}
}

//____ _onEvent() _____________________________________________________________

void WgWidget::_onEvent( const WgEventPtr& _pEvent, WgEventHandler * pHandler )
{
	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_MOUSE_ENTER:
			if( m_bPressed )
				m_state.SetPressed(true);
			else
				m_state.SetHovered(true);
			break;
		case WG_EVENT_MOUSE_LEAVE:
			m_state.SetHovered(false);			// Also clears any pressed flag.
			break;
		case WG_EVENT_MOUSE_PRESS:
		{
			WgMousePressEventPtr pEvent = WgMousePressEvent::Cast(_pEvent);
			if( pEvent->Button() == WG_BUTTON_LEFT )
			{
				if( m_state.IsHovered() )
					m_state.SetPressed(true);

				m_bPressed = true;
			}
			break;
		}
		case WG_EVENT_MOUSE_RELEASE:
		{
			WgMouseReleaseEventPtr pEvent = WgMouseReleaseEvent::Cast(_pEvent);
			if( pEvent->Button() == WG_BUTTON_LEFT )
			{
				if( m_state.IsHovered() )
					m_state.SetPressed(false);

				m_bPressed = false;
			}
			break;
		}
		case WG_EVENT_FOCUS_GAINED:
			m_state.SetFocused(true);
			break;
		case WG_EVENT_FOCUS_LOST:
			m_state.SetFocused(false);
			break;
	}

	if( m_state != oldState )
		_onStateChanged( oldState );
}

bool WgWidget::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pSkin )
		return m_pSkin->MarkTest( ofs, WgRect(0,0,Size()), m_state, m_markOpacity );

	return false;
}
