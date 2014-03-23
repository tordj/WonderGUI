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

const char WgFlexPanel::CLASSNAME[] = {"FlexPanel"};
const char WgFlexHook::CLASSNAME[] = {"FlexHook"};

float	WgFlexOrigo::s_origoTab[9][2] = {	0.f, 0.f,
											0.5f, 0.f,
											1.f, 0.f,
											1.f, 0.5f,
											1.f, 1.f,
											0.5f, 1.f,
											0.f, 1.f,
											0.f, 0.5f,
											0.5f, 0.5f };

//____ WgFlexHook::Constructor ________________________________________________

WgFlexHook::WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo, WgBorders padding ) : m_pParent(pParent),
	m_bFloating(false), m_widthPolicy(WG_BOUND), m_heightPolicy(WG_BOUND),
	m_origo(WG_NORTHWEST), m_hotspot(WG_NORTHWEST),
	m_placementGeo(placementGeo)
{
    m_padding = padding;
}

//____ WgFlexHook::IsInstanceOf() __________________________________________

bool WgFlexHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanelHook::IsInstanceOf(pClassName);
}

//____ WgFlexHook::ClassName() _____________________________________________

const char * WgFlexHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgFlexHook::Cast() __________________________________________________

WgFlexHookPtr WgFlexHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgFlexHookPtr( static_cast<WgFlexHook*>(pHook.GetRealPtr()) );

	return 0;
}


//____ WgFlexHook::SetStretching() ______________________________________________

bool  WgFlexHook::SetStretching( const WgFlexOrigo& topLeftOrigo, 
								 const WgFlexOrigo& bottomRightOrigo, WgBorders padding )
{
	m_bFloating			= false;
	m_topLeftOrigo		= topLeftOrigo;
	m_bottomRightOrigo	= bottomRightOrigo;
	m_padding			= padding;

	m_topLeftOfs.Clear();
	m_bottomRightOfs.Clear();

	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetStretching() ______________________________________________

bool  WgFlexHook::SetStretching( const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
	const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding )
{
	m_bFloating			= false;
	m_topLeftOrigo		= topLeftOrigo;
	m_topLeftOfs		= topLeftOfs;
	m_bottomRightOrigo	= bottomRightOrigo;
	m_bottomRightOfs	= bottomRightOfs;
	m_padding			= padding;

	_refreshRealGeo();
	return true;
}


//____ WgFlexHook::SetFloating() ______________________________________________

bool WgFlexHook::SetFloating( const WgCoord& pos, const WgFlexOrigo& origo )
{
	return SetFloating( pos, origo, origo );
}

bool WgFlexHook::SetFloating( const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot )
{
	m_widthPolicy	= WG_DEFAULT;
	m_heightPolicy	= WG_DEFAULT;
	m_bFloating		= true;
	m_origo			= origo;
	m_hotspot		= hotspot;
	m_placementGeo.SetPos(pos);

	_refreshRealGeo();
	return true;
}

bool WgFlexHook::SetFloating( const WgRect& geometry, const WgFlexOrigo& origo )
{
	return SetFloating( geometry, origo, origo );
}

bool WgFlexHook::SetFloating( const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot )
{
	m_widthPolicy	= WG_BOUND;
	m_heightPolicy	= WG_BOUND;
	m_bFloating		= true;
	m_origo			= origo;
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

	if( pNext->m_bVisible && cover.w > 0 && cover.h > 0 )
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

	if( m_bVisible && cover.w > 0 && cover.h > 0 )
	{
		m_pParent->_onRequestRender( cover, pPrev );
	}

	return true;
}

//____ WgFlexHook::MoveOver() _________________________________________________

bool WgFlexHook::MoveOver( const WgFlexHookPtr& _pSibling )
{
	WgFlexHook * pSibling = _pSibling.GetRealPtr();

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

		if( m_bVisible )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
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

		if( m_bVisible )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, p );
				p = p->_prev();
			}
		}
	}

	return true;
}

//____ WgFlexHook::MoveUnder() ________________________________________________

bool WgFlexHook::MoveUnder( const WgFlexHookPtr& _pSibling )
{
	WgFlexHook * pSibling = _pSibling.GetRealPtr();

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

		if( m_bVisible )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
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

		if( m_bVisible )
		{
			while( p != this )
			{
				WgRect cover( m_realGeo, p->m_realGeo );

				if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
					m_pParent->_onRequestRender( cover, p );

				p = p->_prev();
			}
		}
	}

	return true;
}

