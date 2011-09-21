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
	return Parent()->_hookGeo(this).pos();
}

WgSize WgOrderedHook::Size() const
{
	return Parent()->_hookGeo(this).size();
}

WgRect WgOrderedHook::Geo() const
{
	return Parent()->_hookGeo(this);
}

WgCoord WgOrderedHook::ScreenPos() const
{
	return Parent()->_hookGeo(this).pos() + Parent()->ScreenPos();

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

void WgOrderedHook::SetHidden( bool bHide )
{
	if( bHide != m_bHidden )
	{
		m_bHidden = bHide;
		if( bHide )
			Parent()->_onGizmoDisappeared(this);
		else
			Parent()->_onGizmoAppeared(this);
	}
}

WgOrderedLayout * WgOrderedHook::Parent() const
{
	return static_cast<WgOrderedLayout*>(_parent());
}


WgWidget* WgOrderedHook::GetRoot()
{
	WgGizmoHook * p = Parent()->Hook();
	if( p )
		return p->GetRoot();
	else
		return 0;
}

WgOrderedHook::WgOrderedHook() : m_bHidden(false)
{
}

WgOrderedHook::~WgOrderedHook()
{
}


void WgOrderedHook::RequestRender()
{
	WgOrderedLayout * p = Parent();

	p->_onRenderRequested(this);
}

void WgOrderedHook::RequestRender( const WgRect& rect )
{
	Parent()->_onRenderRequested(this, rect);
}

void WgOrderedHook::RequestResize()
{
	Parent()->_onResizeRequested(this);
}


WgGizmoHook * WgOrderedHook::_prevHook() const
{
	return _prev();
}

WgGizmoHook * WgOrderedHook::_nextHook() const
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

//____ ScrollIntoView() _______________________________________________________

void WgOrderedLayout::ScrollIntoView( WgGizmo * pGizmo )
{
	//TODO: Implement
}

//____ FindGizmo() ____________________________________________________________

WgGizmo * WgOrderedLayout::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	WgOrderedHook * pHook = m_hooks.First();
	WgGizmo * 		pResult = 0;
	WgRect			rect = _hookGeo(pHook);


	while( pHook && !pResult )
	{
		if( rect.contains( ofs ) )
		{
			WgGizmo * pGizmo = pHook->Gizmo();
			if( pGizmo->IsContainer() )
			{
				pResult = pGizmo->CastToContainer()->FindGizmo( ofs - rect.pos(), mode );
			}
			else
			{
				switch( mode )
				{
					case WG_SEARCH_ACTION_TARGET:
					case WG_SEARCH_MARKPOLICY:
						if( pGizmo->MarkTest( ofs - rect.pos() ) )
							pResult = pGizmo;
						break;
					case WG_SEARCH_GEOMETRY:
						pResult = pGizmo;
						break;
				}
			}
		}
		pHook = pHook->Next();

		if( pHook )
			_advanceGeoToHook( rect, pHook );
	}

	if( !pResult && mode == WG_SEARCH_GEOMETRY )
		pResult = this;

	return pResult;


}

//____ _onCloneContent() ______________________________________________________

void WgOrderedLayout::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement
}

//____ _onRender() ____________________________________________________________

void WgOrderedLayout::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	//Do nothing, we are a container.
}

//____ _castDirtyRect() _______________________________________________________

void WgOrderedLayout::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
}

//____ _renderDirtyRects() ____________________________________________________

void WgOrderedLayout::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{

}

//____ _clearDirtyRects() _____________________________________________________

void WgOrderedLayout::_clearDirtyRects()
{
	m_dirt.Clear();

	WgOrderedHook *pHook	= m_hooks.First();

	while( pHook )
	{
		if( pHook->Gizmo()->IsContainer() )
			pHook->Gizmo()->CastToContainer()->_clearDirtyRects();

		pHook = pHook->Next();
	}
}

//____ _onAlphaTest() _________________________________________________________

bool WgOrderedLayout::_onAlphaTest( const WgCoord& ofs )
{
	return false;
}
