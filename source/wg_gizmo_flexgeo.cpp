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
#include <wg_rectchain.h>

static const char	c_gizmoType[] = {"FlexGeo"};

WgFlexAnchor		WgGizmoFlexGeo::g_baseAnchors[9] = { WgFlexAnchor(0.f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 0.5f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.f, 0.5f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 0.5f, WgCord(0,0)) };

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
	RefreshRealGeo();
	return true;
}

//____ WgFlexHook::SetAnchored() ______________________________________________

bool WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight )
{
	int nbAnchors = m_pParent->NbAnchors();
	if( anchorTopLeft >= nbAnchors || anchorBottomRight >= nbAnchors )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;

	RefreshRealGeo();
	return true;
}

//____ WgFlexHook::SetAnchored() ______________________________________________

bool  WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	int nbAnchors = m_pParent->NbAnchors();
	if( anchorTopLeft >= nbAnchors || anchorBottomRight >= nbAnchors )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;
	m_borders			= borders;

	RefreshRealGeo();
	return true;
}


//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating()
{
	// Return to old floating position and size.

	m_bFloating = true;
	RefreshRealGeo();
	return true;
}

//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating( const WgRect& geometry )
{
	return SetFloating( geometry, WG_NORTHWEST, WG_NORTHWEST );
}

//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot )
{
	return SetFloating( geometry, hotspot, hotspot );
}

//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot, int anchor )
{
	if( anchor >= m_pParent->NbAnchors() )
		return false;

	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo	= geometry;

	LimitPlacementSize();
	RefreshRealGeo();
	return true;
}

//____ WgFlexHook::Top() ______________________________________________________

void WgFlexHook::Top()
{
	MoveOver( Chain()->Last() );
}

//____ WgFlexHook::Bottom() ___________________________________________________

void WgFlexHook::Bottom()
{
	MoveUnder( Chain()->First() );
}

//____ WgFlexHook::Up() _______________________________________________________

bool WgFlexHook::Up()
{
	WgFlexHook * pNext = Next();

	if( pNext == 0 )
		return false;

	// Get area potentially exposed

	WgRect cover( m_realGeo, pNext->m_realGeo );

	// Move up

	MoveUp();

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
	WgFlexHook * pPrev = Prev();

	if( pPrev == 0 )
		return false;

	// Get area of sibling potentially exposed

	WgRect cover( m_realGeo, pPrev->m_realGeo );

	// Move down

	MoveDown();

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

	WgFlexHook * pFirst = Chain()->First();
	while( pFirst != this && pFirst != pSibling )
		pFirst = pFirst->Next();

	WgFlexHook * pLast = pFirst->Next();
	while( pLast != this && pLast != pSibling )
		pLast = pLast->Next();

	if( pFirst == this )			// Move us forward
	{
		WgFlexHook * p = pFirst->Next();

		MoveAfter( pSibling );

		// Request render on all areas covered by siblings we have skipped in front of.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, this );

			p = p->Next();
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = pLast->Prev();
		MoveAfter( pSibling );

		// Request render on our siblings for the area we previously have covered.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, p );

			p = p->Prev();
		}
	}

	return true;
}

//____ WgFlexHook::MoveUnder() ________________________________________________

