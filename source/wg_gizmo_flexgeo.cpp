/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_gizmo_flexgeo.h>
#include <wg_patches.h>
#include <wg_util.h>

static const char	c_gizmoType[] = {"FlexGeo"};

WgFlexAnchor		WgGizmoFlexGeo::g_baseAnchors[9] = { WgFlexAnchor(0.f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 0.5f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.f, 0.5f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 0.5f, WgCoord(0,0)) };

//____ WgFlexHook::Constructor ________________________________________________

WgFlexHook::WgFlexHook( WgGizmoFlexGeo * pParent, const WgRect& placementGeo ) : m_pParent(pParent), m_bHidden(false),
	m_bFloating(false), m_sizePolicy(WG_SIZE_SPECIFIED),
	m_minSize(0,0), m_maxSize(65536,65536), m_anchor(WG_NORTHWEST), m_hotspot(WG_NORTHWEST),
	m_placementGeo(placementGeo), m_anchorTopLeft(WG_NORTHWEST),
	m_anchorBottomRight(WG_SOUTHEAST), m_borders(0)
{
}

//____ WgFlexHook::SetAnchored() ______________________________________________

bool WgFlexHook::SetAnchored()
{
	// Return to old anchors.

	m_bFloating = false;
	_refreshRealGeo();
	return true;
}

bool  WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	int nbAnchors = m_pParent->NbAnchors();
	if( anchorTopLeft >= nbAnchors || anchorBottomRight >= nbAnchors )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;
	m_borders			= borders;

	_refreshRealGeo();
	return true;
}


//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating()
{
	// Return to old floating position and size.

	m_bFloating = true;
	_refreshRealGeo();
	return true;
}

bool WgFlexHook::SetFloating( const WgCoord& pos, WgOrientation origo )
{
	return SetFloating( pos, origo, origo );
}

bool WgFlexHook::SetFloating( const WgCoord& pos, int anchor, WgOrientation hotspot )
{
	if( anchor >= m_pParent->NbAnchors() )
		return false;

	m_sizePolicy	= WG_SIZE_DEFAULT;
	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo.SetPos(pos);

	_limitPlacementSize();
	_refreshRealGeo();
	return true;
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgOrientation origo )
{
	return SetFloating( geometry, origo, origo );
}

bool WgFlexHook::SetFloating( const WgRect& geometry, int anchor, WgOrientation hotspot )
{
	if( anchor >= m_pParent->NbAnchors() )
		return false;

	m_sizePolicy	= WG_SIZE_SPECIFIED;
	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo	= geometry;

	_limitPlacementSize();
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::Top() ______________________________________________________

void WgFlexHook::Top()
{
	MoveOver( _chain()->Last() );
}

//____ WgFlexHook::Bottom() ___________________________________________________

void WgFlexHook::Bottom()
{
	MoveUnder( _chain()->First() );
}

//____ WgFlexHook::Up() _______________________________________________________

bool WgFlexHook::Up()
{
	WgFlexHook * pNext = _next();

	if( pNext == 0 )
		return false;

	// Get area potentially exposed

	WgRect cover( m_realGeo, pNext->m_realGeo );

	// Move up

	_moveUp();

	// Request render on potentially exposed area.

	if( cover.w > 0 && cover.h > 0 )
	{
		m_pParent->_onRequestRender( cover, this );
	}

	return true;
}

//____ WgFlexHook::Down() _____________________________________________________

bool WgFlexHook::Down()
{
	WgFlexHook * pPrev = _prev();

	if( pPrev == 0 )
		return false;

	// Get area of sibling potentially exposed

	WgRect cover( m_realGeo, pPrev->m_realGeo );

	// Move down

	_moveDown();

	// Request render on potentially exposed area.

	if( cover.w > 0 && cover.h > 0 )
	{
		m_pParent->_onRequestRender( cover, pPrev );
	}

	return true;
}

//____ WgFlexHook::MoveOver() _________________________________________________

bool WgFlexHook::MoveOver( WgFlexHook * pSibling )
{
	if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
		return false;

	WgFlexHook * pFirst = _chain()->First();
	while( pFirst != this && pFirst != pSibling )
		pFirst = pFirst->_next();

	WgFlexHook * pLast = pFirst->_next();
	while( pLast != this && pLast != pSibling )
		pLast = pLast->_next();

	if( pFirst == this )			// Move us forward
	{
		WgFlexHook * p = pFirst->_next();

		_moveAfter( pSibling );

		// Request render on all areas covered by siblings we have skipped in front of.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, this );

			p = p->_next();
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = pLast->_prev();
		_moveAfter( pSibling );

		// Request render on our siblings for the area we previously have covered.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, p );

			p = p->_prev();
		}
	}

	return true;
}

