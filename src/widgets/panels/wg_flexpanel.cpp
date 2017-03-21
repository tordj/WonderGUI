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
	
	float	FlexPos::s_origoTab[9][2] = {	0.f, 0.f,
												0.5f, 0.f,
												1.f, 0.f,
												1.f, 0.5f,
												1.f, 1.f,
												0.5f, 1.f,
												0.f, 1.f,
												0.f, 0.5f,
												0.5f, 0.5f };


	//____ addPinned() _________________________________________________________
	
	bool FlexPanelChildren::addPinned( const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
			FlexPanelSlot * pSlot = m_pSlotArray->add();
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
			
			pSlot->bPinned = true;
			pSlot->topLeftPin = topLeft;
			pSlot->bottomRightPin = bottomRight;
			
			m_pHolder->_didAddSlots(pSlot, 1);
			return true;
	}
	
	//____ addMovable() ________________________________________________________
	
	bool FlexPanelChildren::addMovable( const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
			FlexPanelSlot * pSlot = m_pSlotArray->add();
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
			
			pSlot->placementGeo = geometry;
			pSlot->origo = origo;
			pSlot->hotspot = hotspot;
			
			m_pHolder->_didAddSlots(pSlot, 1);
			return true;
	}
	
	//____ insertPinned() ________________________________________________________

	bool FlexPanelChildren::insertPinned( int index, const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
			if( index < 0 || index >= m_pSlotArray->size() )
				return false;

			FlexPanelSlot * pSlot = m_pSlotArray->insert(index);
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());

			pSlot->bPinned = true;
			pSlot->topLeftPin = topLeft;
			pSlot->bottomRightPin = bottomRight;

			m_pHolder->_didAddSlots(pSlot, 1);
			return true;				
	}
	
	//____ insertMovable() ________________________________________________________
		
	bool FlexPanelChildren::insertMovable( int index, const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
			if( index < 0 || index >= m_pSlotArray->size() )
				return false;

			FlexPanelSlot * pSlot = m_pSlotArray->insert(index);
			pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());

			pSlot->placementGeo = geometry;
			pSlot->origo = origo;
			pSlot->hotspot = hotspot;

			m_pHolder->_didAddSlots(pSlot, 1);
			return true;		
	}

	//____ setPinned() ________________________________________________________
	
	bool FlexPanelChildren::setPinned( int index )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);

		if( !p->bPinned )
		{
			p->bPinned = true;
			
			Size sz = m_pHolder->size();
			
			p->topLeftPin		= FlexPos( p->realGeo.x/(float)sz.w, p->realGeo.y/(float)sz.h );
			p->bottomRightPin	= FlexPos( p->realGeo.right()/(float)sz.w, p->realGeo.bottom()/(float)sz.h );
		
			m_pHolder->_refreshRealGeo( p );
		}
		return true;
	}
	
	bool FlexPanelChildren::setPinned( int index, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);

		p->bPinned			= true;
		p->topLeftPin		= topLeft;
		p->bottomRightPin	= bottomRight;
			
		m_pHolder->_refreshRealGeo( p );
		return true;
	}
	
	//____ setMovable() ________________________________________________________
		
	bool FlexPanelChildren::setMovable( int index, const FlexPos& origo, const FlexPos& hotspot )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);

		if( p->bPinned )
		{
			p->bPinned 		= false;
			p->origo		= origo;
			p->hotspot		= hotspot;		
			p->placementGeo	= p->realGeo - origo.pos( m_pHolder->size() ) + hotspot.pos(p->realGeo);		
			
			m_pHolder->_refreshRealGeo( p );
		}
		return true;
	}
	
	bool FlexPanelChildren::setMovable( int index, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
					
		p->bPinned 		= false;
		p->origo		= origo;
		p->hotspot		= hotspot;		
		p->placementGeo	= geometry;

		m_pHolder->_refreshRealGeo( p );
		return true;
	}
	
	//____ moveToBack() ________________________________________________________
		
	bool FlexPanelChildren::moveToBack( int index )
	{
		return moveBelow( index, 0 );
	}
	
	//____ moveToFront() ________________________________________________________

	bool FlexPanelChildren::moveToFront( int index )
	{
		return moveAbove( index, m_pSlotArray->size()-1 );
	}
		
	//____ moveAbove() ________________________________________________________
	
	bool FlexPanelChildren::moveAbove( int index, int otherWidget )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		if( otherWidget < 0 || otherWidget >= m_pSlotArray->size() )
			return false;

		if( index > otherWidget )
			otherWidget++;

		if( index == otherWidget )
			return false;

		m_pSlotArray->move( index, otherWidget );

		FlexPanelSlot * pOther = m_pSlotArray->slot(index);				// This is correct, we have already switched places...
		FlexPanelSlot * pIndex = m_pSlotArray->slot(otherWidget);

	
		if( pIndex->bVisible )
		{
			if( pIndex > pOther )			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.
		
				FlexPanelSlot * p = pOther;
				while( p < pIndex )
				{
					Rect cover( pIndex->realGeo, p->realGeo );

					if( p->bVisible && !cover.isEmpty() )
						m_pHolder->_onRequestRender( cover, pIndex );
					p++;
				}
			}
			else							// Move us backward
			{
		
				// Request render on our siblings for the area we previously have covered.
		
				FlexPanelSlot * p = pIndex+1;
				while( p <= pOther )
				{
					Rect cover( pIndex->realGeo, p->realGeo );
	
					if( p->bVisible && !cover.isEmpty() )
						m_pHolder->_onRequestRender( cover, p );
					p++;
				}
			}
		}
		
		return true;
		
	}
		
	//____ moveBelow() ________________________________________________________
	
	bool FlexPanelChildren::moveBelow( int index, int otherWidget )
	{
		
	}
	
	//____ isMovable() ________________________________________________________
		
	bool FlexPanelChildren::isMovable( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;
			
		return !m_pSlotArray->slot(index)->bPinned;
	}
		
	//____ isPinned() ________________________________________________________
	
	bool FlexPanelChildren::isPinned( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;
			
		return m_pSlotArray->slot(index)->bPinned;		
	}
	
	//____ setOrigo() ________________________________________________________	
	
	bool FlexPanelChildren::setOrigo( int index, const FlexPos& origo )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->origo = origo;
		m_pHolder->_refreshRealGeo( p );
	
		return true;		
	}
	
	//____ origo() ________________________________________________________
	
	FlexPos FlexPanelChildren::origo( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return FlexPos();
			
		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return FlexPos();
			
		return m_pSlotArray->slot(index)->origo;				
	}
		
	//____ setHotspot() ________________________________________________________
	
	bool FlexPanelChildren::setHotspot( int index, const FlexPos& hotspot )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->hotspot = hotspot;
		m_pHolder->_refreshRealGeo( p );
	
		return true;		
	}

	//____ hotspot() ________________________________________________________
	
	FlexPos FlexPanelChildren::hotspot( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return FlexPos();
			
		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return FlexPos();
			
		return m_pSlotArray->slot(index)->hotspot;
	}
	
	//____ setGeo() ________________________________________________________
		
	bool FlexPanelChildren::setGeo( int index, const Rect& geometry )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->placementGeo = geometry;
		m_pHolder->_refreshRealGeo( p );
	
		return true;		
	}

	//____ geo() ________________________________________________________
	
	Rect FlexPanelChildren::geo( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return Rect();
			
		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return Rect();
			
		return m_pSlotArray->slot(index)->placementGeo;
	}

	//____ setOfs() ________________________________________________________
	
	bool FlexPanelChildren::setOfs( int index, const Coord& ofs )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->placementGeo.setPos(ofs);
		m_pHolder->_refreshRealGeo( p );
	
		return true;		
	}
	
	//____ ofs() ________________________________________________________
		
	Coord FlexPanelChildren::ofs( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return Coord();
			
		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return Coord();
			
		return m_pSlotArray->slot(index)->placementGeo.pos();		
	}

	//____ setSize() ________________________________________________________
	
	bool FlexPanelChildren::setSize( int index, const Size& size )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->placementGeo.setSize(size);
		m_pHolder->_refreshRealGeo( p );
	
		return true;		
	}
	
	//____ size() ________________________________________________________
		
	Rect FlexPanelChildren::size( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return Size();
			
		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return Size();
			
		return m_pSlotArray->slot(index)->placementGeo.size();		
	}

	//____ move() ________________________________________________________
	
	bool FlexPanelChildren::move( int index, const Coord& ofs )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( p->bPinned )
			return false;
	
		p->placementGeo += ofs;
		m_pHolder->_refreshRealGeo( p );
	
		return true;				
	}
	
	//____ topLeftCorner() ________________________________________________________
		
	FlexPos FlexPanelChildren::topLeftCorner( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return FlexPos();

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( !p->bPinned )
			return FlexPos();

		return p->topLeftPin;
	}

	//____ bottomRightCorner() ________________________________________________________
	
	FlexPos FlexPanelChildren::bottomRightCorner( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return FlexPos();

		FlexPanelSlot * p = m_pSlotArray->slot(index);
		if( !p->bPinned )
			return FlexPos();

		return p->bottomRightPin;		
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

			FlexPanelSlot * p = m_children.begin();

			while( p < m_children.end() )
			{
				_refreshRealGeo( p );
				p++;
			}	
		}
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size FlexPanel::preferredSize() const
	{
		Size minSize;
	
		FlexPanelSlot * p = m_children.begin();
		while( p < m_children.end() )
		{
			minSize = Size::max(minSize,_sizeNeededForGeo(p));
			p++;
		}			
		return minSize;
	}
	
	
	//____ _didAddSlots() _____________________________________________________________
		
	void FlexPanel::_didAddSlots( Slot * _pSlot, int nb )
	{
		_unhideSlots(_pSlot,nb);
	}
	
	//____ _willRemoveSlots() _____________________________________________________________
	
	void FlexPanel::_willRemoveSlots( Slot * pSlot, int nb )
	{
		_hideSlots(pSlot,nb);
	}
	
	//____ _hideSlots() _____________________________________________________________

	void FlexPanel::_hideSlots( PanelSlot *, int nb )
	{
		FlexPanelSlot * pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == true )
			{
				pSlot[i].bVisible = false;
				_onRequestRender(pSlot[i].realGeo);
			}
		}		
	}
	
	//____ _unhideSlots() _____________________________________________________________

	void FlexPanel::_unhideSlots( PanelSlot *, int nb )
	{
		FlexPanelSlot * pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == false )
			{
				pSlot[i].bVisible = true;
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].realGeo);
			}
		}
	}

		
	//____ _onRequestRender() ______________________________________________________
	
	void FlexPanel::_onRequestRender( const Rect& rect, const FlexPanelSlot * pSlot )
	{
		if( !pSlot->bVisible )
			return;
	
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,size())) );
	
		// Remove portions of patches that are covered by opaque upper siblings
	
		
	
		FlexPanelSlot * pCover = pSlot+1;
		while( pCover < m_children.end() )
		{
			if( pCover->bVisible && pCover->realGeo.intersectsWith( rect ) )
				pCover->pWidget->_maskPatches( patches, pCover->realGeo, Rect(0,0,65536,65536 ), _getBlendMode() );
	
			pCover++;
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

		FlexPanelSlot * p = m_children.begin();
		while( p < m_children.end() )
		{
			_refreshRealGeo(p);
			p++;
		}
	}

	//____ _childPos() ________________________________________________________

	Coord FlexPanel::_childPos( void * pChildRef ) const
	{
		return ((FlexPanelSlot*)pChildRef)->realGeo.pos();
	}

	//____ _childSize() __________________________________________________________

	Size FlexPanel::_childSize( void * pChildRef ) const
	{
		return ((FlexPanelSlot*)pChildRef)->realGeo.size();
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( void * pChildRef )
	{
		FlexPanelSlot * pSlot = reinterpret_cast<FlexPanelSlot*>(pChildRef);
		_onRequestRender( pSlot->realGeo, pSlot );
	}
	
	void FlexPanel::_childRequestRender( void * pChildRef, const Rect& rect )
	{
		FlexPanelSlot * pSlot = reinterpret_cast<FlexPanelSlot*>(pChildRef);
		_onRequestRender( rect + pSlot->m_realGeo.pos(), pSlot );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( void * pChildRef )
	{
		FlexPanelSlot * pSlot = reinterpret_cast<FlexPanelSlot*>(pChildRef);
		
		_refreshRealGeo(pSlot);
	}


	//____ _firstChild() __________________________________________________________

	Widget * FlexPanel::_firstChild() const 
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->pWidget;
	}

	//____ _lastChild() __________________________________________________________

	Widget * FlexPanel::_lastChild() const 
	{ 
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->pWidget;
	}


	//____ _prevChild() __________________________________________________________

	Widget * FlexPanel::_prevChild( void * pChildRef ) const
	{
		FlexPanelSlot * pSlot = reinterpret_cast<FlexPanelSlot*>(pChildRef);

		if (pSlot > m_children.begin())
			return pSlot[-1].pWidget;

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * FlexPanel::_nextChild( void * pChildRef ) const
	{
		FlexPanelSlot * pSlot = reinterpret_cast<FlexPanelSlot*>(pChildRef);

		if (pSlot < m_children.last())
			return pSlot[1].pWidget;

		return nullptr;
	}


	//____ _firstChildWithGeo() _____________________________________________________
	
	void FlexPanel::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pWidget = nullptr;
		else
		{
			FlexPanelSlot * pSlot = m_children.first();
			package.pMagic = pSlot;
			package.pWidget = pSlot->pWidget;
			package.geo = pSlot->realGeo;			
		}			
	}
	
	//____ _nextChildWithGeo() ______________________________________________________
	
	void FlexPanel::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		PackPanelSlot * pSlot = (PackPanelSlot*) package.pMagic;
		
		if( pSlot == m_children.last() )
			package.pWidget = nullptr;
		else
		{
			pSlot++;
			package.pMagic = pSlot;
			package.pWidget = pSlot->pWidget;
			package.geo = pSlot->realGeo;			
		}	
	}
	

	//____ _refreshRealGeo() ___________________________________________
	
	void FlexPanel::_refreshRealGeo( FlexPanelSlot * pSlot )
	{
		Rect	newGeo;
	
		if( pSlot->bPinned )
		{
			Coord topLeft = pSlot->topLeftPin.pos( m_size );
			Coord bottomRight = pSlot->bottomRightPin.pos( m_size );
	
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
		else
		{
			// Calculate size
	
			Size sz = pSlot->placementGeo.isEmpty() ? pSlot->pWidget->preferredSize() : pSlot->placementGeo.size();
			sz.limit( pSlot->pWidget->minSize(), pSlot->pWidget->maxSize() );		// Respect widgets limits.
	
			// Calculate position
	
			Coord pos = pSlot->origo.pos( m_size );			// Origo,
			pos -= pSlot->hotspot.pos(sz);					// hotspot
			pos += pSlot->placementGeo.pos();				// and Offset.
	
			// Limit size/pos according to parent
	
			if( isConfiningWidgets() )
			{
				if( sz.w > m_size.w )
					sz.w = m_size.w;
				if( sz.h > m_size.h )
	
				if( pos.x + sz.w > m_size.w )
					pos.x = m_size.w - sz.w;
	
				if( pos.y + sz.h > m_size.h )
					pos.y = m_size.h - sz.h;
			}
	
			newGeo = Rect( pos, sz );
		}

		// Request render and update positions.
	
		if( newGeo != m_realGeo )
		{
			_onRequestRender( pSlot->realGeo, pSlot );
			pSlot->realGeo = newGeo;
			pSlot->pWidget->_setSize(newGeo);
			_onRequestRender( pSlot->realGeo, pSlot );
		}
	}
	
	//____ _sizeNeededForGeo() ________________________________________
	
	Size FlexPanel::_sizeNeededForGeo( FlexPanelSlot * pSlot )
	{
		Size sz;
	
		if( pSlot->bPinned )
		{
			sz = pSlot->pWidget->preferredSize();
	
			sz += Size( pSlot->topLeftPin.offset.x, pSlot->topLeftPin.offset.y );
			sz -= Size( pSlot->bottomRightPin.offset.x, pSlot->bottomRightPin.offset.y );
	
			sz.w = (int) (sz.w / (float) (pSlot->bottomRightPin.origo.x - pSlot->topLeftPin.origo.x));
			sz.h = (int) (sz.w / (float) (pSlot->bottomRightPin.origo.y - pSlot->topLeftPin.origo.y));
		}
		else
		{
	        Rect geo = pSlot->placementGeo;
	        
			Coord hotspot = pSlot->hotspot.pos(geo.size());
			Coord offset = geo.pos() - hotspot;
	
			int leftOfOrigo = 0 - offset.x;
			int rightOfOrigo = offset.x + geo.w;
			int aboveOrigo = 0 - offset.y;
			int belowOrigo = offset.y + geo.h;
	
			if( leftOfOrigo > 0 )
				sz.w = (int) (leftOfOrigo / pSlot->origo.x);
	
			if( rightOfOrigo > 0 )
			{
				int w = (int) (rightOfOrigo / (1.f - pSlot->origo.x) );
				if( sz.w < w )
					sz.w = w;
			}
	
			if( aboveOrigo > 0 )
				sz.h = (int) (aboveOrigo / pSlot->origo.y);
	
			if( belowOrigo > 0 )
			{
				int h = (int) (belowOrigo / (1.f - pSlot->origo.y) );
				if( sz.h < h )
					sz.h = h;
			}
		}
	
		return sz;
	}

	

} // namespace wg