//____ WgFlexHook::SetOrigo() _________________________________________________

bool WgFlexHook::SetOrigo( const WgFlexOrigo& origo )
{
	if( !m_bFloating )
		return false;

	if( origo != m_origo )
	{
		m_origo = origo;
		_refreshRealGeo();
	}

	return true;
}

//____ WgFlexHook::SetHotspot() _______________________________________________

bool WgFlexHook::SetHotspot( const WgFlexOrigo& hotspot )
{
	if( !m_bFloating )
		return false;

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
	if( !m_bFloating )
		return false;

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
	if( !m_bFloating )
		return false;

	m_placementGeo = geometry;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfs() ___________________________________________________

bool WgFlexHook::SetOfs( const WgCoord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x = ofs.x;
	m_placementGeo.y = ofs.y;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfsX() __________________________________________________

bool WgFlexHook::SetOfsX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x = x;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetOfsY() __________________________________________________

bool WgFlexHook::SetOfsY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.y = y;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetSize() __________________________________________________

bool WgFlexHook::SetSize( const WgSize& size )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.w = size.w;
	m_placementGeo.h = size.h;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetWidth() _________________________________________________

bool WgFlexHook::SetWidth( int width )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.w = width;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::SetHeight() ________________________________________________

bool WgFlexHook::SetHeight( int height )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.h = height;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::Move() _____________________________________________________

bool WgFlexHook::Move( const WgCoord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo += ofs;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::MoveX() ____________________________________________________

bool WgFlexHook::MoveX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x += x;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::MoveY() ____________________________________________________

bool WgFlexHook::MoveY( int y )
{
	if( !m_bFloating )
		return false;

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

WgContainer * WgFlexHook::_parent() const
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

		WgSize sz = _sizeFromPolicy( m_placementGeo.Size() + m_padding, m_widthPolicy, m_heightPolicy );

		// Calculate position

		WgCoord pos = m_origo.Position( parentSize );	// Origo,
		pos -= m_hotspot.Position(sz);					// hotspot
		pos += m_placementGeo.Pos();					// and Offset.

		// Limit size/pos according to parent

		if( m_pParent->IsConfiningWidgets() )
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
		WgCoord topLeft = m_topLeftOrigo.Position( parentSize ) + m_topLeftOfs;
		WgCoord bottomRight = m_bottomRightOrigo.Position( parentSize ) + m_bottomRightOfs;

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
        WgRect geo = m_placementGeo + m_padding;
        
		WgCoord hotspot = m_hotspot.Position(geo.Size());
		WgCoord offset = geo.Pos() - hotspot;

		int leftOfOrigo = 0 - offset.x;
		int rightOfOrigo = offset.x + geo.w;
		int aboveOrigo = 0 - offset.y;
		int belowOrigo = offset.y + geo.h;

		if( leftOfOrigo > 0 )
			sz.w = (int) (leftOfOrigo / m_origo.x);

		if( rightOfOrigo > 0 )
		{
			int w = (int) (rightOfOrigo / (1.f - m_origo.x) );
			if( sz.w < w )
				sz.w = w;
		}

		if( aboveOrigo > 0 )
			sz.h = (int) (aboveOrigo / m_origo.y);

		if( belowOrigo > 0 )
		{
			int h = (int) (belowOrigo / (1.f - m_origo.y) );
			if( sz.h < h )
				sz.h = h;
		}
	}
	else
	{
		sz = m_pWidget->PreferredSize() + m_padding;

		sz += WgSize( m_topLeftOfs.x, m_topLeftOfs.y );
		sz -= WgSize( m_bottomRightOfs.x, m_bottomRightOfs.y );

		sz.w = (int) (sz.w / (float) (m_bottomRightOrigo.x - m_topLeftOrigo.x));
		sz.h = (int) (sz.w / (float) (m_bottomRightOrigo.y - m_topLeftOrigo.y));
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

WgFlexPanel::WgFlexPanel() : m_bConfineWidgets(false)
{
	m_bSiblingsOverlap = true;
}

//____ Destructor _____________________________________________________________

WgFlexPanel::~WgFlexPanel()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgFlexPanel::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgFlexPanel::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgFlexPanelPtr WgFlexPanel::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgFlexPanelPtr( static_cast<WgFlexPanel*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetConfineWidgets() ___________________________________________________

void WgFlexPanel::SetConfineWidgets( bool bConfineWidgets )
{
	if( bConfineWidgets != m_bConfineWidgets )
	{
		m_bConfineWidgets = bConfineWidgets;

		WgFlexHook * pHook = m_hooks.First();
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_next();
		}
	}
}



//____ AddWidget() _____________________________________________________________

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), WgBorders(0) );
	p->_setWidget( pWidget.GetRealPtr() );

	m_hooks.PushBack(p);

	p->SetFloating( WgCoord(0,0) );
	return p;
}

//____ AddWidget() _____________________________________________________________

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgFlexOrigo& topLeftOrigo,
									 const WgFlexOrigo& bottomRightOrigo, WgBorders padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), padding );
	p->_setWidget( pWidget.GetRealPtr() );

	m_hooks.PushBack(p);

	p->SetStretching( topLeftOrigo, bottomRightOrigo, padding );
	return p;
}

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
							 const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), padding );
	p->_setWidget( pWidget.GetRealPtr() );

	m_hooks.PushBack(p);

	p->SetStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
	return p;
}

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, WgBorders padding )
{
	return AddWidget( pWidget, pos, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding )
{
	if( !pWidget )
		return 0;

	WgSize bestSize = pWidget->PreferredSize();

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,bestSize), padding );
	p->_setWidget( pWidget.GetRealPtr() );
	m_hooks.PushBack(p);
	p->SetFloating( pos, origo, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, WgBorders padding )
{
	return AddWidget( pWidget, geometry, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::AddWidget( const WgWidgetPtr& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), padding );
	p->_setWidget( pWidget.GetRealPtr() );

	m_hooks.PushBack(p);

	p->SetFloating( geometry, origo, hotspot );
	return p;
}


