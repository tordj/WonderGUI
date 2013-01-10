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

static const char	c_hookType[] = {"Capsule"};


//_____________________________________________________________________________
const char * WgCapsule::Hook::Type( void ) const
{
	return ClassType();
}

//_____________________________________________________________________________
const char * WgCapsule::Hook::ClassType()
{
	return c_hookType;
}

//____ Constructor ____________________________________________________________

WgCapsule::WgCapsule()
{
	m_hook.m_pParent = this;
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
		return false;

	m_hook._attachWidget(pWidget);
	pWidget->_onNewSize(Size());

	_requestRender();
	_requestResize();
	return &m_hook;
}

//____ DeleteChild() ___________________________________________________________

bool WgCapsule::DeleteChild()
{
	WgWidget * pWidget = m_hook._releaseWidget();
	if( pWidget )
	{
		delete pWidget;
		_requestRender();
		_requestResize();
		return true;
	}

	return false;
}

//____ ReleaseChild() __________________________________________________________

WgWidget * WgCapsule::ReleaseChild()
{
	WgWidget * pWidget = m_hook._releaseWidget();
	if( pWidget )
	{
		_requestRender();
		_requestResize();
	}

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

//____ HeightForWidth() ________________________________________________________

int WgCapsule::HeightForWidth( int width ) const
{
	if( m_hook.Widget() )
		return m_hook.Widget()->HeightForWidth( width );
	else
		return WgWidget::HeightForWidth(width);
}

//____ WidthForHeight() ________________________________________________________

int WgCapsule::WidthForHeight( int height ) const
{
	if( m_hook.Widget() )
		return m_hook.Widget()->WidthForHeight( height );
	else
		return WgWidget::WidthForHeight(height);
}

//____ PreferredSize() ___________________________________________________________

WgSize WgCapsule::PreferredSize() const
{
	if( m_hook.Widget() )
		return m_hook.Widget()->PreferredSize();
	else
		return WgSize(1,1);
}

//____ _onCollectPatches() _____________________________________________________

void WgCapsule::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_hook.Widget() )
		m_hook.Widget()->_onCollectPatches( container, geo, clip );
}

//____ _onMaskPatches() ________________________________________________________

void WgCapsule::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( m_hook.Widget() )
		m_hook.Widget()->_onMaskPatches( patches, geo, clip, blendMode );
}

//____ _onCloneContent() _______________________________________________________

void WgCapsule::_onCloneContent( const WgWidget * _pOrg )
{
}

//____ _onNewSize() ____________________________________________________________

void WgCapsule::_onNewSize( const WgSize& size )
{
	if( m_hook.Widget() )
		m_hook.Widget()->_onNewSize(size);
}

//____ _firstHook() ____________________________________________________________

WgHook* WgCapsule::_firstHook() const
{
	if( m_hook.Widget() )
		return const_cast<Hook*>(&m_hook);
	else
		return 0;
}

//____ _lastHook() _____________________________________________________________

WgHook* WgCapsule::_lastHook() const
{
	if( m_hook.Widget() )
		return const_cast<Hook*>(&m_hook);
	else
		return 0;
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hook.Widget() )
	{
		geo = WgRect(0,0,Size());
		return const_cast<Hook*>(&m_hook);
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
		geo = WgRect(0,0,Size());
		return const_cast<Hook*>(&m_hook);
	}

	return 0;
}

//_____ _prevHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}