bool WgFlexHook::MoveUnder( WgFlexHook * pSibling )
{
	if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
		return false;

	WgFlexHook * pFirst = Chain()->First();
	while( pFirst != this && pFirst != pSibling )
		pFirst = pFirst->Next();

	WgFlexHook * pLast = pFirst->Next();
	while( pLast != this && pLast != pSibling )
		pLast = pLast->Next();

	if( pFirst == this )			// Move us forward
	{
		WgFlexHook * p = Next();
		MoveBefore( pSibling );

		// Request render on all areas covered by siblings we have skipped in front of.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, this );

			p = p->Next();
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = Prev();
		MoveBefore( pSibling );

		// Request render on our siblings for the area we previously have covered.

		while( p != this )
		{
			WgRect cover( m_realGeo, p->m_realGeo );

			if( cover.w > 0 && cover.h > 0 )
				m_pParent->_onRequestRender( cover, p );

			p = p->Prev();
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

//____ WgFlexHook::SetOrigo() _________________________________________________

bool WgFlexHook::SetOrigo( int anchor )
{
	if( anchor >= m_pParent->NbAnchors() )
		return false;

	if( anchor != m_anchor )
	{
		m_anchor = anchor;
		RefreshRealGeo();
	}

	return true;
}

//____ WgFlexHook::SetHotspot() _______________________________________________

bool WgFlexHook::SetHotspot( WgLocation hotspot )
{
	if( hotspot != m_hotspot )
	{
		m_hotspot = hotspot;
		RefreshRealGeo();
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
			RefreshRealGeo();
	}
	return true;
}

//____ WgFlexHook::SetMinSize() _______________________________________________

bool WgFlexHook::SetMinSize( const WgSize& size )
{
	if( size.w < 0 || size.h < 0 || size.w > m_maxSize.w || size.h > m_maxSize.h )
		return false;

	m_minSize = size;
	LimitPlacementSize();
	if( m_bFloating )
		RefreshRealGeo();
	return true;
}

//____ WgFlexHook::SetMaxSize() _______________________________________________

bool WgFlexHook::SetMaxSize( const WgSize& size )
{
	if( size.w < m_minSize.w || size.h < m_minSize.h )
		return false;

	m_maxSize = size;
	LimitPlacementSize();
	if( m_bFloating )
		RefreshRealGeo();
	return true;
}

//____ WgFlexHook::SetGeo() ___________________________________________________

bool WgFlexHook::SetGeo( const WgRect& geometry )
{
	m_placementGeo = geometry;
	bool ret = LimitPlacementSize();				// Return false if size of geometry was affected by limits.
	if( m_bFloating )
	{
		if( !RefreshRealGeo() )
			ret = false;							// Return false if we could not get (exactly) the requested geometry.
	}
	return ret;
}

//____ WgFlexHook::SetOfs() ___________________________________________________

bool WgFlexHook::SetOfs( const WgCord& ofs )
{
	m_placementGeo.x = ofs.x;
	m_placementGeo.y = ofs.y;
	if( m_bFloating )
	{
		if( !RefreshRealGeo() )
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
		if( !RefreshRealGeo() )
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
		if( !RefreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetSize() __________________________________________________

bool WgFlexHook::SetSize( const WgSize& size )
{
	m_placementGeo.w = size.w;
	m_placementGeo.h = size.h;
	if( m_bFloating )
	{
		if( !RefreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetWidth() _________________________________________________

bool WgFlexHook::SetWidth( int width )
{
	m_placementGeo.w = width;
	if( m_bFloating )
	{
		if( !RefreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::SetHeight() ________________________________________________

bool WgFlexHook::SetHeight( int height )
{
	m_placementGeo.h = height;
	if( m_bFloating )
	{
		if( !RefreshRealGeo() )
			return false;
	}
	return true;
}

//____ WgFlexHook::Move() _____________________________________________________

bool WgFlexHook::Move( const WgCord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->position(m_pParent->Size());
	m_placementGeo += ofs;

	if( !RefreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;
}

//____ WgFlexHook::MoveX() ____________________________________________________

bool WgFlexHook::MoveX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->position(m_pParent->Size());
	m_placementGeo.x += x;

	if( !RefreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;}

//____ WgFlexHook::MoveY() ____________________________________________________

bool WgFlexHook::MoveY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->position(m_pParent->Size());
	m_placementGeo.y += y;

	if( !RefreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;
}

//____ WgFlexHook::ScreenPos() ________________________________________________

WgCord WgFlexHook::ScreenPos() const
{
	return m_realGeo.pos() + m_pParent->ScreenPos();
}

//____ WgFlexHook::ScreenGeo() ________________________________________________

WgRect WgFlexHook::ScreenGeo() const
{
	return m_realGeo + m_pParent->ScreenPos();
}

//____ WgFlexHook::_prevHook() ________________________________________________

WgGizmoHook * WgFlexHook::_prevHook() const
{
	return Prev();
}

//____ WgFlexHook::_nextHook() ________________________________________________

WgGizmoHook * WgFlexHook::_nextHook() const
{
	return Next();
}

//____ WgFlexHook::_parent() __________________________________________________

WgGizmoContainer* WgFlexHook::_parent() const
{
	return m_pParent;
}

//____ WgFlexHook::GetRoot() __________________________________________________

WgWidget* WgFlexHook::GetRoot()
{
	WgGizmoHook * p = m_pParent->Hook();
	if( p )
		return p->GetRoot();
	else
		return 0;
}

//____ WgFlexHook::LimitPlacementSize() _______________________________________

bool WgFlexHook::LimitPlacementSize()
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

//____ WgFlexHook::RefreshRealGeo() ___________________________________________

bool WgFlexHook::RefreshRealGeo()
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
				sz = m_placementGeo.size();
				break;
			case WG_SIZE_MIN:
				sz = m_pGizmo->MinSize();
				break;
			case WG_SIZE_BEST:
				sz = m_pGizmo->BestSize();
				if( sz.w == 0 && sz.h == 0 )
					sz = m_pGizmo->MinSize();
				break;
			case WG_SIZE_MAX:
				sz = m_pGizmo->MaxSize();
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

		Detta ser skumt ut.... både Anchor() OCH m_hotspot? Ska det inte vara antingen eller?
		Ersätt även switch-sats med anrop till WgUtil::LocationToOfs().

		WgCord pos = m_pParent->Anchor(m_anchor)->position( parentSize );

		switch( m_hotspot )
		{
			case WG_NORTHWEST:
				break;

			case WG_NORTH:
				pos.x -= sz.w/2;
				break;

			case WG_NORTHEAST:
				pos.x -= sz.w;
				break;

			case WG_EAST:
				pos.x -= sz.w;
				pos.y -= sz.h/2;
				break;

			case WG_SOUTHEAST:
				pos.x -= sz.w;
				pos.y -= sz.h;
				break;

			case WG_SOUTH:
				pos.x -= sz.w/2;
				pos.y -= sz.h;
				break;

			case WG_SOUTHWEST:
				pos.y -= sz.h;
				break;

			case WG_WEST:
				pos.y -= sz.h/2;
				break;

			case WG_CENTER:
				pos.x -= sz.w/2;
				pos.y -= sz.h/2;
				break;
		}

		pos += m_placementGeo.pos();

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
		WgCord topLeft = m_pParent->Anchor(m_anchorTopLeft)->position( parentSize );
		WgCord bottomRight = m_pParent->Anchor(m_anchorBottomRight)->position( parentSize );

		newGeo = WgRect(topLeft,bottomRight);
		newGeo.shrink( m_borders );
	}

	// Return now if there was no change

	if( newGeo == m_realGeo )
		return true;

	// Request render and update positions.

	RequestRender();
	m_realGeo = newGeo;
	DoSetNewSize(newGeo);
	RequestRender();

	//

	return true;

}

//____ WgFlexHook::RequestRender() ____________________________________________

void WgFlexHook::RequestRender()
{
	m_pParent->_onRequestRender( m_realGeo, this );
}

//____ WgFlexHook::RequestRender() ____________________________________________

void WgFlexHook::RequestRender( const WgRect& rect )
{
	m_pParent->_onRequestRender( rect + m_realGeo.pos(), this );
}

//____ WgFlexHook::RequestResize() ____________________________________________

void WgFlexHook::RequestResize()
{
	if( m_sizePolicy != WG_SIZE_SPECIFIED )
		RefreshRealGeo();
}


//____ WgFlexHook::_castDirtRecursively() ___________________________________

void WgFlexHook::_castDirtRecursively( const WgRect& parentGeo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain * pDirtOut )
{
	WgRectChain	siblingDirt;

	// Recurse through the siblings ontop of us if there are any, filling dirt

	if( NextHook() )
	{
		NextHook()->_castDirtRecursively( parentGeo, clip, pDirtIn, &siblingDirt );
	}
	else
		siblingDirt.PushExistingRect( pDirtIn );

	// Get our screen geometry and clippedArea.

	WgRect screenGeo = m_realGeo + parentGeo.pos();
	WgRect clippedArea( screenGeo, clip );

	// If we are outside visible bounds, we just transfer the dirt and return.

	if( clippedArea.w == 0 || clippedArea.h == 0 )
	{
		siblingDirt.Transfer( pDirtOut) ;
		return;
	}

	// Loop through dirty rects

	WgRectLink * pRect = siblingDirt.Pop();;
	while( pRect )
	{
		WgRect dirtyArea( clippedArea, *pRect );
		if( dirtyArea.w != 0 && dirtyArea.h != 0 )
		{

			if( m_pGizmo->IsContainer() )
			{
				// This is a container, call CastDirt recursively,

				_doCastDirtyRect( screenGeo, clippedArea, pRect, pDirtOut );
			}
			else
			{
				// Add dirtyArea to our list of what to render

				m_dirt.Add( dirtyArea );

				// Mask ourselves from the rectangle and move remains to dirtOut

				WgRectChain temp;
				temp.PushExistingRect( pRect );
				_doMaskRects( temp, screenGeo, clippedArea );
				temp.Transfer( pDirtOut);
			}
		}
		else
		{
			pDirtOut->PushExistingRect( pRect );
		}

		pRect = siblingDirt.Pop();
	}

}

//____ WgFlexHook::_renderDirtyRects() _____________________________________________________

void WgFlexHook::_renderDirtyRects( WgGfxDevice * pDevice, const WgCord& parentPos, Uint8 _layer )
{
	WgRect geo =  m_realGeo + parentPos;

	if( m_pGizmo->IsContainer() )
	{
		_doRenderDirtyRects( pDevice, geo, geo, _layer );
	}
	else
	{
		WgRectLink * pDirt = m_dirt.pRectList;
		while( pDirt )
		{
			DoRender( pDevice, geo, geo, *pDirt, _layer );
			pDirt = pDirt->pNext;
		}
	}
}



//____ Constructor ____________________________________________________________

WgGizmoFlexGeo::WgGizmoFlexGeo() : m_bConfineChildren(false)
{
}

//____ Destructor _____________________________________________________________

WgGizmoFlexGeo::~WgGizmoFlexGeo()
{
	DeleteAllGizmos();
	DeleteAllAnchors();
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
			pHook->RefreshRealGeo();
			pHook = pHook->NextHook();
		}
	}
}



//____ AddGizmo() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetFloating();
	return p;
}

//____ AddGizmo() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}

//____ AddGizmo() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot, int anchor )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );

	m_hooks.PushBack(p);

	p->SetFloating( geometry, hotspot, anchor );
	return p;
}

//____ AddGizmo() _____________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	m_hooks.PushBack(p);
	p->SetFloating( geometry, hotspot );
	return p;
}

//____ InsertGizmo() __________________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgCord& pos, WgLocation hotspot, int anchor )
{
	if( !pGizmo )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	m_hooks.PushBack(p);
	p->SetFloating( WgRect(pos, pGizmo->BestSize()), hotspot, anchor );
	return p;
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgCord& pos, WgLocation hotspot )
{
	if( !pGizmo )
		return 0;

	WgSize bestSize = pGizmo->BestSize();

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,bestSize) );
	p->_attachGizmo( pGizmo );
	m_hooks.PushBack(p);
	p->SetFloating( WgRect(pos, bestSize), hotspot );
	return p;
}


//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating();
	return p;
}

//____ InsertGizmo() __________________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}

//____ InsertGizmo() __________________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot, int anchor )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( geometry, hotspot, anchor );
	return p;
}

//____ InsertGizmo() __________________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( geometry, hotspot );
	return p;
}


WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCord& pos, WgLocation hotspot, int anchor )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( pos, hotspot, anchor );
	return p;
}


WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgCord& pos, WgLocation hotspot )
{
	if( !pGizmo || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pGizmo->BestSize()) );
	p->_attachGizmo( pGizmo );
	p->MoveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( pos, hotspot );
	return p;
}


//____ DeleteGizmo() _________________________________________________

bool WgGizmoFlexGeo::DeleteGizmo( WgGizmo * pGizmo )
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

//____ ReleaseGizmo() _________________________________________________________

bool WgGizmoFlexGeo::ReleaseGizmo( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return false;

	// Force rendering of the area the gizmo was covering

	WgFlexHook * pHook = (WgFlexHook *) pGizmo->Hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Delete the gizmo and return

	pHook->_releaseGizmo();
	delete pHook;
	return true;
}

//____ DeleteAllGizmos() ______________________________________________________

void WgGizmoFlexGeo::DeleteAllGizmos()
{
	WgRectChain	dirt;

	// Collect dirty areas and delete hooks, taking any connected gizmos with them.

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		dirt.Add( pHook->m_realGeo );
		WgFlexHook * pDelete = pHook;
		pHook = pHook->NextHook();
		delete pDelete;
	}

	// RequestRender on all dirty rectangles

	WgRectLink * pDirt = dirt.pRectList;
	while( pDirt )
	{
		RequestRender( * pDirt );
		pDirt = pDirt->pNext;
	}
}

