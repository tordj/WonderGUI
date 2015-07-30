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

WgFlexHook::WgFlexHook( WgFlexPanel * pParent, const WgRect& placementGeo, WgBorder padding ) : m_pParent(pParent),
	m_bFloating(false), m_widthPolicy(WG_BOUND), m_heightPolicy(WG_BOUND),
	m_origo(WG_NORTHWEST), m_hotspot(WG_NORTHWEST),
	m_placementGeo(placementGeo)
{
    m_padding = padding;
}

//____ WgFlexHook::isInstanceOf() __________________________________________

bool WgFlexHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanelHook::isInstanceOf(pClassName);
}

//____ WgFlexHook::className() _____________________________________________

const char * WgFlexHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgFlexHook::cast() __________________________________________________

WgFlexHook_p WgFlexHook::cast( const WgHook_p& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgFlexHook_p( static_cast<WgFlexHook*>(pHook.rawPtr()) );

	return 0;
}


//____ WgFlexHook::setStretching() ______________________________________________

bool  WgFlexHook::setStretching( const WgFlexOrigo& topLeftOrigo, 
								 const WgFlexOrigo& bottomRightOrigo, WgBorder padding )
{
	m_bFloating			= false;
	m_topLeftOrigo		= topLeftOrigo;
	m_bottomRightOrigo	= bottomRightOrigo;
	m_padding			= padding;

	m_topLeftOfs.clear();
	m_bottomRightOfs.clear();

	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setStretching() ______________________________________________

bool  WgFlexHook::setStretching( const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
	const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding )
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


//____ WgFlexHook::setFloating() ______________________________________________

bool WgFlexHook::setFloating( const WgCoord& pos, const WgFlexOrigo& origo )
{
	return setFloating( pos, origo, origo );
}

bool WgFlexHook::setFloating( const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot )
{
	m_widthPolicy	= WG_DEFAULT;
	m_heightPolicy	= WG_DEFAULT;
	m_bFloating		= true;
	m_origo			= origo;
	m_hotspot		= hotspot;
	m_placementGeo.setPos(pos);

	_refreshRealGeo();
	return true;
}

bool WgFlexHook::setFloating( const WgRect& geometry, const WgFlexOrigo& origo )
{
	return setFloating( geometry, origo, origo );
}

bool WgFlexHook::setFloating( const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot )
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

//____ WgFlexHook::top() ______________________________________________________

void WgFlexHook::top()
{
	moveOver( _chain()->last() );
}

//____ WgFlexHook::bottom() ___________________________________________________

void WgFlexHook::bottom()
{
	moveUnder( _chain()->first() );
}

//____ WgFlexHook::up() _______________________________________________________

bool WgFlexHook::up()
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

//____ WgFlexHook::down() _____________________________________________________

bool WgFlexHook::down()
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

//____ WgFlexHook::moveOver() _________________________________________________

bool WgFlexHook::moveOver( const WgFlexHook_p& _pSibling )
{
	WgFlexHook * pSibling = _pSibling.rawPtr();

	if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
		return false;

	WgFlexHook * pFirst = _chain()->first();
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

//____ WgFlexHook::moveUnder() ________________________________________________

bool WgFlexHook::moveUnder( const WgFlexHook_p& _pSibling )
{
	WgFlexHook * pSibling = _pSibling.rawPtr();

	if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
		return false;

	WgFlexHook * pFirst = _chain()->first();
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

//____ WgFlexHook::setOrigo() _________________________________________________

bool WgFlexHook::setOrigo( const WgFlexOrigo& origo )
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

//____ WgFlexHook::setHotspot() _______________________________________________

bool WgFlexHook::setHotspot( const WgFlexOrigo& hotspot )
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

//____ WgFlexHook::setSizePolicy() ____________________________________________

bool WgFlexHook::setSizePolicy( WgSizePolicy width, WgSizePolicy height )
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

//____ WgFlexHook::setGeo() ___________________________________________________

bool WgFlexHook::setGeo( const WgRect& geometry )
{
	if( !m_bFloating )
		return false;

	m_placementGeo = geometry;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setOfs() ___________________________________________________

bool WgFlexHook::setOfs( const WgCoord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x = ofs.x;
	m_placementGeo.y = ofs.y;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setOfsX() __________________________________________________

bool WgFlexHook::setOfsX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x = x;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setOfsY() __________________________________________________

bool WgFlexHook::setOfsY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.y = y;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setSize() __________________________________________________

bool WgFlexHook::setSize( const WgSize& size )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.w = size.w;
	m_placementGeo.h = size.h;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setWidth() _________________________________________________

bool WgFlexHook::setWidth( int width )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.w = width;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::setHeight() ________________________________________________

bool WgFlexHook::setHeight( int height )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.h = height;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::move() _____________________________________________________

bool WgFlexHook::move( const WgCoord& ofs )
{
	if( !m_bFloating )
		return false;

	m_placementGeo += ofs;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::moveX() ____________________________________________________

bool WgFlexHook::moveX( int x )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.x += x;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::moveY() ____________________________________________________

bool WgFlexHook::moveY( int y )
{
	if( !m_bFloating )
		return false;

	m_placementGeo.y += y;
	_refreshRealGeo();
	return true;
}

//____ WgFlexHook::globalPos() ________________________________________________

WgCoord WgFlexHook::globalPos() const
{
	return m_realGeo.pos() + m_pParent->globalPos();
}

//____ WgFlexHook::globalGeo() ________________________________________________

WgRect WgFlexHook::globalGeo() const
{
	return m_realGeo + m_pParent->globalPos();
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
	WgSize	parentSize = m_pParent->size();

	if( m_bFloating )
	{
		// Calculate size

		WgSize sz = _sizeFromPolicy( m_placementGeo.size() + m_padding, m_widthPolicy, m_heightPolicy );

		// Calculate position

		WgCoord pos = m_origo.position( parentSize );	// Origo,
		pos -= m_hotspot.position(sz);					// hotspot
		pos += m_placementGeo.pos();					// and Offset.

		// Limit size/pos according to parent

		if( m_pParent->isConfiningWidgets() )
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
		WgCoord topLeft = m_topLeftOrigo.position( parentSize ) + m_topLeftOfs;
		WgCoord bottomRight = m_bottomRightOrigo.position( parentSize ) + m_bottomRightOfs;

		newGeo = WgRect(topLeft,bottomRight);
	}
    newGeo.shrink( m_padding );


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
        
		WgCoord hotspot = m_hotspot.position(geo.size());
		WgCoord offset = geo.pos() - hotspot;

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
		sz = m_pWidget->preferredSize() + m_padding;

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
	m_pParent->_onRequestRender( rect + m_realGeo.pos(), this );
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

//____ isInstanceOf() _________________________________________________________

bool WgFlexPanel::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgFlexPanel::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgFlexPanel_p WgFlexPanel::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgFlexPanel_p( static_cast<WgFlexPanel*>(pObject.rawPtr()) );

	return 0;
}

//____ setConfineWidgets() ___________________________________________________

void WgFlexPanel::setConfineWidgets( bool bConfineWidgets )
{
	if( bConfineWidgets != m_bConfineWidgets )
	{
		m_bConfineWidgets = bConfineWidgets;

		WgFlexHook * pHook = m_hooks.first();
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_next();
		}
	}
}



//____ addWidget() _____________________________________________________________

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), WgBorder(0) );
	p->_setWidget( pWidget.rawPtr() );

	m_hooks.pushBack(p);

	p->setFloating( WgCoord(0,0) );
	return p;
}

//____ addWidget() _____________________________________________________________

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgFlexOrigo& topLeftOrigo,
									 const WgFlexOrigo& bottomRightOrigo, WgBorder padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), padding );
	p->_setWidget( pWidget.rawPtr() );

	m_hooks.pushBack(p);

	p->setStretching( topLeftOrigo, bottomRightOrigo, padding );
	return p;
}

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
							 const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), padding );
	p->_setWidget( pWidget.rawPtr() );

	m_hooks.pushBack(p);

	p->setStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
	return p;
}

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, WgBorder padding )
{
	return addWidget( pWidget, pos, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding )
{
	if( !pWidget )
		return 0;

	WgSize bestSize = pWidget->preferredSize();

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,bestSize), padding );
	p->_setWidget( pWidget.rawPtr() );
	m_hooks.pushBack(p);
	p->setFloating( pos, origo, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, WgBorder padding )
{
	return addWidget( pWidget, geometry, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::addWidget( const WgWidget_p& pWidget, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding )
{
	if( !pWidget )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), padding );
	p->_setWidget( pWidget.rawPtr() );

	m_hooks.pushBack(p);

	p->setFloating( geometry, origo, hotspot );
	return p;
}


