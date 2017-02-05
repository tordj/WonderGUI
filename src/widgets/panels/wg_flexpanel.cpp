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

namespace wg 
{
	
	const char FlexPanel::CLASSNAME[] = {"FlexPanel"};
	const char FlexHook::CLASSNAME[] = {"FlexHook"};
	
	float	FlexOrigo::s_origoTab[9][2] = {	0.f, 0.f,
												0.5f, 0.f,
												1.f, 0.f,
												1.f, 0.5f,
												1.f, 1.f,
												0.5f, 1.f,
												0.f, 1.f,
												0.f, 0.5f,
												0.5f, 0.5f };
	
	//____ FlexHook::Constructor ________________________________________________
	
	FlexHook::FlexHook( FlexPanel * pParent, const Rect& placementGeo, Border padding ) : m_pParent(pParent),
		m_bFloating(false), m_widthPolicy(SizePolicy::Bound), m_heightPolicy(SizePolicy::Bound),
		m_origo(Origo::NorthWest), m_hotspot(Origo::NorthWest),
		m_placementGeo(placementGeo)
	{
	    m_padding = padding;
	}
	
	//____ FlexHook::isInstanceOf() __________________________________________
	
	bool FlexHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return PanelHook::isInstanceOf(pClassName);
	}
	
	//____ FlexHook::className() _____________________________________________
	
	const char * FlexHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ FlexHook::cast() __________________________________________________
	
	FlexHook_p FlexHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return FlexHook_p( static_cast<FlexHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	//____ FlexHook::setStretching() ______________________________________________
	
	bool  FlexHook::setStretching( const FlexOrigo& topLeftOrigo, 
									 const FlexOrigo& bottomRightOrigo, Border padding )
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
	
	//____ FlexHook::setStretching() ______________________________________________
	
	bool  FlexHook::setStretching( const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, 
		const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding )
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
	
	
	//____ FlexHook::setFloating() ______________________________________________
	
	bool FlexHook::setFloating( const Coord& pos, const FlexOrigo& origo )
	{
		return setFloating( pos, origo, origo );
	}
	
	bool FlexHook::setFloating( const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot )
	{
		m_widthPolicy	= SizePolicy::Default;
		m_heightPolicy	= SizePolicy::Default;
		m_bFloating		= true;
		m_origo			= origo;
		m_hotspot		= hotspot;
		m_placementGeo.setPos(pos);
	
		_refreshRealGeo();
		return true;
	}
	
	bool FlexHook::setFloating( const Rect& geometry, const FlexOrigo& origo )
	{
		return setFloating( geometry, origo, origo );
	}
	
	bool FlexHook::setFloating( const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot )
	{
		m_widthPolicy	= SizePolicy::Bound;
		m_heightPolicy	= SizePolicy::Bound;
		m_bFloating		= true;
		m_origo			= origo;
		m_hotspot		= hotspot;
		m_placementGeo	= geometry;
	
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::top() ______________________________________________________
	
	void FlexHook::top()
	{
		moveOver( _chain()->last() );
	}
	
	//____ FlexHook::bottom() ___________________________________________________
	
	void FlexHook::bottom()
	{
		moveUnder( _chain()->first() );
	}
	
	//____ FlexHook::up() _______________________________________________________
	
	bool FlexHook::up()
	{
		FlexHook * pNext = _next();
	
		if( pNext == 0 )
			return false;
	
		// Get area potentially exposed
	
		Rect cover( m_realGeo, pNext->m_realGeo );
	
		// Move up
	
		_moveUp();
	
		// Request render on potentially exposed area.
	
		if( pNext->m_bVisible && cover.w > 0 && cover.h > 0 )
		{
			m_pParent->_onRequestRender( cover, this );
		}
	
		return true;
	}
	
	//____ FlexHook::down() _____________________________________________________
	
	bool FlexHook::down()
	{
		FlexHook * pPrev = _prev();
	
		if( pPrev == 0 )
			return false;
	
		// Get area of sibling potentially exposed
	
		Rect cover( m_realGeo, pPrev->m_realGeo );
	
		// Move down
	
		_moveDown();
	
		// Request render on potentially exposed area.
	
		if( m_bVisible && cover.w > 0 && cover.h > 0 )
		{
			m_pParent->_onRequestRender( cover, pPrev );
		}
	
		return true;
	}
	
	//____ FlexHook::moveOver() _________________________________________________
	
	bool FlexHook::moveOver( const FlexHook_p& _pSibling )
	{
		FlexHook * pSibling = _pSibling.rawPtr();
	
		if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
			return false;
	
		FlexHook * pFirst = _chain()->first();
		while( pFirst != this && pFirst != pSibling )
			pFirst = pFirst->_next();
	
		FlexHook * pLast = pFirst->_next();
		while( pLast != this && pLast != pSibling )
			pLast = pLast->_next();
	
		if( pFirst == this )			// Move us forward
		{
			FlexHook * p = pFirst->_next();
	
			_moveAfter( pSibling );
	
			// Request render on all areas covered by siblings we have skipped in front of.
	
			if( m_bVisible )
			{
				while( p != this )
				{
					Rect cover( m_realGeo, p->m_realGeo );
	
					if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
						m_pParent->_onRequestRender( cover, this );
					p = p->_next();
				}
			}
		}
		else							// Move us backward
		{
			FlexHook * p = pLast->_prev();
			_moveAfter( pSibling );
	
			// Request render on our siblings for the area we previously have covered.
	
			if( m_bVisible )
			{
				while( p != this )
				{
					Rect cover( m_realGeo, p->m_realGeo );
	
					if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
						m_pParent->_onRequestRender( cover, p );
					p = p->_prev();
				}
			}
		}
	
		return true;
	}
	
	//____ FlexHook::moveUnder() ________________________________________________
	
	bool FlexHook::moveUnder( const FlexHook_p& _pSibling )
	{
		FlexHook * pSibling = _pSibling.rawPtr();
	
		if( !pSibling || pSibling->m_pParent != m_pParent || pSibling == this )
			return false;
	
		FlexHook * pFirst = _chain()->first();
		while( pFirst != this && pFirst != pSibling )
			pFirst = pFirst->_next();
	
		FlexHook * pLast = pFirst->_next();
		while( pLast != this && pLast != pSibling )
			pLast = pLast->_next();
	
		if( pFirst == this )			// Move us forward
		{
			FlexHook * p = _next();
			_moveBefore( pSibling );
	
			// Request render on all areas covered by siblings we have skipped in front of.
	
			if( m_bVisible )
			{
				while( p != this )
				{
					Rect cover( m_realGeo, p->m_realGeo );
	
					if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
						m_pParent->_onRequestRender( cover, this );
	
					p = p->_next();
				}
			}
		}
		else							// Move us backward
		{
			FlexHook * p = _prev();
			_moveBefore( pSibling );
	
			// Request render on our siblings for the area we previously have covered.
	
			if( m_bVisible )
			{
				while( p != this )
				{
					Rect cover( m_realGeo, p->m_realGeo );
	
					if( p->m_bVisible && cover.w > 0 && cover.h > 0 )
						m_pParent->_onRequestRender( cover, p );
	
					p = p->_prev();
				}
			}
		}
	
		return true;
	}
	
	//____ FlexHook::setOrigo() _________________________________________________
	
	bool FlexHook::setOrigo( const FlexOrigo& origo )
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
	
	//____ FlexHook::setHotspot() _______________________________________________
	
	bool FlexHook::setHotspot( const FlexOrigo& hotspot )
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
	
	//____ FlexHook::setSizePolicy() ____________________________________________
	
	bool FlexHook::setSizePolicy( SizePolicy width, SizePolicy height )
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
	
	//____ FlexHook::setGeo() ___________________________________________________
	
	bool FlexHook::setGeo( const Rect& geometry )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo = geometry;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setOfs() ___________________________________________________
	
	bool FlexHook::setOfs( const Coord& ofs )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.x = ofs.x;
		m_placementGeo.y = ofs.y;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setOfsX() __________________________________________________
	
	bool FlexHook::setOfsX( int x )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.x = x;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setOfsY() __________________________________________________
	
	bool FlexHook::setOfsY( int y )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.y = y;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setSize() __________________________________________________
	
	bool FlexHook::setSize( const Size& size )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.w = size.w;
		m_placementGeo.h = size.h;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setWidth() _________________________________________________
	
	bool FlexHook::setWidth( int width )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.w = width;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::setHeight() ________________________________________________
	
	bool FlexHook::setHeight( int height )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.h = height;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::move() _____________________________________________________
	
	bool FlexHook::move( const Coord& ofs )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo += ofs;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::moveX() ____________________________________________________
	
	bool FlexHook::moveX( int x )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.x += x;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::moveY() ____________________________________________________
	
	bool FlexHook::moveY( int y )
	{
		if( !m_bFloating )
			return false;
	
		m_placementGeo.y += y;
		_refreshRealGeo();
		return true;
	}
	
	//____ FlexHook::globalPos() ________________________________________________
	
	Coord FlexHook::globalPos() const
	{
		return m_realGeo.pos() + m_pParent->globalPos();
	}
	
	//____ FlexHook::globalGeo() ________________________________________________
	
	Rect FlexHook::globalGeo() const
	{
		return m_realGeo + m_pParent->globalPos();
	}
	
	//____ FlexHook::_prevHook() ________________________________________________
	
	Hook * FlexHook::_prevHook() const
	{
		return _prev();
	}
	
	//____ FlexHook::_nextHook() ________________________________________________
	
	Hook * FlexHook::_nextHook() const
	{
		return _next();
	}
	
	//____ FlexHook::_parent() __________________________________________________
	
	Container * FlexHook::_parent() const
	{
		return m_pParent;
	}
	
	//____ FlexHook::_refreshRealGeo() ___________________________________________
	
	void FlexHook::_refreshRealGeo()
	{
		Rect	newGeo;
		Size	parentSize = m_pParent->size();
	
		if( m_bFloating )
		{
			// Calculate size
	
			Size sz = _sizeFromPolicy( m_placementGeo.size() + m_padding, m_widthPolicy, m_heightPolicy );
	
			// Calculate position
	
			Coord pos = m_origo.position( parentSize );	// Origo,
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
	
			sz.limit( m_pWidget->minSize(), m_pWidget->maxSize() );		// Respect widgets limits.
			newGeo = Rect( pos, sz );
		}
		else
		{
			Coord topLeft = m_topLeftOrigo.position( parentSize ) + m_topLeftOfs;
			Coord bottomRight = m_bottomRightOrigo.position( parentSize ) + m_bottomRightOfs;
	
			newGeo = Rect(topLeft,bottomRight);
			
			// Respect widgets limits, apply in such a way that rectangle centers in specified rectangle
			
			Size sz = newGeo.size();
			sz.limit( m_pWidget->minSize(), m_pWidget->maxSize() );
			if( sz != newGeo.size() )
			{
				newGeo.x += newGeo.w - sz.w / 2;
				newGeo.y += newGeo.h - sz.h / 2;
				newGeo.w = sz.w;
				newGeo.h = sz.h;
			}
		}
	    newGeo.shrink( m_padding );

		// Request render and update positions.
	
		if( newGeo != m_realGeo )
		{
			m_pWidget->_requestRender();
			m_realGeo = newGeo;
			m_pWidget->_setSize(newGeo);
			m_pWidget->_requestRender();
		}
	}
	
	//____ FlexHook::_sizeNeededForGeo() ________________________________________
	
	Size FlexHook::_sizeNeededForGeo()
	{
		Size sz;
	
		if( m_bFloating )
		{
	        Rect geo = m_placementGeo + m_padding;
	        
			Coord hotspot = m_hotspot.position(geo.size());
			Coord offset = geo.pos() - hotspot;
	
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
	
			sz += Size( m_topLeftOfs.x, m_topLeftOfs.y );
			sz -= Size( m_bottomRightOfs.x, m_bottomRightOfs.y );
	
			sz.w = (int) (sz.w / (float) (m_bottomRightOrigo.x - m_topLeftOrigo.x));
			sz.h = (int) (sz.w / (float) (m_bottomRightOrigo.y - m_topLeftOrigo.y));
		}
	
		return sz;
	}
	
	
	//____ Constructor ____________________________________________________________
	
	FlexPanel::FlexPanel() : m_bConfineWidgets(false)
	{
		m_bSiblingsOverlap = true;
	}
	
	//____ Destructor _____________________________________________________________
	
	FlexPanel::~FlexPanel()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool FlexPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * FlexPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	FlexPanel_p FlexPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return FlexPanel_p( static_cast<FlexPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setConfineWidgets() ___________________________________________________
	
	void FlexPanel::setConfineWidgets( bool bConfineWidgets )
	{
		if( bConfineWidgets != m_bConfineWidgets )
		{
			m_bConfineWidgets = bConfineWidgets;
	
			FlexHook * pHook = m_hooks.first();
			while( pHook )
			{
				pHook->_refreshRealGeo();
				pHook = pHook->_next();
			}
		}
	}
	
	
	
	//____ addWidget() _____________________________________________________________
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget )
	{
		if( !pWidget )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), Border(0) );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		m_hooks.pushBack(p);
	
		p->setFloating( Coord(0,0) );
		return p;
	}
	
	//____ addWidget() _____________________________________________________________
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const FlexOrigo& topLeftOrigo,
										 const FlexOrigo& bottomRightOrigo, Border padding )
	{
		if( !pWidget )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		m_hooks.pushBack(p);
	
		p->setStretching( topLeftOrigo, bottomRightOrigo, padding );
		return p;
	}
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, 
								 const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding )
	{
		if( !pWidget )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		m_hooks.pushBack(p);
	
		p->setStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
		return p;
	}
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const Coord& pos, const FlexOrigo& origo, Border padding )
	{
		return addWidget( pWidget, pos, origo, origo, padding );
	}
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding )
	{
		if( !pWidget )
			return 0;
	
		Size bestSize = pWidget->preferredSize();
	
		FlexHook * p = new FlexHook( this, Rect(0,0,bestSize), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		m_hooks.pushBack(p);
		p->setFloating( pos, origo, hotspot );
		return p;
	}
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const Rect& geometry, const FlexOrigo& origo, Border padding )
	{
		return addWidget( pWidget, geometry, origo, origo, padding );
	}
	
	FlexHook * FlexPanel::addWidget( const Widget_p& pWidget, const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding )
	{
		if( !pWidget )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		m_hooks.pushBack(p);
	
		p->setFloating( geometry, origo, hotspot );
		return p;
	}
	
	
	//____ insertWidget() _________________________________________________
	
	FlexHook * FlexPanel::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling )
	{
		if( !pWidget || !pSibling || pSibling->_parent() != this )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), Border(0) );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		p->_moveBefore( (FlexHook*) reinterpret_cast<Hook*>(pSibling->_holdersRef()) );
		p->setFloating( Coord(0,0) );
		return p;
	}
	
	
	FlexHook * FlexPanel::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const FlexOrigo& topLeftOrigo, const Coord& topLeftOfs, 
										   const FlexOrigo& bottomRightOrigo, const Coord& bottomRightOfs, Border padding )
	{
		if( !pWidget || !pSibling || pSibling->_parent() != this )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), Border(0) );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		p->_moveBefore( (FlexHook*) reinterpret_cast<Hook*>(pSibling->_holdersRef()) );
		p->setStretching( topLeftOrigo, topLeftOfs, bottomRightOrigo, bottomRightOfs, padding );
		return p;
	}
	
	
	FlexHook * FlexPanel::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Rect& geometry, const FlexOrigo& origo, Border padding )
	{
		return insertWidget( pWidget, pSibling, geometry, origo, origo, padding );
	}
	
	FlexHook * FlexPanel::insertWidget(const Widget_p& pWidget, const Widget_p& pSibling, const Rect& geometry, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding )
	{
		if( !pWidget || !pSibling || pSibling->_parent() != this )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		p->_moveBefore( (FlexHook*) reinterpret_cast<Hook*>(pSibling->_holdersRef()) );
		p->setFloating( geometry, origo, hotspot );
		return p;
	}
	
	FlexHook * FlexPanel::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Coord& pos, const FlexOrigo& origo, Border padding )
	{
		return insertWidget( pWidget, pSibling, pos, origo, origo, padding );
	}
	
	FlexHook * FlexPanel::insertWidget( const Widget_p& pWidget, const Widget_p& pSibling, const Coord& pos, const FlexOrigo& origo, const FlexOrigo& hotspot, Border padding )
	{
		if( !pWidget || !pSibling ||  pSibling->_parent() != this )
			return 0;
	
		FlexHook * p = new FlexHook( this, Rect(0,0,pWidget->preferredSize()), padding );
		p->_setWidget( pWidget.rawPtr() );
		pWidget->_setHolder( this, (Hook*) p );

		p->_moveBefore( (FlexHook*) reinterpret_cast<Hook*>(pSibling->_holdersRef()) );
		p->setFloating( pos, origo, hotspot );
		return p;
	}
	
	
	//____ removeWidget() _________________________________________________________
	
	bool FlexPanel::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget || pWidget->_parent() != this )
			return false;
	
		// Force rendering of the area the widget was covering
	
		FlexHook * pHook = (FlexHook *)reinterpret_cast<Hook*>(pWidget->_holdersRef());
		pWidget->_setHolder( nullptr, nullptr );
		_onRequestRender( pHook->m_realGeo, pHook );
	
		// Remove the widget and return
	
		delete pHook;
		return true;
	}
	
	//____ clear() ______________________________________________________
	
	bool FlexPanel::clear()
	{
		Patches	dirt;
	
		// Collect dirty areas and delete hooks.
	
		FlexHook * pHook = m_hooks.first();
		while( pHook )
		{
			pHook->_widget()->_setHolder( nullptr, nullptr );
			if( pHook->m_bVisible )
				dirt.add( pHook->m_realGeo );
			FlexHook * pDelete = pHook;
			pHook = pHook->_next();
			delete pDelete;
		}
	
		// RequestRender on all dirty patches
	
		for( const Rect * pRect = dirt.begin() ; pRect != dirt.end() ; pRect++ )
			_requestRender( * pRect );
	
		return true;
	}
	
	
	//____ preferredSize() _____________________________________________________________
	
	Size FlexPanel::preferredSize() const
	{
		Size minSize;
	
		FlexHook * pHook = m_hooks.first();
		while( pHook )
		{
			Size sz = pHook->_sizeNeededForGeo();
			minSize = Size::max(minSize,sz);
			pHook = pHook->_next();
		}
		return minSize;
	}
	
	//____ _onRequestRender() ______________________________________________________
	
	void FlexPanel::_onRequestRender( const Rect& rect, const FlexHook * pHook )
	{
		if( !pHook->m_bVisible )
			return;
	
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,size())) );
	
		// Remove portions of patches that are covered by opaque upper siblings
	
		FlexHook * pCover = pHook->_next();
		while( pCover )
		{
			if( pCover->m_bVisible && pCover->m_realGeo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->m_realGeo, Rect(0,0,65536,65536 ), _getBlendMode() );
	
			pCover = pCover->_next();
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void FlexPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement
	}
	
	//____ _setSize() ____________________________________________________________
	
	void FlexPanel::_setSize( const Size& size )
	{
		Panel::_setSize(size);
		FlexHook * pHook = m_hooks.last();
	
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_prev();
		}
	}

	//____ _childPos() ________________________________________________________

	Coord FlexPanel::_childPos( void * pChildRef ) const
	{
		return ((Hook*)pChildRef)->pos();
	}

	//____ _childSize() __________________________________________________________

	Size FlexPanel::_childSize( void * pChildRef ) const
	{
		return ((Hook*)pChildRef)->size();
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( void * pChildRef )
	{
		FlexHook * pHook = static_cast<FlexHook*>(reinterpret_cast<Hook*>(pChildRef));
		_onRequestRender( pHook->m_realGeo, pHook );
	}
	
	void FlexPanel::_childRequestRender( void * pChildRef, const Rect& rect )
	{
		FlexHook * pHook = static_cast<FlexHook*>(reinterpret_cast<Hook*>(pChildRef));
		_onRequestRender( rect + pHook->m_realGeo.pos(), pHook );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( void * pChildRef )
	{
		FlexHook * pHook = static_cast<FlexHook*>(reinterpret_cast<Hook*>(pChildRef));

		if( pHook->m_widthPolicy != SizePolicy::Bound || pHook->m_heightPolicy != SizePolicy::Bound )
			pHook->_refreshRealGeo();
	}


	//____ _firstChild() __________________________________________________________

	Widget * FlexPanel::_firstChild() const 
	{
		FlexHook * p = m_hooks.first();
		return  p ? p->_widget() : nullptr; 
	}

	//____ _lastChild() __________________________________________________________

	Widget * FlexPanel::_lastChild() const 
	{ 
		FlexHook * p = m_hooks.last();
		return  p ? p->_widget() : nullptr; 
	}


	//____ _prevChild() __________________________________________________________

	Widget * FlexPanel::_prevChild( void * pChildRef ) const
	{
		Hook *p = ((Hook*)pChildRef)->_prevHook();
		return p ? p->_widget() : nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * FlexPanel::_nextChild( void * pChildRef ) const
	{
		Hook *p = ((Hook*)pChildRef)->_nextHook();
		return p ? p->_widget() : nullptr;
	}


	//____ _firstChildWithGeo() _____________________________________________________
	
	void FlexPanel::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		FlexHook * p = m_hooks.first();
		if( p )
		{
			package.pMagic = p;
			package.pWidget = p->_widget();
			package.geo = p->m_realGeo;
		}
		else
			package.pWidget = nullptr;
	}
	
	//____ _nextChildWithGeo() ______________________________________________________
	
	void FlexPanel::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		FlexHook * p = ((FlexHook*)package.pMagic)->_next();
		if( p )
		{
			package.pMagic = p;
			package.pWidget = p->_widget();
			package.geo = p->m_realGeo;
		}
		else
			package.pWidget = nullptr;
	}
		

} // namespace wg