//____ ReleaseAllGizmos) ______________________________________________________

void WgGizmoFlexGeo::ReleaseAllGizmos()
{
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		pHook->_releaseGizmo();
		pHook = pHook->NextHook();
	}

	DeleteAllGizmos();		// Will only delete the hooks and request render on dirty areas since
							// we already have disconnected the children.
}

//____ AddAnchor() ____________________________________________________________

int WgGizmoFlexGeo::AddAnchor( float relativeX, float relativeY, const WgCord& pixelOfs )
{
	m_anchors.push_back( WgFlexAnchor( relativeX, relativeY, pixelOfs ) );
	return m_anchors.size()+9-1;
}

//____ ReplaceAnchor() ________________________________________________________

bool WgGizmoFlexGeo::ReplaceAnchor( int index, float relativeX, float relativeY, const WgCord& pixelOfs )
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
			pHook->RefreshRealGeo();

		pHook = pHook->NextHook();
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

		pHook = pHook->NextHook();
	}

	// Update geometry for all affected hooks.

	for( unsigned int i = 0 ; i < vNeedsUpdate.size() ; i++ )
		vNeedsUpdate[i]->RefreshRealGeo();

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

		pHook = pHook->NextHook();
	}

	// Update geometry for all affected hooks.

	for( unsigned int i = 0 ; i < vNeedsUpdate.size() ; i++ )
		vNeedsUpdate[i]->RefreshRealGeo();
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

