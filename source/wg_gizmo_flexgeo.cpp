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

static const char	c_gizmoType[] = {"TordJ/FlexGeo"};

WgFlexAnchor		WgGizmoFlexGeo::g_baseAnchors[9] = { WgFlexAnchor(0.f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 0.f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 0.5f, WgCord(0,0)),
														 WgFlexAnchor(1.f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.f, 1.f, WgCord(0,0)),
														 WgFlexAnchor(0.f, 0.5f, WgCord(0,0)),
														 WgFlexAnchor(0.5f, 0.5f, WgCord(0,0)) };


void WgFlexHook::SetAnchored()
{
	// Return to old anchors.

	m_bFloating = false;
	RefreshRealGeo();
}

bool WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight )
{
	if( anchorTopLeft >= NbAnchors() || anchorBottomRight >= NbAnchors() )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;

	RefreshRealGeo();
	return true;
}

bool  WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( anchorTopLeft >= NbAnchors() || anchorBottomRight >= NbAnchors() )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;
	m_borders			= borders;

	RefreshRealGeo();
	return true;
}



void WgFlexHook::SetFloating()
{
	// Return to old floating position and size.

	m_bFloating = true;
	RefreshRealGeo();
	return true;
}

bool WgFlexHook::SetFloating( const WgRect& geometry )
{
	return SetFloating( geometry, WG_NORTHWEST, WG_NORTHWEST );
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot )
{
	return SetFloating( geometry, hotspot, hotspot );
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot, int anchor )
{
	if( anchor >= NbAnchors() )
		return false;

	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo	= geometry;

	LimitPlacementSize();
	RefreshRealGeo();
	return true;
}

void WgFlexHook::Top()
{
}

void WgFlexHook::Bottom()
{
}

bool WgFlexHook::Up()
{
}

bool WgFlexHook::Down()
{
}

bool WgFlexHook::MoveOver( WgFlexHook * pSibling )
{
}

bool WgFlexHook::MoveUnder( WgFlexHook * pSibling )
{
}

void WgFlexHook::Hide()
{
	if( !m_bHidden )
	{
		Redraw();
		m_bHidden = true;
	}
}

void WgFlexHook::Show()
{
	if( m_bHidden )
	{
		m_bHidden = false;
		Redraw();
	}
}

bool WgFlexHook::SetOrigo( int anchor )
{
	if( anchor >= NbAnchors() )
		return false;

	if( anchor != m_anchor )
	{
		m_anchor = anchor;
		RefreshRealGeo();
	}

	return true;
}

bool WgFlexHook::SetHotspot( WgLocation hotspot )
{
	if( hotspot != m_hotspot )
	{
		m_hotspot = hotspot;
		RefreshRealGeo();
	}

	return true;
}

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

bool WgFlexHook::MoveY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->position(m_pParent->Size());
	m_placementGeo.y += y;

	if( !RefreshRealGeo() )
			return false;							// Return false if we could not get (exactly) the requested position.

	return true;}

WgCord WgFlexHook::Pos() const
{
	return m_realGeo.pos();
}

WgSize WgFlexHook::Size() const
{
	return m_realGeo.size();
}

WgRect WgFlexHook::Geo() const
{
	return m_realGeo;
}

WgCord WgFlexHook::ScreenPos() const
{
	return m_realGeo.pos() + m_pParent->ScreenPos();
}

WgRect WgFlexHook::ScreenGeo() const
{
	return m_realGeo + m_pParent->ScreenPos();
}

WgGizmoHook * WgFlexHook::PrevHook() const
{
	return Prev();
}

WgGizmoHook * WgFlexHook::NextHook() const
{
	return Next();
}

WgGizmoContainer* WgFlexHook::Parent() const
{
	return m_pParent;
}

WgWidget* WgFlexHook::GetRoot()
{
	WgGizmoHook * p = m_pParent->Hook();
	if( p )
		return p->GetRoot();
	else
		return 0;
}

WgFlexHook::WgFlexHook( WgGizmo * pGizmo, WgGizmoFlexGeo * pParent ) : 
	m_bHidden(false), m_bFloating(false), m_sizePolicy(WG_SIZE_SPECIFIED), m_minSize(0,0),
	m_maxSize(65536,65536), m_anchor(WG_NORTHWEST), m_hotspot(WG_NORTHWEST),
	m_placementGeo(0,0,pGizmo->BestSize()), m_anchorTopLeft(WG_NORTHWEST), 
	m_anchorBottomRight(WG_SOUTHEAST), m_borders(0), WgGizmoHook( pGizmo, pParent )

{
	m_pParent = pParent;
}

