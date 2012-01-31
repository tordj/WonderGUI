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

#include <wg_orderedlayout.h>


WgCoord WgOrderedHook::Pos() const
{
	return Parent()->_hookGeo(this).Pos();
}

WgSize WgOrderedHook::Size() const
{
	return Parent()->_hookGeo(this).Size();
}

WgRect WgOrderedHook::Geo() const
{
	return Parent()->_hookGeo(this);
}

WgCoord WgOrderedHook::ScreenPos() const
{
	return Parent()->_hookGeo(this).Pos() + Parent()->ScreenPos();

}

WgRect WgOrderedHook::ScreenGeo() const
{
	return Parent()->_hookGeo(this) + Parent()->ScreenPos();
}

bool WgOrderedHook::MoveUp()
{
	if( _moveUp() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveDown()
{
	if( _moveDown() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveBefore( WgOrderedHook * pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveBefore(pSibling) )
		{
			Parent()->_onGizmosReordered();
			return true;
		}
	}
	return false;
}

bool WgOrderedHook::MoveAfter( WgOrderedHook * pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveAfter(pSibling) )
		{
			Parent()->_onGizmosReordered();
			return true;
		}
	}
	return false;
}

bool WgOrderedHook::MoveFirst()
{
	if( _moveFirst() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveLast()
{
	if( _moveLast() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::SetHidden( bool bHide )
{
	if( bHide != m_bHidden )
	{
		m_bHidden = bHide;
		if( bHide )
			Parent()->_onGizmoDisappeared(this);
		else
			Parent()->_onGizmoAppeared(this);
	}

	return true;
}

WgOrderedLayout * WgOrderedHook::Parent() const
{
	return static_cast<WgOrderedLayout*>(_parent());
}


WgWidget* WgOrderedHook::GetRoot()
{
	WgHook * p = Parent()->Hook();
	if( p )
		return p->GetRoot();
	else
		return 0;
}

WgOrderedHook::WgOrderedHook()
{
}

WgOrderedHook::~WgOrderedHook()
{
}


void WgOrderedHook::_requestRender()
{
	WgOrderedLayout * p = Parent();

	p->_onRenderRequested(this);
}

void WgOrderedHook::_requestRender( const WgRect& rect )
{
	Parent()->_onRenderRequested(this, rect);
}

void WgOrderedHook::_requestResize()
{
	Parent()->_onResizeRequested(this);
}


WgHook * WgOrderedHook::_prevHook() const
{
	return _prev();
}

WgHook * WgOrderedHook::_nextHook() const
{
	return _next();
}

//____ Constructor() __________________________________________________________

WgOrderedLayout::WgOrderedLayout() : m_sortOrder(WG_SORT_ASCENDING), m_pSortFunc(0), m_nChildContainers(0)
{

}


//____ Destructor() ___________________________________________________________

WgOrderedLayout::~WgOrderedLayout()
{
}


//____ AddChild() _____________________________________________________________

WgOrderedHook * WgOrderedLayout::AddChild( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return 0;

	WgOrderedHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_attachGizmo( pGizmo );

	_onGizmoAppeared(pHook);
	return pHook;
}

//____ InsertChild() __________________________________________________________

WgOrderedHook * WgOrderedLayout::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling )
{
	if( !pGizmo || !pSibling || !pSibling->ParentX() || pSibling->ParentX() != this )
		return 0;

	WgOrderedHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgOrderedHook*>(pSibling->Hook()));

	pHook->_attachGizmo( pGizmo );
	
	_onGizmoAppeared(pHook);
	return pHook;
}

//____ InsertChildSorted() ____________________________________________________

WgOrderedHook * WgOrderedLayout::InsertChildSorted( WgGizmo * pGizmo )
{
	//TODO: Implement

	return 0;
}

//____ DeleteChild() __________________________________________________________

bool WgOrderedLayout::DeleteChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that gizmo has disappeared

	WgOrderedHook * pHook = (WgOrderedHook *) pGizmo->Hook();
	pHook->_disconnect();

	if( !pHook->Hidden() )
		_onGizmoDisappeared( pHook );

	// Delete the gizmo and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgOrderedLayout::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return 0;

	// Disconnect and notify subclass that gizmo has disappeared

	WgOrderedHook * pHook = (WgOrderedHook *) pGizmo->Hook();
	pHook->_disconnect();

	if( !pHook->Hidden() )
		_onGizmoDisappeared( pHook );

	return pGizmo;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgOrderedLayout::DeleteAllChildren()
{
	m_hooks.Clear();
	_refreshAllGizmos();

	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgOrderedLayout::ReleaseAllChildren()
{
	while( m_hooks.First() )
		m_hooks.PopFront();

	_refreshAllGizmos();

	return true;
}

//____ SortChildren() ___________________________________________________________

void WgOrderedLayout::SortChildren()
{
	//TODO: Implement
}

//____ SetSortOrder() _________________________________________________________

void WgOrderedLayout::SetSortOrder( WgSortOrder order )
{
	m_sortOrder = order;
}

//____ SetSortFunction() ______________________________________________________

void WgOrderedLayout::SetSortFunction( WgGizmoSortFunc pSortFunc )
{
	m_pSortFunc = pSortFunc;
}

//____ _onCloneContent() ______________________________________________________

void WgOrderedLayout::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement
}