//____ InsertWidget() _________________________________________________

WgFlexHook * WgFlexPanel::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), WgBorders(0) );
	p->_setWidget( pWidget.GetRealPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->SetFloating( WgCoord(0,0) );
	return p;
}


WgFlexHook * WgFlexPanel::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
									   const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorders padding )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), WgBorders(0) );
	p->_setWidget( pWidget.GetRealPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->SetStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
	return p;
}


WgFlexHook * WgFlexPanel::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, WgBorders padding )
{
	return InsertWidget( pWidget, pSibling, geometry, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::InsertWidget(const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), padding );
	p->_setWidget( pWidget.GetRealPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->SetFloating( geometry, origo, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, WgBorders padding )
{
	return InsertWidget( pWidget, pSibling, pos, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::InsertWidget( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorders padding )
{
	if( !pWidget || !pSibling || !pSibling->Hook() || pSibling->Hook()->Parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->PreferredSize()), padding );
	p->_setWidget( pWidget.GetRealPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->SetFloating( pos, origo, hotspot );
	return p;
}


//____ RemoveWidget() _________________________________________________________

bool WgFlexPanel::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Force rendering of the area the widget was covering

	WgFlexHook * pHook = (WgFlexHook *) pWidget->_hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Remove the widget and return

	delete pHook;
	return true;
}

//____ Clear() ______________________________________________________

bool WgFlexPanel::Clear()
{
	WgPatches	dirt;

	// Collect dirty areas and delete hooks.

	WgFlexHook * pHook = m_hooks.First();
	while( pHook )
	{
		if( pHook->m_bVisible )
			dirt.Add( pHook->m_realGeo );
		WgFlexHook * pDelete = pHook;
		pHook = pHook->_next();
		delete pDelete;
	}

	// RequestRender on all dirty patches

	for( const WgRect * pRect = dirt.Begin() ; pRect != dirt.End() ; pRect++ )
		_requestRender( * pRect );

	return true;
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
		pHook = pHook->_next();
	}
	return minSize;
}

//____ _onRequestRender() ______________________________________________________

void WgFlexPanel::_onRequestRender( const WgRect& rect, const WgFlexHook * pHook )
{
	if( !pHook->m_bVisible )
		return;

	// Clip our geometry and put it in a dirtyrect-list

	WgPatches patches;
	patches.Add( WgRect( rect, WgRect(0,0,Size())) );

	// Remove portions of patches that are covered by opaque upper siblings

	WgFlexHook * pCover = pHook->_next();
	while( pCover )
	{
		if( pCover->m_bVisible && pCover->m_realGeo.IntersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_realGeo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_next();
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
		pHook = pHook->_prev();
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

