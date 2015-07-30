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

//____ WgCapsuleHook::isInstanceOf() __________________________________________

bool WgCapsuleHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::isInstanceOf(pClassName);
}

//____ WgCapsuleHook::className() _____________________________________________

const char * WgCapsuleHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgCapsuleHook::cast() __________________________________________________

WgCapsuleHook_p WgCapsuleHook::cast( const WgHook_p& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgCapsuleHook_p( static_cast<WgCapsuleHook*>(pHook.rawPtr()) );

	return 0;
}


WgCoord WgCapsuleHook::pos() const { return m_pParent->pos(); }
WgSize WgCapsuleHook::size() const { 	return m_pParent->size(); }
WgRect WgCapsuleHook::geo() const { return m_pParent->geo(); }

WgCoord WgCapsuleHook::globalPos() const { return m_pParent->globalPos(); }
WgRect WgCapsuleHook::globalGeo() const { return m_pParent->globalGeo(); }

WgCapsule_p WgCapsuleHook::parent() const { return m_pParent; }

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

//____ isInstanceOf() _________________________________________________________

bool WgCapsule::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgCapsule::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgCapsule_p WgCapsule::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgCapsule_p( static_cast<WgCapsule*>(pObject.rawPtr()) );

	return 0;
}

//____ setWidget() ______________________________________________________________

WgCapsuleHook_p WgCapsule::setWidget( const WgWidget_p& pWidget )
{
	if( !pWidget )
		return 0;

	m_hook._setWidget(pWidget.rawPtr());
	pWidget->_onNewSize(size());

	_requestRender();
	_requestResize();
	return &m_hook;
}

//____ removeWidget() ___________________________________________________________

bool WgCapsule::removeWidget( const WgWidget_p& pWidget )
{
	if( m_hook._widget() != pWidget.rawPtr() )
		return false;

	m_hook._setWidget(0);
	_requestRender();
	_requestResize();
	return true;
}

//____ clear() __________________________________________________________

bool WgCapsule::clear()
{
	if( !m_hook._widget() )
		return false;

	m_hook._setWidget(0);
	_requestRender();
	_requestResize();
	return true;
}

//____ matchingHeight() ________________________________________________________

int WgCapsule::matchingHeight( int width ) const
{
	if( m_hook._widget() )
		return m_hook._widget()->matchingHeight( width );
	else
		return WgWidget::matchingHeight(width);
}

//____ matchingWidth() ________________________________________________________

int WgCapsule::matchingWidth( int height ) const
{
	if( m_hook._widget() )
		return m_hook._widget()->matchingWidth( height );
	else
		return WgWidget::matchingWidth(height);
}

//____ preferredSize() ___________________________________________________________

WgSize WgCapsule::preferredSize() const
{
	if( m_hook._widget() )
		return m_hook._widget()->preferredSize();
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
		geo = WgRect(0,0,size());
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
		geo = WgRect(0,0,size());
		return const_cast<WgCapsuleHook*>(&m_hook);
	}

	return 0;
}

//_____ _prevHookWithGeo() _____________________________________________________

WgHook * WgCapsule::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}
