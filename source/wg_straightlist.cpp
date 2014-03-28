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

#include <wg_straightlist.h>

const char WgStraightList::CLASSNAME[] = {"StraightList"};
const char WgStraightListHook::CLASSNAME[] = {"StraightListHook"};


//____ WgStraightListHook::IsInstanceOf() __________________________________________

bool WgStraightListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgListHook::IsInstanceOf(pClassName);
}

//____ WgStraightListHook::ClassName() _____________________________________________

const char * WgStraightListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgStraightListHook::Cast() ________________________________________________

WgStraightListHookPtr WgStraightListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgStraightListHookPtr( static_cast<WgStraightListHook*>(pHook.GetRealPtr()) );

	return 0;
}

//____ WqQuickListHook::Pos() _________________________________________________

WgCoord WgStraightListHook::Pos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Pos();
}

WgSize WgStraightListHook::Size() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Size();
}

WgRect WgStraightListHook::Geo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo;
}

WgCoord WgStraightListHook::ScreenPos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return m_pParent->ScreenPos() + geo.Pos();
}

WgRect WgStraightListHook::ScreenGeo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo + ScreenPos();
}


void WgStraightListHook::_requestRender()
{
	m_pParent->_onRequestRender(this);
}

void WgStraightListHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender(this, rect);
}

void WgStraightListHook::_requestResize()
{
	m_pParent->_onRequestResize(this);
}

WgHook *  WgStraightListHook::_prevHook() const
{
	return m_pParent->m_hooks.Prev(this);
}

WgHook *  WgStraightListHook::_nextHook() const
{
	return m_pParent->m_hooks.Next(this);
}

WgContainer *  WgStraightListHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgStraightList::WgStraightList()
{
	m_bHorizontal = false;
	m_sortOrder = WG_SORT_ASCENDING;
	m_pSortFunc = nullptr;
}

//____ Destructor _____________________________________________________________

WgStraightList::~WgStraightList()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgStraightList::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgList::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgStraightList::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgStraightListPtr WgStraightList::Cast( const WgObjectPtr& pObj )
{
	if( pObj && pObj->IsInstanceOf(CLASSNAME) )
		return WgStraightListPtr( static_cast<WgStraightList*>(pObj.GetRealPtr()) );

	return nullptr;
}

//____ AddWidget() ____________________________________________________________

WgStraightListHookPtr WgStraightList::AddWidget( const WgWidgetPtr& pWidget )
{
	WgStraightListHook * pHook = m_hooks.Add();
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}


//____ InsertWidget() ____________________________________________________________

WgStraightListHookPtr WgStraightList::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	int index = 0;
	if( !pSibling )
		index = m_hooks.Size()-1;
	else 
	{
		WgStraightListHook * pHook = static_cast<WgStraightListHook*>(pSibling->_hook());
		if( pHook && pHook->_parent() == this )
			index = m_hooks.Index(pHook);
		else
			return nullptr;
	}

	WgStraightListHook * pHook = m_hooks.Insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ InsertWidgetSorted() ___________________________________________________

WgStraightListHookPtr WgStraightList::InsertWidgetSorted( const WgWidgetPtr& pWidget )
{
	//TODO: Implement!

	return nullptr;
}

//____ RemoveWidget() _________________________________________________________

bool WgStraightList::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->_hook() )
		return false;

	WgStraightListHook * pHook = static_cast<WgStraightListHook*>(pWidget->_hook());
	if( pHook->_parent() != this )
		return false;

	int index = m_hooks.Index(pHook);
	_onWidgetDisappeared( pHook );
	m_hooks.Remove(index);
	return true;
}

//____ Clear() ________________________________________________________________

bool WgStraightList::Clear()
{
	m_hooks.Clear();
	_onRefresh();
	return true;
}

//____ SetOrientation() _______________________________________________________

void WgStraightList::SetOrientation( WgOrientation orientation )
{
	bool bHorizontal = (orientation == WG_HORIZONTAL);

	if( bHorizontal != m_bHorizontal )
	{
		m_bHorizontal = bHorizontal;
		_onRefresh();
	}
}

//____ SortWidgets() __________________________________________________________

void WgStraightList::SortWidgets()
{
	//TODO: Implement!!!
}

//____ SetSortOrder() _________________________________________________________

void WgStraightList::SetSortOrder( WgSortOrder order )
{
	//TODO: Implement!!!
}

//____ SetSortFunction() ______________________________________________________

void WgStraightList::SetSortFunction( WgWidgetSortFunc pSortFunc )
{
	//TODO: Implement!!!
}

//____ PreferredSize() ________________________________________________________

WgSize WgStraightList::PreferredSize() const
{
	return m_preferredSize;
}


void WgStraightList::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
}

void WgStraightList::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
}

void WgStraightList::_onCloneContent( const WgWidget * _pOrg )
{
}

void WgStraightList::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
}

void WgStraightList::_onNewSize( const WgSize& size )
{
}

void WgStraightList::_onRefresh()
{
}

void WgStraightList::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
}

void WgStraightList::_onStateChanged( WgState oldState, WgState newState )
{

}

void WgStraightList::_onRequestRender( WgStraightListHook * pHook )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	_requestRender(geo);
}

void WgStraightList::_onRequestRender( WgStraightListHook * pHook, const WgRect& rect )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	geo.x += rect.x;
	geo.y += rect.y;
	geo.w = rect.w;
	geo.h = rect.h;
	_requestRender(geo);
}

void WgStraightList::_onRequestResize( WgStraightListHook * pHook )
{
	if( pHook->m_bVisible )
	{


	}

}

void WgStraightList::_onWidgetAppeared( WgStraightListHook * pInserted )
{
}

void WgStraightList::_onWidgetDisappeared( WgStraightListHook * pToBeRemoved )
{
}

WgWidget * WgStraightList::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	//TODO: Implement!!!

	return 0;
}

//____ _getChildGeo() _________________________________________________________

void WgStraightList::_getChildGeo( WgRect& geo, const WgHook * _pHook ) const
{
	const WgStraightListHook * pHook = static_cast<const WgStraightListHook*>(_pHook);

	if( m_pSkin )
		geo = m_pSkin->ContentRect( geo, m_state );

	if( m_bHorizontal )
	{
		geo.x += pHook->m_ofs;
		geo.w = pHook->m_length;
	}
	else
	{
		geo.y += pHook->m_ofs;
		geo.h = pHook->m_length;
	}

	// Apply any padding from the entry skin, if entry visible

	if( pHook->m_bVisible )
	{
		int index = m_hooks.Index( pHook );
		if( m_pEntrySkin[index&0x1] )
			geo = m_pSkin->ContentRect( geo, pHook->_widget()->State() );
	}
}

//____ _firstHook() ___________________________________________________________

WgHook* WgStraightList::_firstHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(0);

	return nullptr;
}

//____ _lastHook() ____________________________________________________________

WgHook* WgStraightList::_lastHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(m_hooks.Size()-1);

	return nullptr;
}

//____ _firstHookWithGeo() ____________________________________________________

WgHook* WgStraightList::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return nullptr;

	WgHook * p = m_hooks.Hook(0);
	_getChildGeo(geo,p);
	return p;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook* WgStraightList::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgStraightListHook * p = m_hooks.Next(static_cast<WgStraightListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//_____ _lastHookWithGeo() ____________________________________________________

WgHook* WgStraightList::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return nullptr;

	WgHook * p = m_hooks.Hook(m_hooks.Size()-1);
	_getChildGeo(geo,p);
	return p;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook* WgStraightList::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgStraightListHook * p = m_hooks.Prev(static_cast<WgStraightListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}