//____ WgFlexHook::MoveUnder() ________________________________________________

bool WgFlexHook::MoveUnder( WgFlexHook * pSibling )
{
	if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
		return false;

	WgFlexHook * pFirst = _chain()->First();
	while( pFirst != this && pFirst != pSibling )
		pFirst = pFirst->_next();

	WgFlexHook * pLast = pFirst->_next();
	while( pLast != this && pLast != pSibling )
		pLast = pLast->_next();

	if( pFirst == this )			// Move us forward
	{
		WgFlexHook * p = _next();
		_moveBefore( pSibling );

		// Request render on all areas covered by siblings we have skipped in front of.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, this );

			p = p->_next();
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = _prev();
		_moveBefore( pSibling );

		// Request render on our siblings for the area we previously have covered.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, p );

			p = p->_prev();
		}
	}

	return true;
}

//____ WgFlexHook::Hide() _____________________________________________________

void WgFlexHook::Hide()
{
	if( !m_bHidden )
	{
		m_pParent->_onRequestRender( m_realGeo, this );
		m_bHidden = true;
	}
}

//____ WgFlexHook::Show() _____________________________________________________

void WgFlexHook::Show()
{
	if( m_bHidden )
	{
		m_bHidden = false;
		m_pParent->_onRequestRender( m_realGeo, this );
	}
}

//____ WgFlexHook::SetAnchor() _________________________________________________

bool WgFlexHook::SetAnchor( int anchor )
{
	if( anchor >= m_pParent->NbAnchors() )
		return false;

	if( anchor != m_anchor )
	{
		m_anchor = anchor;
		_refreshRealGeo();
	}

	return true;
}

//____ WgFlexHook::SetHotspot() _______________________________________________

bool WgFlexHook::SetHotspot( WgOrientation hotspot )
{
	if( hotspot != m_hotspot )
	{
		m_hotspot = hotspot;
		_refreshRealGeo();
	}

	return true;
}

//____ WgFlexHook::SetSizePolicy() ____________________________________________

bool WgFlexHook::SetSizePolicy( WgSizePolicy policy )
{
	if( policy != m_sizePolicy )
	{
		m_sizePolicy = policy;
		if( m_bFloating )
			_refreshRealGeo();
	}
	return true;
}

//____ WgFlexHook::SetMinSize() _______________________________________________