//____ HeightForWidth() _______________________________________________________

int WgGizmoFlexGeo::HeightForWidth( int width ) const
{
	return 0;		// No recommendation.
}

//____ WidthForHeight() _______________________________________________________

int WgGizmoFlexGeo::WidthForHeight( int height ) const
{
	return 0;		// No recommendation.
}

//____ MinSize() ______________________________________________________________

WgSize WgGizmoFlexGeo::MinSize() const
{
	return WgSize(1,1);
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmoFlexGeo::BestSize() const
{
	return WgSize(0,0);		// No recommendation.
}

//____ MaxSize() ______________________________________________________________

WgSize WgGizmoFlexGeo::MaxSize() const
{
	return WgSize(65535,65535);
}

//____ FindGizmo() ____________________________________________________________

WgGizmo * WgGizmoFlexGeo::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
	WgFlexHook * pHook = m_hooks.Last();
	WgGizmo * pResult = 0;

	while( pHook && !pResult )
	{
		if( pHook->m_realGeo.contains( ofs ) )
		{
			if( pHook->Gizmo()->IsContainer() )
			{
				pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - pHook->m_realGeo.pos(), mode );
			}
			else
			{
				switch( mode )
				{
					case WG_SEARCH_ACTION_TARGET:
					case WG_SEARCH_MARKPOLICY:
						if( pHook->Gizmo()->MarkTest( ofs - pHook->m_realGeo.pos() ) )
							pResult = pHook->Gizmo();
						break;
					case WG_SEARCH_GEOMETRY:
						pResult = pHook->Gizmo();
						break;
				}
			}
		}
		pHook = pHook->PrevHook();
	}

	return pResult;
}


