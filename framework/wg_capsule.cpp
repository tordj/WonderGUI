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

#include <wg_capsule.h>
#include <wg_patches.h>

static const char	c_hookType[] = {"Capsule"};


//_____________________________________________________________________________
const char * WgCapsule::CapsuleHook::Type( void ) const
{
	return ClassType();
}

//_____________________________________________________________________________
const char * WgCapsule::CapsuleHook::ClassType()
{
	return c_hookType;
}

//_____________________________________________________________________________
WgCoord WgCapsule::CapsuleHook::PixelPos() const
{
    if( m_pParent->m_pSkin )
        return m_pParent->m_pSkin->ContentOfs(m_pParent->m_state,m_pParent->m_scale);
    else
        return WgCoord();
}

//_____________________________________________________________________________
WgSize WgCapsule::CapsuleHook::PixelSize() const
{
    if( m_pParent->m_pSkin )
        return m_pParent->PixelSize() - m_pParent->m_pSkin->ContentPadding(m_pParent->m_scale);
    else
        return m_pParent->PixelSize();
}

//_____________________________________________________________________________
WgRect WgCapsule::CapsuleHook::PixelGeo() const
{
    if( m_pParent->m_pSkin )
        return m_pParent->m_pSkin->ContentRect( { 0,0, m_pParent->PixelSize() }, m_pParent->m_state, m_pParent->m_scale );
    else
        return { 0,0, m_pParent->PixelSize() };
}

//_____________________________________________________________________________
WgCoord WgCapsule::CapsuleHook::ScreenPixelPos() const
{
    if( m_pParent->m_pSkin )
        return m_pParent->ScreenPixelPos() + m_pParent->m_pSkin->ContentOfs(m_pParent->m_state,m_pParent->m_scale);
    else
        return m_pParent->ScreenPixelPos();
}

//_____________________________________________________________________________
WgRect WgCapsule::CapsuleHook::ScreenPixelGeo() const
{
    if( m_pParent->m_pSkin )
        return m_pParent->m_pSkin->ContentRect( m_pParent->ScreenPixelGeo(), m_pParent->m_state, m_pParent->m_scale );
    else
        return m_pParent->ScreenPixelGeo();
}



//____ Constructor ____________________________________________________________

WgCapsule::WgCapsule()
{
	m_hook.m_pParent = this;
    m_bSiblingsOverlap = false;
}



//____ IsCapsule() ______________________________________________________________

bool WgCapsule::IsCapsule() const
{
	return true;
}

//____ CastToCapsule() _______________________________________________________

WgCapsule * WgCapsule::CastToCapsule()
{
	return this;
}

const WgCapsule * WgCapsule::CastToCapsule() const
{
	return this;
}

//____ SetChild() ______________________________________________________________

WgHook * WgCapsule::SetChild( WgWidget * pWidget )
{
	if( !pWidget )
          return NULL;

    WgSize sz = PixelSize();
    if( m_pSkin )
        sz -= m_pSkin->ContentPadding(m_scale);
    
	m_hook._attachWidget(pWidget);
	pWidget->_onNewSize(sz);

	_childAddedRemovedOrReplaced();
	return &m_hook;
}

//____ DeleteChild() ___________________________________________________________

bool WgCapsule::DeleteChild()
{
	WgWidget * pWidget = m_hook._releaseWidget();
	if( pWidget )
	{
		delete pWidget;
		_childAddedRemovedOrReplaced();
		return true;
	}

	return false;
}

//____ ReleaseChild() __________________________________________________________

WgWidget * WgCapsule::ReleaseChild()
{
	WgWidget * pWidget = m_hook._releaseWidget();
	if( pWidget )
		_childAddedRemovedOrReplaced();

	return pWidget;
}

//____ DeleteChild() ___________________________________________________________

bool WgCapsule::DeleteChild( WgWidget * pWidget )
{
	if( pWidget == m_hook.Widget() )
		return DeleteChild();

	return false;
}

//____ ReleaseChild() __________________________________________________________

WgWidget * WgCapsule::ReleaseChild( WgWidget * pWidget )
{
	if(pWidget == m_hook.Widget() )
		return ReleaseChild();

	return 0;
}

//____ DeleteAllChildren() _____________________________________________________

bool WgCapsule::DeleteAllChildren()
{
	return DeleteChild();
}

//____ ReleaseAllChildren() ____________________________________________________

bool WgCapsule::ReleaseAllChildren()
{
	ReleaseChild();
	return true;
}