bool WgFlexHook::SetMinSize( const WgSize& size )
{
	if( size.w < 0 || size.h < 0 || size.w > m_maxSize.w || size.h > m_maxSize.h )
		return false;

	m_minSize = size;
	_limitPlacementSize();
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetMaxSize() _______________________________________________

bool WgFlexHook::SetMaxSize( const WgSize& size )
{
	if( size.w < m_minSize.w || size.h < m_minSize.h )
		return false;

	m_maxSize = size;
	_limitPlacementSize();
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetGeo() ___________________________________________________

bool WgFlexHook::SetGeo( const WgRect& geometry )
{
	m_sizePolicy = WG_SIZE_SPECIFIED;
	m_placementGeo = geometry;
	bool ret = _limitPlacementSize();				// Return false if size of geometry was affected by limits.
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			ret = false;							// Return false if we could not get (exactly) the requested geometry.
	}
	return ret;
}

//____ WgFlexHook::SetOfs() ___________________________________________________

bool WgFlexHook::SetOfs( const WgCoord& ofs )
{
	m_placementGeo.x = ofs.x;
	m_placementGeo.y = ofs.y;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetOfsX() __________________________________________________

bool WgFlexHook::SetOfsX( int x )
{
	m_placementGeo.x = x;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetOfsY() __________________________________________________

bool WgFlexHook::SetOfsY( int y )
{
	m_placementGeo.y = y;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetSize() __________________________________________________

bool WgFlexHook::SetSize( const WgSize& size )
{
	m_sizePolicy = WG_SIZE_SPECIFIED;
	m_placementGeo.w = size.w;
	m_placementGeo.h = size.h;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetWidth() _________________________________________________

bool WgFlexHook::SetWidth( int width )
{
	m_sizePolicy = WG_SIZE_SPECIFIED;

	//TODO: Set height to something that makes sense...
	m_placementGeo.w = width;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetHeight() ________________________________________________

bool WgFlexHook::SetHeight( int height )
{
	m_sizePolicy = WG_SIZE_SPECIFIED;

	//TODO: Set width to something that makes sense...
	m_placementGeo.h = height;
	if( m_bFloating )
	{
		if( !_refreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::Move() _____________________________________________________

bool WgFlexHook::Move( const WgCoord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->Position(m_pParent->Size());
	m_placementGeo += ofs;

	if( !_refreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;
}

//____ WgFlexHook::MoveX() ____________________________________________________

bool WgFlexHook::MoveX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->Position(m_pParent->Size());
	m_placementGeo.x += x;

	if( !_refreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;}

//____ WgFlexHook::MoveY() ____________________________________________________

bool WgFlexHook::MoveY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->Position(m_pParent->Size());
	m_placementGeo.y += y;

	if( !_refreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;
}

//____ WgFlexHook::ScreenPos() ________________________________________________

WgCoord WgFlexHook::ScreenPos() const
{
	return m_realGeo.Pos() + m_pParent->ScreenPos();
}

//____ WgFlexHook::ScreenGeo() ________________________________________________

WgRect WgFlexHook::ScreenGeo() const
{
	return m_realGeo + m_pParent->ScreenPos();
}

//____ WgFlexHook::_prevHook() ________________________________________________

WgHook * WgFlexHook::_prevHook() const
{
	return _prev();
}

//____ WgFlexHook::_nextHook() ________________________________________________

WgHook * WgFlexHook::_nextHook() const
{
	return _next();
}

//____ WgFlexHook::_parent() __________________________________________________

WgGizmoParent * WgFlexHook::_parent() const
{
	return m_pParent;
}

//____ WgFlexHook::GetRoot() __________________________________________________

WgWidget* WgFlexHook::GetRoot()
{
	WgHook * p = m_pParent->Hook();
	if( p )
		return p->GetRoot();
	else
		return 0;
}

//____ WgFlexHook::_limitPlacementSize() _______________________________________

bool WgFlexHook::_limitPlacementSize()
{
	bool ret = true;

	if( m_placementGeo.w > m_maxSize.w )
	{
		m_placementGeo.w = m_maxSize.w;
		ret = false;
	}

	if( m_placementGeo.h > m_maxSize.h )
	{
		m_placementGeo.h = m_maxSize.h;
		ret = false;
	}

	if( m_placementGeo.w < m_minSize.w )
	{
		m_placementGeo.w = m_minSize.w;
		ret = false;
	}

	if( m_placementGeo.h < m_minSize.h )
	{
		m_placementGeo.h = m_minSize.h;
		ret = false;
	}

	return ret;
}

//____ WgFlexHook::_refreshRealGeo() ___________________________________________

bool WgFlexHook::_refreshRealGeo()
{
	WgRect	newGeo;
	WgSize	parentSize = m_pParent->Size();


	if( m_bFloating )
	{
		// Calculate size

		WgSize sz;

		switch( m_sizePolicy )
		{
			case WG_SIZE_SPECIFIED:
				sz = m_placementGeo.Size();
				break;
			case WG_SIZE_MIN:
				sz = m_minSize;
				break;
			case WG_SIZE_DEFAULT:
				sz = m_pGizmo->DefaultSize();

				if( sz.w > m_maxSize.w )
				{
					sz.w = m_maxSize.w;
					sz.h = m_pGizmo->HeightForWidth(m_maxSize.w);
				}

				break;
			case WG_SIZE_MAX:
				sz = m_maxSize;
				break;
		};


		if( sz.w < m_minSize.w )
			sz.w = m_minSize.w;
		if( sz.h < m_minSize.h )
			sz.h = m_minSize.h;

		if( sz.w > m_maxSize.w )
			sz.w = m_maxSize.w;
		if( sz.h > m_maxSize.h )
			sz.h = m_maxSize.h;

		// Calculate position

		WgCoord pos = m_pParent->Anchor(m_anchor)->Position( parentSize );	// Anchor,
		pos -= WgUtil::OrientationToOfs( m_hotspot, sz );						// hotspot
		pos += m_placementGeo.Pos();										// and Offset.

		// Limit size/pos according to parent

		if( m_pParent->IsConfiningChildren() )
		{
			if( sz.w > parentSize.w )
				sz.w = parentSize.w;
			if( sz.h > parentSize.h )

			if( pos.x + sz.w > parentSize.w )
				pos.x = parentSize.w - sz.w;

			if( pos.y + sz.h > parentSize.h )
				pos.y = parentSize.h - sz.h;
		}

		newGeo = WgRect( pos, sz );
	}
	else
	{
		WgCoord topLeft = m_pParent->Anchor(m_anchorTopLeft)->Position( parentSize );
		WgCoord bottomRight = m_pParent->Anchor(m_anchorBottomRight)->Position( parentSize );

		newGeo = WgRect(topLeft,bottomRight);
		newGeo.Shrink( m_borders );
	}

	// Return now if there was no change

	if( newGeo == m_realGeo )
		return true;

	// Request render and update positions.

	_requestRender();
	m_realGeo = newGeo;
	m_pGizmo->_onNewSize(newGeo);
	_requestRender();

	//

	return true;

}

//____ WgFlexHook::_requestRender() ____________________________________________

void WgFlexHook::_requestRender()
{
	m_pParent->_onRequestRender( m_realGeo, this );
}

//____ WgFlexHook::_requestRender() ____________________________________________

void WgFlexHook::_requestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_realGeo.Pos(), this );
}

//____ WgFlexHook::_requestResize() ____________________________________________

void WgFlexHook::_requestResize()
{
	if( m_sizePolicy != WG_SIZE_SPECIFIED )
		_refreshRealGeo();
}


//____ Constructor ____________________________________________________________

WgGizmoFlexGeo::WgGizmoFlexGeo() : m_bConfineChildren(false)
{
	m_bSiblingsOverlap = true;
}

//____ Destructor _____________________________________________________________

WgGizmoFlexGeo::~WgGizmoFlexGeo()
{
// This is done automatically now, calls like these in destructor are forbidden.

//	DeleteAllGizmos();
//	DeleteAllAnchors();
}

//____ Type() _________________________________________________________________

const char * WgGizmoFlexGeo::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoFlexGeo::GetMyType()
{
	return c_gizmoType;
}

//____ SetConfineChildren() ___________________________________________________

void WgGizmoFlexGeo::SetConfineChildren( bool bConfineChildren )
{
	if( bConfineChildren != m_bConfineChildren )
	{
		m_bConfineChildren = bConfineChildren;

		WgFlexHook * pHook = m_hooks.First();
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->Next();
		}
	}
}



//____ AddChild() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetFloating();
	return p;
}

//____ AddChild() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo, const WgCoord& pos, WgOrientation origo )
{
	return AddChild( pGizmo, pos, origo, origo );
}

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo, const WgCoord& pos, int anchor, WgOrientation hotspot )
{
	if( !pGizmo )
		return 0;

	WgSize bestSize = pGizmo->DefaultSize();

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,bestSize) );
	p->_attachGizmo( pGizmo );
	m_hooks.PushBack(p);
	p->SetFloating( pos, anchor, hotspot );
	return p;
}

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo, const WgRect& geometry, WgOrientation origo )
{
	return AddChild( pGizmo, geometry, origo, origo );
}

WgFlexHook * WgGizmoFlexGeo::AddChild( WgGizmo * pGizmo, const WgRect& geometry, int anchor, WgOrientation hotspot )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetFloating( geometry, anchor, hotspot );
	return p;
}


//____ InsertChild() _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating();
	return p;
}


WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}


WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgOrientation origo )
{
	return InsertChild( pGizmo, pSibling, geometry, origo, origo );
}

WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, int anchor, WgOrientation hotspot )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( geometry, anchor, hotspot );
	return p;
}

WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, WgOrientation origo )
{
	return InsertChild( pGizmo, pSibling, pos, origo, origo );
}

WgFlexHook * WgGizmoFlexGeo::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCoord& pos, int anchor, WgOrientation hotspot )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->DefaultSize()) );
	p->_attachGizmo( pGizmo );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( pos, anchor, hotspot );
	return p;
}




//____ DeleteChild() _________________________________________________

bool WgGizmoFlexGeo::DeleteChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return false;

	// Force rendering of the area the gizmo was covering

	WgFlexHook * pHook = (WgFlexHook *) pGizmo->Hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Delete the gizmo and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgGizmoFlexGeo::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return 0;

	// Force rendering of the area the gizmo was covering

	WgFlexHook * pHook = (WgFlexHook *) pGizmo->Hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Delete the gizmo and return

	pHook->_releaseGizmo();
	delete pHook;
	return pGizmo;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgGizmoFlexGeo::DeleteAllChildren()
{
	WgPatches	dirt;

	// Collect dirty areas and delete hooks, taking any connected gizmos with them.

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		dirt.Add( pHook->m_realGeo );
		WgFlexHook * pDelete = pHook;
		pHook = pHook->Next();
		delete pDelete; 
	}

	// RequestRender on all dirty patches

	for( const WgRect * pRect = dirt.Begin() ; pRect != dirt.End() ; pRect++ ) 
		RequestRender( * pRect );

	return true;
}