//____ _onCollectRects() _______________________________________________________

void WgGizmoFlexGeo::_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		pHook->_doCollectRects( rects, pHook->m_realGeo + geo.pos(), clip );
		pHook = pHook->NextHook();
	}
}

//____ _onMaskRects() __________________________________________________________

void WgGizmoFlexGeo::_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		pHook->_doMaskRects( rects, pHook->m_realGeo + geo.pos(), clip );
		pHook = pHook->NextHook();
	}
}

//____ _onRequestRender() ______________________________________________________

void WgGizmoFlexGeo::_onRequestRender( const WgRect& rect, const WgFlexHook * pHook )
{
	if( pHook->IsHidden() )
		return;

	// Clip our geometry and put it in a dirtyrect-list

	WgRectChain rects;
	rects.Add( WgRect( rect, WgRect(0,0,Size())) );

	// Remove portions of dirty rect that are covered by opaque upper siblings,
	// possibly filling list with many small dirty rects instead.

	WgFlexHook * pCover = pHook->NextHook();
	while( pCover )
	{
		if( pCover->m_realGeo.intersectsWith( pHook->m_realGeo ) )
			pCover->_doMaskRects( rects, pCover->m_realGeo, WgRect(0,0,65536,65536 ) );

		pCover = pCover->NextHook();
	}

	// Make request render calls

	WgRectLink * pRect = rects.pRectList;
	while( pRect )
	{
		RequestRender( * pRect );
		pRect = pRect->pNext;
	}
}

//____ _onRender() ____________________________________________________________

void WgGizmoFlexGeo::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Do nothing
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoFlexGeo::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement
}

//____ _renderDirtyRects() ____________________________________________________

void WgGizmoFlexGeo::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
	WgFlexHook *pHook	= m_hooks.First();
	WgCord		pos		= _canvas.pos();

	while( pHook )
	{
		pHook->_renderDirtyRects( pDevice, pos, _layer );
		pHook = pHook->NextHook();
	}
}

//____ _clearDirtyRects() _____________________________________________________

void WgGizmoFlexGeo::_clearDirtyRects()
{
	WgFlexHook *pHook	= m_hooks.First();

	while( pHook )
	{
		pHook->m_dirt.Clear();
		if( pHook->Gizmo()->IsContainer() )
			pHook->_doClearDirtyRects();

		pHook = pHook->NextHook();
	}
}


//____ _castDirtyRect() _______________________________________________________

void WgGizmoFlexGeo::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
	WgFlexHook * pHook = m_hooks.First();

	if( pHook )
	{
		pHook->_castDirtRecursively( geo, WgRect(clip,geo), pDirtIn, pDirtOut );
	}
}


//____ _onNewSize() ____________________________________________________________

void WgGizmoFlexGeo::_onNewSize( const WgSize& size )
{
	WgFlexHook * pHook = m_hooks.Last();

	while( pHook )
	{
		pHook->RefreshRealGeo();
		pHook = pHook->PrevHook();
	}
}

//____ _onAction() _____________________________________________________________

void WgGizmoFlexGeo::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ _onAlphaTest() __________________________________________________________

bool WgGizmoFlexGeo::_onAlphaTest( const WgCord& ofs )
{
	return false;
}