//____ insertWidget() _________________________________________________

WgFlexHook * WgFlexPanel::insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling )
{
	if( !pWidget || !pSibling || !pSibling->hook() || pSibling->hook()->parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), WgBorder(0) );
	p->_setWidget( pWidget.rawPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->setFloating( WgCoord(0,0) );
	return p;
}


WgFlexHook * WgFlexPanel::insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgFlexOrigo& topLeftOrigo, const WgCoord& topLeftOfs, 
									   const WgFlexOrigo& bottomRightOrigo, const WgCoord& bottomRightOfs, WgBorder padding )
{
	if( !pWidget || !pSibling || !pSibling->hook() || pSibling->hook()->parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), WgBorder(0) );
	p->_setWidget( pWidget.rawPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->setStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
	return p;
}


WgFlexHook * WgFlexPanel::insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, WgBorder padding )
{
	return insertWidget( pWidget, pSibling, geometry, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::insertWidget(const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgRect& geometry, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding )
{
	if( !pWidget || !pSibling || !pSibling->hook() || pSibling->hook()->parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), padding );
	p->_setWidget( pWidget.rawPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->setFloating( geometry, origo, hotspot );
	return p;
}

WgFlexHook * WgFlexPanel::insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, WgBorder padding )
{
	return insertWidget( pWidget, pSibling, pos, origo, origo, padding );
}

WgFlexHook * WgFlexPanel::insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling, const WgCoord& pos, const WgFlexOrigo& origo, const WgFlexOrigo& hotspot, WgBorder padding )
{
	if( !pWidget || !pSibling || !pSibling->hook() || pSibling->hook()->parent() != this )
		return 0;

	WgFlexHook * p = new WgFlexHook( this, WgRect(0,0,pWidget->preferredSize()), padding );
	p->_setWidget( pWidget.rawPtr() );
	p->_moveBefore( (WgFlexHook*)pSibling->_hook() );
	p->setFloating( pos, origo, hotspot );
	return p;
}