bool WgFlexHook::LimitPlacementSize()
{
	int ret = true;

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

bool WgFlexHook::RefreshRealGeo()
{
}

void WgFlexHook::RequestRender()
{
	m_pParent->OnRequestRender( m_realGeo );
}

void WgFlexHook::RequestRender( const WgRect& rect )
{
	m_pParent->OnRequestRender( rect + m_realGeo.pos() );
}

void WgFlexHook::RequestResize()
{
	if( m_sizePolicy != WG_SIZE_SPECIFIED )
		RefreshRealGeo();
}

//____ () _________________________________________________

WgGizmoFlexGeo::WgGizmoFlexGeo() : m_bClipChildren(false), m_bConfineChildren(false), m_clipMode( NO_CLIP )
{
}

//____ () _________________________________________________

WgGizmoFlexGeo::~WgGizmoFlexGeo()
{
	DeleteAllGizmos();
	DeleteAllAnchors();
}

//____ () _________________________________________________

const char * WgGizmoFlexGeo::Type( void ) const
{
	return GetMyType();
}

//____ () _________________________________________________

const char * WgGizmoFlexGeo::GetMyType()
{
	return c_gizmoType;
}

//____ () _________________________________________________

void WgGizmoFlexGeo::SetClipChildren( bool bClipChildren )
{
	if( bClipChildren != m_bClipChildren )
	{
		m_bClipChildren = bClipChildren;

		// Request render on portion of children and grandchildren outside our geometry.

		m_clipMode = INVERTED_CLIP;
		if( !m_bConfineChildren )
		{
			WgFlexHook * p = m_hooks.First();
			while( p )
			{
				p->Gizmo()->Redraw();
				p = p->NextHook();
			}
		}

		//

		if( bClipChildren )
			m_clipMode = CLIP;
		else
			m_clipMode = NO_CLIP;
	}

}

//____ () _________________________________________________

void WgGizmoFlexGeo::SetConfineChildren( bool bConfineChildren )
{
	if( bConfineChildren != m_bConfineChildren )
	{
		m_bConfineChildren = bConfineChildren;

		BoundingBoxChanged();

		// Update geo on children and render if changed.
		// Clip rendering if m_bClipChildren!

		Continue here!!!
	}
}



//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::DeleteGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::ReleaseGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::DeleteAllGizmos()
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::ReleaseAllGizmos()
{
}

//____ AddAnchor() _________________________________________________

int WgGizmoFlexGeo::AddAnchor( float relativeX, float relativeY, const WgCord& pixelOfs )
{
	m_anchors.push_back( WgFlexAnchor( relativeX, relativeY, pixelOfs ) );
	return m_anchors.size()+9-1;
}

//____ ReplaceAnchor() _________________________________________________

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

//____ DeleteAnchor() _________________________________________________

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
			(!pHook->m_bFloating && (pHook->m_anchorTopLeft == index || pHook->m_anchorBottomRight == index) )
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

	for( int i = 0 ; i < vNeedsUpdate.size() ; i++ )
		vNeedsUpdate[i]->RefreshRealGeo();
}

//____ Anchor() _________________________________________________

const WgFlexAnchor * WgGizmoFlexGeo::Anchor( int index )
{
	if( index < 9 )
		return &g_baseAnchors[index];
	else
	{
		index -= 9;
		if( index < m_anchors.size() )
			return &m_anchors[index];
	}
	return 0;
}

//____ () _________________________________________________

WgRect WgGizmoFlexGeo::BoundingBoxForSize( WgSize size ) const
{
}

//____ () _________________________________________________

int WgGizmoFlexGeo::HeightForWidth( int width ) const
{
}

//____ () _________________________________________________

int WgGizmoFlexGeo::WidthForHeight( int height ) const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::MinSize() const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::BestSize() const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::MaxSize() const
{
}

//____ () _________________________________________________

WgGizmo * WgGizmoFlexGeo::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnRequestRender( const WgRect& rect, WgFlexHook * pHook )
{
	// Check if this branch should be rendered at all

	if( pHook->IsHidden() )
		return;

	//TODO: Clip against any siblings that might be covering.




	// Clip against our own geometry and render

	switch( m_clipMode )
	{
		case NO_CLIP:
		{
			RequestRender( rect );
			break;
		}
		case CLIP:
		{
			WgRect	clip( 0,0,Size() );
			if( clip.contains(rect) )
				RequestRender( rect );
			else
				RequestRender( WgRect( rect, clip ) );
			break;
		}
		case INVERTED_CLIP:
		{
			WgRect	clip( 0,0,Size() );
			if( clip.contains(rect) )
				return;

			RequestRender( rect );				//TODO: Can be clipped into one or more rectangles for optimization.
			break;
		}
	}

}


//____ () _________________________________________________

void WgGizmoFlexGeo::OnCloneContent( const WgGizmo * _pOrg )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnNewSize( const WgSize& size )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::OnAlphaTest( const WgCord& ofs )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnEnable()
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnDisable()
{
}

