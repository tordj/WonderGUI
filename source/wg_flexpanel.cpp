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

#include <wg_flexpanel.h>
#include <wg_patches.h>
#include <wg_util.h>

static const char	c_widgetType[] = {"FlexPanel"};
static const char	c_hookType[] = {"FlexHook"};

WgFlexAnchor		WgFlexPanel::g_baseAnchors[9] = { WgFlexAnchor(0.f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 0.f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 0.5f, WgCoord(0,0)),
														 WgFlexAnchor(1.f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.f, 1.f, WgCoord(0,0)),
														 WgFlexAnchor(0.f, 0.5f, WgCoord(0,0)),
														 WgFlexAnchor(0.5f, 0.5f, WgCoord(0,0)) };

//____ WgFlexHook::Constructor ________________________________________________

WgFlexHook::WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo ) : m_pParent(pParent), m_bHidden(false),
	m_bFloating(false), m_widthPolicy(WG_BOUND), m_heightPolicy(WG_BOUND),
	m_anchor(WG_NORTHWEST), m_hotspot(WG_NORTHWEST),
	m_placementGeo(placementGeo), m_anchorTopLeft(WG_NORTHWEST),
	m_anchorBottomRight(WG_SOUTHEAST)
{
}

//____ WgFlexHook::Type() _____________________________________________________

const char * WgFlexHook::Type( void ) const
{
	return ClassType();
}

//____ WgFlexHook::ClassType() ________________________________________________

const char * WgFlexHook::ClassType()
{
	return c_hookType;
}


//____ WgFlexHook::SetAnchored() ______________________________________________

bool WgFlexHook::SetAnchored()
{
	// Return to old anchors.

	m_bFloating = false;
	_refreshRealGeo();
	return true;
}

bool  WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight, WgBorders padding )
{
	int nbAnchors = m_pParent->NbAnchors();
	if( anchorTopLeft >= nbAnchors || anchorBottomRight >= nbAnchors )
		return false;

	m_bFloating			= false;
	m_anchorTopLeft		= anchorTopLeft;
	m_anchorBottomRight = anchorBottomRight;
	m_padding			= padding;

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

	m_widthPolicy	= WG_DEFAULT;
	m_heightPolicy	= WG_DEFAULT;
	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo.SetPos(pos);

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

	m_widthPolicy	= WG_BOUND;
	m_heightPolicy	= WG_BOUND;
	m_bFloating		= true;
	m_anchor		= anchor;
	m_hotspot		= hotspot;
	m_placementGeo	= geometry;

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

	if( !pNext->m_bHidden && cover.w > 0 && cover.h > 0 )
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

	if( !m_bHidden && cover.w > 0 && cover.h > 0 )
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

		if( !m_bHidden )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( !p->m_bHidden && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, this );
				p = p->_next();
			}
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = pLast->_prev();
		_moveAfter( pSibling );

		// Request render on our siblings for the area we previously have covered.

		if( !m_bHidden )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( !p->m_bHidden && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, p );
				p = p->_prev();
			}
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

		if( !m_bHidden )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( !p->m_bHidden && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, this );

				p = p->_next();
			}
		}
	}
	else							// Move us backward
	{
		WgFlexHook * p = _prev();
		_moveBefore( pSibling );

		// Request render on our siblings for the area we previously have covered.

		if( !m_bHidden )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( !p->m_bHidden && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, p );

				p = p->_prev();
			}
		}
	}

	return true;
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

bool WgFlexHook::SetSizePolicy( WgSizePolicy width, WgSizePolicy height )
{
	if( width != m_widthPolicy || height != m_heightPolicy )
	{
		m_widthPolicy = width;
		m_heightPolicy = height;
		if( m_bFloating )
			_refreshRealGeo();
	}
	return true;
}

//____ WgFlexHook::SetGeo() ___________________________________________________