//____ ReleaseAllChildren() ______________________________________________________

bool WgGizmoFlexGeo::ReleaseAllChildren()
{
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		pHook->_releaseGizmo();
		pHook = pHook->Next();
	}

	DeleteAllChildren();	// Will only delete the hooks and request render on dirty areas since
							// we already have disconnected the children.
	return true;
}

//____ AddAnchor() ____________________________________________________________

int WgGizmoFlexGeo::AddAnchor( float relativeX, float relativeY, const WgCoord& pixelOfs )
{
	m_anchors.push_back( WgFlexAnchor( relativeX, relativeY, pixelOfs ) );
	return m_anchors.size()+9-1;
}

//____ ReplaceAnchor() ________________________________________________________

bool WgGizmoFlexGeo::ReplaceAnchor( int index, float relativeX, float relativeY, const WgCoord& pixelOfs )
{
	if( index < 9 || index >= NbAnchors() )
		return false;

	m_anchors[index-9] = WgFlexAnchor( relativeX, relativeY, pixelOfs );

	// Update geometry for all gizmos using this anchor.

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		if( (pHook->m_bFloating && pHook->m_anchor == index) ||
			(!pHook->m_bFloating && (pHook->m_anchorBottomRight == index || pHook->m_anchorTopLeft == index)) )
			pHook->_refreshRealGeo();

		pHook = pHook->Next();
	}

	return true;
}

//____ DeleteAnchor() _________________________________________________________

bool WgGizmoFlexGeo::DeleteAnchor( int index )
{
	if( index < 9 || index >= NbAnchors() )
		return false;


	m_anchors.erase( m_anchors.begin()+(index-9) );

	// Update hooks with affected anchors. Save list of affected hooks for later
	// update of geometry since we need to update all anchors first.

	std::vector<WgFlexHook*>	vNeedsUpdate;
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		// Check if this gizmo will have its geometry altered.

		if( (pHook->m_bFloating && pHook->m_anchor == index) ||
			(!pHook->m_bFloating && (pHook->m_anchorTopLeft == index || pHook->m_anchorBottomRight == index)) )
			vNeedsUpdate.push_back(pHook);

		// Update hooks anchor references.

		if( pHook->m_anchor == index )
			pHook->m_anchor = WG_NORTHWEST;
		else if( pHook->m_anchor > index )
			pHook->m_anchor--;

		if( pHook->m_anchorTopLeft == index )
			pHook->m_anchorTopLeft = WG_NORTHWEST;
		else if( pHook->m_anchorTopLeft > index )
			pHook->m_anchorTopLeft--;

		if( pHook->m_anchorBottomRight == index )
			pHook->m_anchorBottomRight = WG_SOUTHEAST;
		else if( pHook->m_anchorBottomRight > index )
			pHook->m_anchorBottomRight--;

		pHook = pHook->Next();
	}

	// Update geometry for all affected hooks.

	for( unsigned int i = 0 ; i < vNeedsUpdate.size() ; i++ )
		vNeedsUpdate[i]->_refreshRealGeo();

	return true;
}

//____ DeleteAllAnchors() _____________________________________________________