//____ MatchingPixelHeight() ________________________________________________________

int WgCapsule::MatchingPixelHeight( int width ) const
{
    WgSize padding;
    if( m_pSkin )
        padding = m_pSkin->ContentPadding(m_scale);

	if( m_hook.Widget() )
		return m_hook.Widget()->MatchingPixelHeight( width - padding.w ) + padding.h;
	else
		return WgWidget::MatchingPixelHeight(width);
}

//____ MatchingPixelWidth() ________________________________________________________

int WgCapsule::MatchingPixelWidth( int height ) const
{
    WgSize padding;
    if( m_pSkin )
        padding = m_pSkin->ContentPadding(m_scale);
    
	if( m_hook.Widget() )
		return m_hook.Widget()->MatchingPixelWidth( height - padding.h ) + padding.w;
	else
		return WgWidget::MatchingPixelWidth(height);
}

//____ PreferredPixelSize() ___________________________________________________________

WgSize WgCapsule::PreferredPixelSize() const
{
    WgSize sz;
    
	if( m_hook.Widget() )
		sz = m_hook.Widget()->PreferredPixelSize();
	else
		sz = WgSize(1,1);
    
    if( m_pSkin )
        sz += m_pSkin->ContentPadding(m_scale);
    
    return sz;
}

//____ _childAddedRemovedOrReplaced() _________________________________________

void WgCapsule::_childAddedRemovedOrReplaced()
{
	_requestRender();
	_requestResize();
}

//____ _onCollectPatches() _____________________________________________________

void WgCapsule::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
    if( m_pSkin )
        container.Add( WgRect( geo, clip ) );
	else if( m_hook.Widget() )
		m_hook.Widget()->_onCollectPatches( container, geo, clip );
}

//____ _onMaskPatches() ________________________________________________________

void WgCapsule::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
    if( m_pSkin && m_pSkin->IsOpaque(m_state) )
        patches.Sub( WgRect( geo, clip ) );
	else if( m_hook.Widget() )
    {
        WgRect childGeo;
        if( m_pSkin )
            childGeo = m_pSkin->ContentRect(geo,m_state,m_scale);
        else
            childGeo = geo;
        
        m_hook.Widget()->_onMaskPatches( patches, childGeo, clip, blendMode );
    }
}

//____ _onCloneContent() _______________________________________________________

void WgCapsule::_onCloneContent( const WgWidget * _pOrg )
{
}

//____ _onNewSize() ____________________________________________________________

void WgCapsule::_onNewSize( const WgSize& size )
{
	if( m_hook.Widget() )
    {
        WgSize sz = size;
        if( m_pSkin )
            sz -= m_pSkin->ContentPadding(m_scale);
        
        m_hook.Widget()->_onNewSize(sz);
    }
}

//____ _onRenderRequested() ___________________________________________________

void WgCapsule::_onRenderRequested()
{
    if( m_pSkin )
        _requestRender( m_pSkin->ContentRect(PixelSize(), m_state, m_scale) );
    else
	_requestRender();
}

void WgCapsule::_onRenderRequested(const WgRect& rect)
{
    if( m_pSkin )
        _requestRender( rect + m_pSkin->ContentOfs(m_state,m_scale) );
    else
	_requestRender(rect);
}

//____ _onResizeRequested() ___________________________________________________

void WgCapsule::_onResizeRequested()
{
	_requestResize();
}


//____ _firstHook() ____________________________________________________________

WgHook* WgCapsule::_firstHook() const
{
	if( m_hook.Widget() )
		return const_cast<CapsuleHook*>(&m_hook);
	else
		return 0;
}

//____ _lastHook() _____________________________________________________________

WgHook* WgCapsule::_lastHook() const
{
	if( m_hook.Widget() )
		return const_cast<CapsuleHook*>(&m_hook);
	else
		return 0;
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hook.Widget() )
	{
        if( m_pSkin )
            geo = m_pSkin->ContentRect(PixelSize(), m_state, m_scale );
        else
		geo = WgRect(0,0,PixelSize());

		return const_cast<CapsuleHook*>(&m_hook);
	}

	return 0;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgCapsule::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}

//____ _lastHookWithGeo() ______________________________________________________

WgHook * WgCapsule::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hook.Widget() )
	{
        if( m_pSkin )
            geo = m_pSkin->ContentRect(PixelSize(), m_state, m_scale );
        else
		geo = WgRect(0,0, PixelSize());

		return const_cast<CapsuleHook*>(&m_hook);
	}

	return 0;
}

//_____ _prevHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}