bool WgFlexHook::SetGeo( const WgRect& geometry )
{
	m_placementGeo = geometry;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfs() ___________________________________________________

bool WgFlexHook::SetOfs( const WgCoord& ofs )
{
	m_placementGeo.x = ofs.x;
	m_placementGeo.y = ofs.y;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfsX() __________________________________________________

bool WgFlexHook::SetOfsX( int x )
{
	m_placementGeo.x = x;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfsY() __________________________________________________

bool WgFlexHook::SetOfsY( int y )
{
	m_placementGeo.y = y;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetSize() __________________________________________________

bool WgFlexHook::SetSize( const WgSize& size )
{
	m_placementGeo.w = size.w;
	m_placementGeo.h = size.h;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetWidth() _________________________________________________

bool WgFlexHook::SetWidth( int width )
{
	m_placementGeo.w = width;
	if( m_bFloating )
		_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetHeight() ________________________________________________

bool WgFlexHook::SetHeight( int height )
{
	m_placementGeo.h = height;
	if( m_bFloating )
		_refreshRealGeo();
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

	_refreshRealGeo();
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

	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::MoveY() ____________________________________________________

bool WgFlexHook::MoveY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = m_realGeo;
	m_placementGeo -= m_pParent->Anchor(m_anchor)->Position(m_pParent->Size());
	m_placementGeo.y += y;

	_refreshRealGeo();
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

WgWidgetHolder * WgFlexHook::_parent() const
{
	return m_pParent;
}

//____ WgFlexHook::_refreshRealGeo() ___________________________________________

void WgFlexHook::_refreshRealGeo()
{
	WgRect	newGeo;
	WgSize	parentSize = m_pParent->Size();

	if( m_bFloating )
	{
		// Calculate size

		WgSize sz = _sizeFromPolicy( m_placementGeo.Size(), m_widthPolicy, m_heightPolicy );

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
	}

	newGeo.Shrink( m_padding );

	// Request render and update positions.

	if( newGeo != m_realGeo )
	{
		_requestRender();
		m_realGeo = newGeo;
		m_pWidget->_onNewSize(newGeo);
		_requestRender();
	}
}

//____ WgFlexHook::_sizeNeededForGeo() ________________________________________

WgSize WgFlexHook::_sizeNeededForGeo()
{
	WgSize sz;

	if( m_bFloating )
	{
		const WgFlexAnchor * pa = m_pParent->Anchor(m_anchor);

		WgCoord hotspot = WgUtil::OrientationToOfs(m_hotspot,m_placementGeo.Size());
		WgCoord offset = pa->Offset() + m_placementGeo.Pos() - hotspot;

		int leftOfAnchor = 0 - offset.x;
		int rightOfAnchor = offset.x + m_placementGeo.w;
		int aboveAnchor = 0 - offset.y;
		int belowAnchor = offset.y + m_placementGeo.h;

		if( leftOfAnchor > 0 )
			sz.w = (int) (leftOfAnchor / pa->RelativeX());

		if( rightOfAnchor > 0 )
		{
			int w = (int) (rightOfAnchor / (1.f - pa->RelativeX()) );
			if( sz.w < w )
				sz.w = w;
		}

		if( aboveAnchor > 0 )
			sz.h = (int) (aboveAnchor / pa->RelativeY());

		if( belowAnchor > 0 )
		{
			int h = (int) (belowAnchor / (1.f - pa->RelativeY()) );
			if( sz.h < h )
				sz.h = h;
		}
	}
	else
	{
		sz = m_pWidget->PreferredSize() + m_padding;

		const WgFlexAnchor * pa1 = m_pParent->Anchor(m_anchorTopLeft);
		const WgFlexAnchor * pa2 = m_pParent->Anchor(m_anchorBottomRight);

		sz += WgSize(pa1->OffsetX(),pa1->OffsetY());
		sz -= WgSize(pa2->OffsetX(),pa2->OffsetY());

		sz.w = (int) (sz.w / (float) (pa2->RelativeX() - pa1->RelativeX()));
		sz.h = (int) (sz.w / (float) (pa2->RelativeY() - pa1->RelativeY()));
	}

	return sz;
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
	if( m_widthPolicy != WG_BOUND || m_heightPolicy != WG_BOUND )
		_refreshRealGeo();
}


//____ Constructor ____________________________________________________________

WgFlexPanel::WgFlexPanel() : m_bConfineChildren(false)
{
	m_bSiblingsOverlap = true;
}

//____ Destructor _____________________________________________________________

WgFlexPanel::~WgFlexPanel()
{
// This is done automatically now, calls like these in destructor are forbidden.

//	DeleteAllWidgets();
//	DeleteAllAnchors();
}

//____ Type() _________________________________________________________________

const char * WgFlexPanel::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgFlexPanel::GetClass()
{
	return c_widgetType;
}

//____ SetConfineChildren() ___________________________________________________

void WgFlexPanel::SetConfineChildren( bool bConfineChildren )
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

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );

	m_hooks.PushBack(p);

	p->SetFloating();
	return p;
}

//____ AddChild() _____________________________________________________________

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );

	m_hooks.PushBack(p);

	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget, const WgCoord& pos, WgOrientation origo )
{
	return AddChild( pWidget, pos, origo, origo );
}

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget, const WgCoord& pos, int anchor, WgOrientation hotspot )
{
	if( !pWidget )
		return 0;

	WgSize bestSize = pWidget->PreferredSize();

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,bestSize) );
	p->_attachWidget( pWidget );
	m_hooks.PushBack(p);
	p->SetFloating( pos, anchor, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget, const WgRect& geometry, WgOrientation origo )
{
	return AddChild( pWidget, geometry, origo, origo );
}

WgFlexHook * WgFlexPanel::AddChild( WgWidget * pWidget, const WgRect& geometry, int anchor, WgOrientation hotspot )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );

	m_hooks.PushBack(p);

	p->SetFloating( geometry, anchor, hotspot );
	return p;
}


//____ InsertChild() _________________________________________________

WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating();
	return p;
}


WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetAnchored( anchorTopLeft, anchorBottomRight, borders );
	return p;
}


WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgRect& geometry, WgOrientation origo )
{
	return InsertChild( pWidget, pSibling, geometry, origo, origo );
}

WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgRect& geometry, int anchor, WgOrientation hotspot )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( geometry, anchor, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgCoord& pos, WgOrientation origo )
{
	return InsertChild( pWidget, pSibling, pos, origo, origo );
}

WgFlexHook * WgFlexPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling, const WgCoord& pos, int anchor, WgOrientation hotspot )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()) );
	p->_attachWidget( pWidget );
	p->_moveBefore( (WgFlexHook*)pSibling->Hook() );
	p->SetFloating( pos, anchor, hotspot );
	return p;
}




//____ DeleteChild() _________________________________________________

bool WgFlexPanel::DeleteChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Force rendering of the area the widget was covering

	WgFlexHook * pHook = (WgFlexHook *) pWidget->Hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Delete the widget and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgWidget * WgFlexPanel::ReleaseChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return 0;

	// Force rendering of the area the widget was covering

	WgFlexHook * pHook = (WgFlexHook *) pWidget->Hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Delete the widget and return

	pHook->_releaseWidget();
	delete pHook;
	return pWidget;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgFlexPanel::DeleteAllChildren()
{
	WgPatches	dirt;

	// Collect dirty areas and delete hooks, taking any connected widgets with them.

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		if( !pHook->m_bHidden )
			dirt.Add( pHook->m_realGeo );
		WgFlexHook * pDelete = pHook;
		pHook = pHook->Next();
		delete pDelete;
	}

	// RequestRender on all dirty patches

	for( const WgRect * pRect = dirt.Begin() ; pRect != dirt.End() ; pRect++ )
		_requestRender( * pRect );

	return true;
}

//____ ReleaseAllChildren() ______________________________________________________

bool WgFlexPanel::ReleaseAllChildren()
{
	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		pHook->_releaseWidget();
		pHook = pHook->Next();
	}

	DeleteAllChildren();	// Will only delete the hooks and request render on dirty areas since
							// we already have disconnected the children.
	return true;
}

//____ AddAnchor() ____________________________________________________________

int WgFlexPanel::AddAnchor( float relativeX, float relativeY, const WgCoord& pixelOfs )
{
	m_anchors.push_back( WgFlexAnchor( relativeX, relativeY, pixelOfs ) );
	return m_anchors.size()+9-1;
}

//____ ReplaceAnchor() ________________________________________________________

bool WgFlexPanel::ReplaceAnchor( int index, float relativeX, float relativeY, const WgCoord& pixelOfs )
{
	if( index < 9 || index >= NbAnchors() )
		return false;

	m_anchors[index-9] = WgFlexAnchor( relativeX, relativeY, pixelOfs );

	// Update geometry for all widgets using this anchor.

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

bool WgFlexPanel::DeleteAnchor( int index )
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
		// Check if this widget will have its geometry altered.

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

void WgFlexPanel::DeleteAllAnchors()
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
		// Check if this widget will have its geometry altered.

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

const WgFlexAnchor * WgFlexPanel::Anchor( int index )
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

//____ PreferredSize() _____________________________________________________________

WgSize WgFlexPanel::PreferredSize() const
{
	WgSize minSize;

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		WgSize sz = pHook->_sizeNeededForGeo();
		minSize = WgSize::Max(minSize,sz);
		pHook = pHook->Next();
	}
	return minSize;
}

//____ _onRequestRender() ______________________________________________________

void WgFlexPanel::_onRequestRender( const WgRect& rect, const WgFlexHook * pHook )
{
	if( pHook->m_bHidden )
		return;

	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,Size())) );

	// Remove portions of patches that are covered by opaque upper siblings

	WgFlexHook * pCover = pHook->Next();
	while( pCover )
	{
		if( !pCover->m_bHidden && pCover->m_realGeo.IntersectsWith( rect ) )
			pCover->Widget()->_onMaskPatches( patches, pCover->m_realGeo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->Next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.Begin() ; pRect < patches.End() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onCloneContent() _______________________________________________________

void WgFlexPanel::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement
}

//____ _onNewSize() ____________________________________________________________

void WgFlexPanel::_onNewSize( const WgSize& size )
{
	WgFlexHook * pHook = m_hooks.Last();

	while( pHook )
	{
		pHook->_refreshRealGeo();
		pHook = pHook->Prev();
	}
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgFlexPanel::_firstHookWithGeo( WgRect& writeGeo ) const
{
	WgFlexHook * p = m_hooks.First();
	if( p )
		writeGeo = p->m_realGeo;

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgFlexPanel::_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const
{
	WgFlexHook * p = ((WgFlexHook*)pHook)->_next();
	if( p )
		writeGeo = p->m_realGeo;

	return p;
}

//____ _lastHookWithGeo() _____________________________________________________

WgHook * WgFlexPanel::_lastHookWithGeo( WgRect& writeGeo ) const
{
	WgFlexHook * p = m_hooks.Last();
	if( p )
		writeGeo = p->m_realGeo;

	return p;
}

//____ _prevHookWithGeo() ______________________________________________________

WgHook * WgFlexPanel::_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const
{
	WgFlexHook * p = ((WgFlexHook*)pHook)->_prev();
	if( p )
		writeGeo = p->m_realGeo;

	return p;
}

