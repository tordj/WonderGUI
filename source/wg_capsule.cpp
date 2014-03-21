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

const char WgCapsule::CLASSNAME[] = {"Capsule"};
const char WgCapsuleHook::CLASSNAME[] = {"CapsuleHook"};

//____ WgCapsuleHook::IsInstanceOf() __________________________________________

bool WgCapsuleHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgCapsuleHook::ClassName() _____________________________________________

const char * WgCapsuleHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgCapsuleHook::Cast() __________________________________________________

WgCapsuleHookPtr WgCapsuleHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgCapsuleHookPtr( static_cast<WgCapsuleHook*>(pHook.GetRealPtr()) );

	return 0;
}


WgCoord WgCapsuleHook::Pos() const { return m_pParent->Pos(); }
WgSize WgCapsuleHook::Size() const { 	return m_pParent->Size(); }
WgRect WgCapsuleHook::Geo() const { return m_pParent->Geo(); }

WgCoord WgCapsuleHook::ScreenPos() const { return m_pParent->ScreenPos(); }
WgRect WgCapsuleHook::ScreenGeo() const { return m_pParent->ScreenGeo(); }

WgCapsulePtr WgCapsuleHook::Parent() const { return m_pParent; }

void WgCapsuleHook::_requestRender() { m_pParent->_requestRender(); }
void WgCapsuleHook::_requestRender( const WgRect& rect ) { m_pParent->_requestRender(rect); }
void WgCapsuleHook::_requestResize() { m_pParent->_requestResize(); }

WgHook * WgCapsuleHook::_prevHook() const { return 0; }
WgHook * WgCapsuleHook::_nextHook() const { return 0; }
WgContainer * WgCapsuleHook::_parent() const { return m_pParent; }


//____ Constructor ____________________________________________________________

WgCapsule::WgCapsule()
{
	m_hook.m_pParent = this;
}

//____ IsInstanceOf() _________________________________________________________

bool WgCapsule::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgCapsule::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgCapsulePtr WgCapsule::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCapsulePtr( static_cast<WgCapsule*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetWidget() ______________________________________________________________

WgCapsuleHookPtr WgCapsule::SetWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return false;

	m_hook._setWidget(pWidget.GetRealPtr());
	pWidget->_onNewSize(Size());

	_requestRender();
	_requestResize();
	return &m_hook;
}

//____ RemoveWidget() ___________________________________________________________

bool WgCapsule::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( m_hook._widget() != pWidget.GetRealPtr() )
		return false;

	m_hook._setWidget(0);
	_requestRender();
	_requestResize();
	return true;
}

//____ Clear() __________________________________________________________

bool WgCapsule::Clear()
{
	if( !m_hook._widget() )
		return false;

	m_hook._setWidget(0);
	_requestRender();
	_requestResize();
	return true;
}

//____ HeightForWidth() ________________________________________________________

int WgCapsule::HeightForWidth( int width ) const
{
	if( m_hook._widget() )
		return m_hook._widget()->HeightForWidth( width );
	else
		return WgWidget::HeightForWidth(width);
}

//____ WidthForHeight() ________________________________________________________

int WgCapsule::WidthForHeight( int height ) const
{
	if( m_hook._widget() )
		return m_hook._widget()->WidthForHeight( height );
	else
		return WgWidget::WidthForHeight(height);
}

//____ PreferredSize() ___________________________________________________________

WgSize WgCapsule::PreferredSize() const
{
	if( m_hook._widget() )
		return m_hook._widget()->PreferredSize();
	else
		return WgSize(1,1);
}

//____ _onCollectPatches() _____________________________________________________

void WgCapsule::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_hook._widget() )
		m_hook._widget()->_onCollectPatches( container, geo, clip );
}

//____ _onMaskPatches() ________________________________________________________

void WgCapsule::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( m_hook._widget() )
		m_hook._widget()->_onMaskPatches( patches, geo, clip, blendMode );
}

//____ _onCloneContent() _______________________________________________________

void WgCapsule::_onCloneContent( const WgWidget * _pOrg )
{
}

//____ _onNewSize() ____________________________________________________________

void WgCapsule::_onNewSize( const WgSize& size )
{
	if( m_hook._widget() )
		m_hook._widget()->_onNewSize(size);
}

//____ _firstHook() ____________________________________________________________

WgHook* WgCapsule::_firstHook() const
{
	if( m_hook._widget() )
		return const_cast<WgCapsuleHook*>(&m_hook);
	else
		return 0;
}

//____ _lastHook() _____________________________________________________________

WgHook* WgCapsule::_lastHook() const
{
	if( m_hook._widget() )
		return const_cast<WgCapsuleHook*>(&m_hook);
	else
		return 0;
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hook._widget() )
	{
		geo = WgRect(0,0,Size());
		return const_cast<WgCapsuleHook*>(&m_hook);
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
	if( m_hook._widget() )
	{
		geo = WgRect(0,0,Size());
		return const_cast<WgCapsuleHook*>(&m_hook);
	}

	return 0;
}

//_____ _prevHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}
