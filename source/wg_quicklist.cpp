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

#include <wg_quicklist.h>

const char WgQuickList::CLASSNAME[] = {"QuickList"};
const char WgQuickListHook::CLASSNAME[] = {"QuickListHook"};


//____ WgQuickListHook::IsInstanceOf() __________________________________________

bool WgQuickListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgListHook::IsInstanceOf(pClassName);
}

//____ WgQuickListHook::ClassName() _____________________________________________

const char * WgQuickListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgQuickListHook::Cast() ________________________________________________

WgQuickListHookPtr WgQuickListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgQuickListHookPtr( static_cast<WgQuickListHook*>(pHook.GetRealPtr()) );

	return 0;
}

//____ WqQuickListHook::Pos() _________________________________________________

WgCoord WgQuickListHook::Pos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Pos();
}

WgSize WgQuickListHook::Size() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo.Size();
}

WgRect WgQuickListHook::Geo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo;
}

WgCoord WgQuickListHook::ScreenPos() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return m_pParent->ScreenPos() + geo.Pos();
}

WgRect WgQuickListHook::ScreenGeo() const
{
	WgRect	geo;
	m_pParent->_getChildGeo(geo,this);
	return geo + ScreenPos();
}


void WgQuickListHook::_requestRender()
{
	m_pParent->_onRequestRender(this);
}

void WgQuickListHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender(this, rect);
}

void WgQuickListHook::_requestResize()
{
	m_pParent->_onRequestResize(this);
}

WgHook *  WgQuickListHook::_prevHook() const
{
	return m_pParent->m_hooks.Prev(this);
}

WgHook *  WgQuickListHook::_nextHook() const
{
	return m_pParent->m_hooks.Next(this);
}

WgContainer *  WgQuickListHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgQuickList::WgQuickList()
{
	m_bHorizontal = false;
	m_sortOrder = WG_SORT_ASCENDING;
	m_pSortFunc = nullptr;
}

//____ Destructor _____________________________________________________________

WgQuickList::~WgQuickList()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgQuickList::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgList::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgQuickList::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgQuickListPtr WgQuickList::Cast( const WgObjectPtr& pObj )
{
	if( pObj && pObj->IsInstanceOf(CLASSNAME) )
		return WgQuickListPtr( static_cast<WgQuickList*>(pObj.GetRealPtr()) );

	return nullptr;
}

//____ AddWidget() ____________________________________________________________

WgQuickListHookPtr WgQuickList::AddWidget( const WgWidgetPtr& pWidget )
{
	WgQuickListHook * pHook = m_hooks.Add();
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}


//____ InsertWidget() ____________________________________________________________

WgQuickListHookPtr WgQuickList::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	int index = 0;
	if( !pSibling )
		index = m_hooks.Size()-1;
	else 
	{
		WgQuickListHook * pHook = static_cast<WgQuickListHook*>(pSibling->_hook());
		if( pHook && pHook->_parent() == this )
			index = m_hooks.Index(pHook);
		else
			return nullptr;
	}

	WgQuickListHook * pHook = m_hooks.Insert(index);
	pHook->m_pParent = this;
	pHook->_setWidget(pWidget.GetRealPtr());

	_onWidgetAppeared( pHook );
	return pHook;
}

//____ InsertWidgetSorted() ___________________________________________________

WgQuickListHookPtr WgQuickList::InsertWidgetSorted( const WgWidgetPtr& pWidget )
{
	//TODO: Implement!

	return nullptr;
}

//____ RemoveWidget() _________________________________________________________

bool WgQuickList::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->_hook() )
		return false;

	WgQuickListHook * pHook = static_cast<WgQuickListHook*>(pWidget->_hook());
	if( pHook->_parent() != this )
		return false;

	int index = m_hooks.Index(pHook);
	_onWidgetDisappeared( pHook );
	m_hooks.Remove(index);
	return true;
}