//____ removeWidget() _________________________________________________________

bool WgFlexPanel::removeWidget( const WgWidget_p& pWidget )
{
	if( !pWidget || !pWidget->hook() || pWidget->hook()->parent() != this )
		return false;

	// Force rendering of the area the widget was covering

	WgFlexHook * pHook = (WgFlexHook *) pWidget->_hook();
	_onRequestRender( pHook->m_realGeo, pHook );

	// Remove the widget and return

	delete pHook;
	return true;
}

//____ clear() ______________________________________________________

bool WgFlexPanel::clear()
{
	WgPatches	dirt;

	// Collect dirty areas and delete hooks.

	WgFlexHook * pHook = m_hooks.first();
	while( pHook )
	{
		if( pHook->m_bVisible )
			dirt.add( pHook->m_realGeo );
		WgFlexHook * pDelete = pHook;
		pHook = pHook->_next();
		delete pDelete;
	}

	// RequestRender on all dirty patches

	for( const WgRect * pRect = dirt.begin() ; pRect != dirt.end() ; pRect++ )
		_requestRender( * pRect );

	return true;
}


//____ preferredSize() _____________________________________________________________

WgSize WgFlexPanel::preferredSize() const
{
	WgSize minSize;

	WgFlexHook * pHook = m_hooks.first();
	while( pHook )
	{
		WgSize sz = pHook->_sizeNeededForGeo();
		minSize = WgSize::max(minSize,sz);
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
	patches.add( WgRect( rect, WgRect(0,0,size())) );

	// Remove portions of patches that are covered by opaque upper siblings

	WgFlexHook * pCover = pHook->_next();
	while( pCover )
	{
		if( pCover->m_bVisible && pCover->m_realGeo.intersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, pCover->m_realGeo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
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
	WgFlexHook * pHook = m_hooks.last();

	while( pHook )
	{
		pHook->_refreshRealGeo();
		pHook = pHook->_prev();
	}
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgFlexPanel::_firstHookWithGeo( WgRect& writeGeo ) const
{
	WgFlexHook * p = m_hooks.first();
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
	WgFlexHook * p = m_hooks.last();
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