void WgGizmoFlexGeo::DeleteAllAnchors()
{
	if( m_anchors.empty() )
		return;


	m_anchors.clear();

	// Update hooks with affected anchors. Save list of affected hooks for later
	// update of geometry since we need to update all anchors first.

	std::vector<WgFlexHook*>	vNeedsUpdate;
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		// Check if this gizmo will have its geometry altered.

		if( (pHook->m_bFloating && pHook->m_anchor >= 9) ||
			(!pHook->m_bFloating && (pHook->m_anchorTopLeft >= 9 || pHook->m_anchorBottomRight >= 9)) )
			vNeedsUpdate.push_back(pHook);

		// Update hooks anchor references.

		if( pHook->m_anchor >= 9 )
			pHook->m_anchor = WG_NORTHWEST;

		if( pHook->m_anchorTopLeft >= 9 )
			pHook->m_anchorTopLeft = WG_NORTHWEST;

		if( pHook->m_anchorBottomRight >= 9 )
			pHook->m_anchorBottomRight = WG_SOUTHEAST;

		pHook = pHook->Next();
	}

	// Update geometry for all affected hooks.

	for( unsigned int i = 0 ; i < vNeedsUpdate.size() ; i++ )
		vNeedsUpdate[i]->_refreshRealGeo();
}




//____ Anchor() _______________________________________________________________

const WgFlexAnchor * WgGizmoFlexGeo::Anchor( int index )
{
	if( index < 9 )
		return &g_baseAnchors[index];
	else
	{
		index -= 9;
		if( index < (int) m_anchors.size() )
			return &m_anchors[index];
	}
	return 0;
}

//____ DefaultSize() _____________________________________________________________

WgSize WgGizmoFlexGeo::DefaultSize() const
{
	//TODO: FIX THIS! DEFAULT SIZE SHOULD BE WHAT NEEDED TO FIT ALL IN!
	
	return WgSize(0,0);		// No recommendation.
}

//____ FindGizmo() ____________________________________________________________

WgGizmo * WgGizmoFlexGeo::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	WgFlexHook * pHook = m_hooks.Last();
	WgGizmo * pResult = 0;

	while( pHook && !pResult )
	{
		if( pHook->m_realGeo.Contains( ofs ) )
		{
			if( pHook->Gizmo()->IsContainer() )
			{
				pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - pHook->m_realGeo.Pos(), mode );
			}
			else
			{
				switch( mode )
				{
					case WG_SEARCH_ACTION_TARGET:
					case WG_SEARCH_MARKPOLICY:
						if( pHook->Gizmo()->MarkTest( ofs - pHook->m_realGeo.Pos() ) )
							pResult = pHook->Gizmo();
						break;
					case WG_SEARCH_GEOMETRY:
						pResult = pHook->Gizmo();
						break;
				}
			}
		}
		pHook = pHook->Prev();
	}

	if( !pResult && mode == WG_SEARCH_GEOMETRY )
		pResult = this;

	return pResult;
}

//____ _onRequestRender() ______________________________________________________

void WgGizmoFlexGeo::_onRequestRender( const WgRect& rect, const WgFlexHook * pHook )
{
	if( pHook->IsHidden() )
		return;

	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,Size())) );

	// Remove portions of patches that are covered by opaque upper siblings

	WgFlexHook * pCover = pHook->Next();
	while( pCover )
	{
		if( pCover->m_realGeo.IntersectsWith( rect ) )
			pCover->Gizmo()->_onMaskPatches( patches, pCover->m_realGeo, WgRect(0,0,65536,65536 ) );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		RequestRender( * pRect );
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoFlexGeo::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement
}

//____ _onNewSize() ____________________________________________________________

void WgGizmoFlexGeo::_onNewSize( const WgSize& size )
{
	WgFlexHook * pHook = m_hooks.Last();

	while( pHook )
	{
		pHook->_refreshRealGeo();
		pHook = pHook->Prev();
	}
}

//____ _onAction() _____________________________________________________________

void WgGizmoFlexGeo::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgGizmoFlexGeo::_firstHookWithGeo( WgRect& writeGeo ) const
{
	WgFlexHook * p = m_hooks.First();
	if( p )
		writeGeo = p->m_realGeo;

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgGizmoFlexGeo::_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const
{
	WgFlexHook * p = ((WgFlexHook*)pHook)->_next();
	if( p )
		writeGeo = p->m_realGeo;
	
	return p;
}