//____ Clear() ________________________________________________________________

bool WgQuickList::Clear()
{
	m_hooks.Clear();
	_onRefresh();
	return true;
}

//____ SetOrientation() _______________________________________________________

void WgQuickList::SetOrientation( WgOrientation orientation )
{
	bool bHorizontal = (orientation == WG_HORIZONTAL);

	if( bHorizontal != m_bHorizontal )
	{
		m_bHorizontal = bHorizontal;
		_onRefresh();
	}
}

//____ SortWidgets() __________________________________________________________

void WgQuickList::SortWidgets()
{
	//TODO: Implement!!!
}

//____ SetSortOrder() _________________________________________________________

void WgQuickList::SetSortOrder( WgSortOrder order )
{
	//TODO: Implement!!!
}

//____ SetSortFunction() ______________________________________________________

void WgQuickList::SetSortFunction( WgWidgetSortFunc pSortFunc )
{
	//TODO: Implement!!!
}

//____ PreferredSize() ________________________________________________________

WgSize WgQuickList::PreferredSize() const
{
	return m_preferredSize;
}


void WgQuickList::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
}

void WgQuickList::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
}

void WgQuickList::_onCloneContent( const WgWidget * _pOrg )
{
}

void WgQuickList::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
}

void WgQuickList::_onNewSize( const WgSize& size )
{
}

void WgQuickList::_onRefresh()
{
}

void WgQuickList::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
}

void WgQuickList::_onStateChanged( WgState oldState, WgState newState )
{

}

void WgQuickList::_onRequestRender( WgQuickListHook * pHook )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	_requestRender(geo);
}

void WgQuickList::_onRequestRender( WgQuickListHook * pHook, const WgRect& rect )
{
	WgRect geo;
	_getChildGeo(geo, pHook);
	geo.x += rect.x;
	geo.y += rect.y;
	geo.w = rect.w;
	geo.h = rect.h;
	_requestRender(geo);
}

void WgQuickList::_onRequestResize( WgQuickListHook * pHook )
{
	if( pHook->m_bVisible )
	{


	}

}

void WgQuickList::_onWidgetAppeared( WgQuickListHook * pInserted )
{
}

void WgQuickList::_onWidgetDisappeared( WgQuickListHook * pToBeRemoved )
{
}

WgWidget * WgQuickList::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	//TODO: Implement!!!

	return 0;
}

//____ _getChildGeo() _________________________________________________________

void WgQuickList::_getChildGeo( WgRect& geo, const WgHook * _pHook ) const
{
	const WgQuickListHook * pHook = static_cast<const WgQuickListHook*>(_pHook);

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

WgHook* WgQuickList::_firstHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(0);

	return nullptr;
}

//____ _lastHook() ____________________________________________________________

WgHook* WgQuickList::_lastHook() const
{
	if( m_hooks.Size() > 0 )
		return m_hooks.Hook(m_hooks.Size()-1);

	return nullptr;
}

//____ _firstHookWithGeo() ____________________________________________________

WgHook* WgQuickList::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return nullptr;

	WgHook * p = m_hooks.Hook(0);
	_getChildGeo(geo,p);
	return p;
}

//____ _nextHookWithGeo() _____________________________________________________

WgHook* WgQuickList::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgQuickListHook * p = m_hooks.Next(static_cast<WgQuickListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}

//_____ _lastHookWithGeo() ____________________________________________________

WgHook* WgQuickList::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hooks.Size() == 0 )
		return nullptr;

	WgHook * p = m_hooks.Hook(m_hooks.Size()-1);
	_getChildGeo(geo,p);
	return p;
}

//____ _prevHookWithGeo() _____________________________________________________

WgHook* WgQuickList::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	WgQuickListHook * p = m_hooks.Prev(static_cast<WgQuickListHook*>(pHook));
	if( p )
		_getChildGeo(geo,p);
	return p;
}
