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

#include <wg_packpanel.h>
#include <wg_base.h>

namespace wg 
{
	
	const char PackPanel::CLASSNAME[] = {"PackPanel"};
	


	void PackPanelChildren::add( const Widget_p& pWidget )
	{
		auto pSlot = m_pSlotArray->add();
		pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
		pSlot->bVisible = false;
		m_pHolder->_didAddSlots(pSlot, 1);		
	}
	
	bool PackPanelChildren::insert( int index, const Widget_p& pWidget )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		auto pSlot = m_pSlotArray->insert(index);
 		pSlot->replaceWidget(m_pHolder,pWidget.rawPtr());
		pSlot->bVisible = false;
		m_pHolder->_didAddSlots(pSlot, 1);
		return true;		
	}
	
	bool PackPanelChildren::remove( int index )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;

		auto pSlot = m_pSlotArray->insert(index);
		m_pHolder->_willRemoveSlots(pSlot, 1);
		m_pSlotArray->remove(index);
		return true;		
	}

	bool PackPanelChildren::setWeight( int index, float weight )
	{
		if( index < 0 || index >= m_pSlotArray->size() || weight < 0.f )
			return false;

		auto pSlot = m_pSlotArray->slot(index);		

		if( weight != pSlot->weight )
		{
			pSlot->weight = weight;
			m_pHolder->_refreshChildGeo();
		}
		return true;
	}
	
	float PackPanelChildren::weight( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return 0.f;

		return m_pSlotArray->slot(index)->weight;
		
	}

	//____ Constructor ____________________________________________________________
	
	PackPanel::PackPanel() : children( &m_children, this )
	{
		m_bSiblingsOverlap = false;
		m_bHorizontal = true;
	    m_pSizeBroker = 0;
	}
	
	//____ Destructor _____________________________________________________________
	
	PackPanel::~PackPanel()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool PackPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * PackPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	PackPanel_p PackPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return PackPanel_p( static_cast<PackPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setOrientation() ______________________________________________________
	
	void PackPanel::setOrientation( Orientation orientation )
	{
		bool bHorizontal = orientation==Orientation::Horizontal?true:false;
		if( m_bHorizontal != bHorizontal )
		{
			m_bHorizontal = bHorizontal;
			_updatePreferredSize();
			_refreshChildGeo();
		}
	}
	
	
	//____ setSizeBroker() _______________________________________________________
	
	void PackPanel::setSizeBroker( const SizeBroker_p& pBroker )
	{
		if( m_pSizeBroker != pBroker )
		{
			m_pSizeBroker = pBroker;
			_updatePreferredSize();
			_refreshChildGeo();
		}
	}
	
	//____ preferredSize() _______________________________________________________
	
	Size PackPanel::preferredSize() const
	{
		return m_preferredSize;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int PackPanel::matchingHeight( int width ) const
	{
		int height = 0;
	
		if( m_bHorizontal )
		{
			if( m_pSizeBroker )
			{
				// Allocate and populate SizeBroker array
			
				int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
			
				int nItems = _populateSizeBrokerArray(pItemArea);		
			
				// Retrieve item lengths and find height of highest item.
	
				m_pSizeBroker->setItemLengths( pItemArea, nItems, width );
	
				SizeBrokerItem * pI = pItemArea;
	
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() )
					{
						int itemHeight = pS->paddedMatchingHeight( pI->output );
						if( itemHeight > height )
							height = itemHeight;
						pI++;
					}	
				}
	
				// Release temporary memory area
			
				Base::memStackRelease(arrayBytes);
			}
			else 
			{
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() && pS->preferredSize.h > height )
							height = pS->preferredSize.h;	
				}
			}
		}
		else
		{
			if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
			{
				// Allocate and populate SizeBroker array
			
				int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
			
				int nItems = _populateSizeBrokerArray(pItemArea, width);		
			
				// Retrieve preferred length
	
				height = m_pSizeBroker->setPreferredLengths( pItemArea, nItems );
					
				// Release temporary memory area
			
				Base::memStackRelease(arrayBytes);
			}
			else 
			{
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() )
						height += pS->paddedMatchingHeight( width );
				}
			}
		}
		return height;
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int PackPanel::matchingWidth( int height ) const
	{
		int width = 0;
	
		if( !m_bHorizontal )
		{
			if( m_pSizeBroker )
			{
				// Allocate and populate SizeBroker array
			
				int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
			
				int nItems = _populateSizeBrokerArray(pItemArea);		
			
				// Retrieve item lengths and find height of highest item.
	
				m_pSizeBroker->setItemLengths( pItemArea, nItems, height );
	
				SizeBrokerItem * pI = pItemArea;
	
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() )
					{
						int itemWidth = pS->paddedMatchingWidth( pI->output );
						if( itemWidth > width )
							width = itemWidth;
						pI++;
					}	
				}
	
				// Release temporary memory area
			
				Base::memStackRelease(arrayBytes);
			}
			else 
			{
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() && pS->preferredSize.w > width )
							width = pS->preferredSize.w;	
				}
			}
		}
		else
		{
			if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
			{
				// Allocate and populate SizeBroker array
			
				int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
				SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
			
				int nItems = _populateSizeBrokerArray(pItemArea, height);		
			
				// Retrieve preferred length
	
				width = m_pSizeBroker->setPreferredLengths( pItemArea, nItems );
					
				// Release temporary memory area
			
				Base::memStackRelease(arrayBytes);
			}
			else 
			{
				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->isVisible() )
						width += pS->paddedMatchingWidth( height );	
				}
			}
		}
		return width;
	}
	
	
	
	//____ _firstChildWithGeo() _____________________________________________________
	
	void PackPanel::_firstChildWithGeo( WidgetWithGeo& package ) const
	{	
		if( m_children.isEmpty() )
			package.pWidget = nullptr;
		else
		{
			PackPanelSlot * pSlot = m_children.first();
			package.pMagic = pSlot;
			package.pWidget = pSlot->pWidget;
			package.geo = pSlot->geo;			
		}			
	}
	
	//____ _nextChildWithGeo() _____________________________________________________
	
	void PackPanel::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		PackPanelSlot * pSlot = (PackPanelSlot*) package.pMagic;
		
		if( pSlot == m_children.last() )
			package.pWidget = nullptr;
		else
		{
			pSlot++;
			package.pMagic = pSlot;
			package.pWidget = pSlot->pWidget;
			package.geo = pSlot->geo;			
		}	
	}
		
	
	//____ _renderRequested() ____________________________________________________
	
	void PackPanel::_renderRequested( VectorHook * pHook )
	{
		PackHook * p = static_cast<PackHook*>(pHook);
		_requestRender( p->m_geo );
	}
	
	void PackPanel::_renderRequested( VectorHook * pHook, const Rect& rect )
	{
		PackHook * p = static_cast<PackHook*>(pHook);
		_requestRender( rect + p->m_geo.pos() );
	}
	
	//____ _requestResizeRequested() _____________________________________________________
	
	void PackPanel::_requestResizeRequested( VectorHook * pHook )
	{
		// Update cached preferred size of child
		
		PackHook * p = static_cast<PackHook*>(pHook);
		p->m_preferredSize = p->_paddedPreferredSize();
	
		//
		
		_refreshAllWidgets();
	}
	
	//____ _onWidgetAppeared() ______________________________________________________
	
	void PackPanel::_onWidgetAppeared( VectorHook * pInserted )
	{
		// Update cached preferred size of child
		
		PackHook * p = static_cast<PackHook*>(pInserted);
		p->m_preferredSize = p->_paddedPreferredSize();
		
		//
		
		_refreshAllWidgets();    
	}
	
	//____ _onWidgetDisappeared() ___________________________________________________
	
	void PackPanel::_onWidgetDisappeared( VectorHook * pToBeRemoved )
	{
		_refreshAllWidgets();
	}
	
	//____ _onWidgetsReordered() ____________________________________________________
	
	void PackPanel::_onWidgetsReordered()
	{
		_refreshChildGeo();
	}
	
	//____ _refreshAllWidgets() _____________________________________________________
	
	void PackPanel::_refreshAllWidgets()
	{
		_updatePreferredSize();
		_refreshChildGeo();
	}
	
	
	//____ _newHook() ____________________________________________________________
	
	VectorHook * PackPanel::_newHook()
	{
		return new PackHook(this);
	}
	
	//____ _setSize() ____________________________________________________________
	
	void PackPanel::_setSize( const Size& size )
	{
		Panel::_setSize(size);
	    _refreshChildGeo();
	}
	
	
	//____ _updatePreferredSize() ______________________________________________________
	
	void PackPanel::_updatePreferredSize()
	{
		int length = 0;
		int breadth = 0;
	
		if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
		{
			// Allocate and populate SizeBroker array
			
			int arrayBytes = sizeof(SizeBrokerItem)*m_hooks.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
			
			int nItems = _populateSizeBrokerArray(pItemArea);		
			
			// Retrieve preferred length and breadth
			
			length = m_pSizeBroker->setPreferredLengths( pItemArea, nItems );
			
			PackHook * pH = static_cast<PackHook*>(m_hooks.first());
			SizeBrokerItem * pI = pItemArea;
			while( pH )
			{
				if( pH->isVisible() )
				{
					int b = m_bHorizontal?pH->_paddedMatchingHeight(pI->output):pH->_paddedMatchingWidth(pI->output);
					if( b > breadth )
						breadth = b;			
					pI++;
				}
				pH = pH->_next();
			}
			
			// Release temporary memory area
			
			Base::memStackRelease(arrayBytes);
			
		}
		else
		{
			PackHook * p = static_cast<PackHook*>(m_hooks.first());
	
			if( m_bHorizontal )
			{
	            while( p )
	            {
					if( p->isVisible() )
					{
						length += p->m_preferredSize.w;
		                if( p->m_preferredSize.h > breadth )
		                    breadth = p->m_preferredSize.h;
					}
					p = p->_next();
	            }
			}
			else
			{
	            while( p )
	            {
					if( p->isVisible() )
					{
						length += p->m_preferredSize.h;
						if( p->m_preferredSize.w > breadth )
						    breadth = p->m_preferredSize.w;
					}
	                p = p->_next();
	            }
			}
		}
	
		//
		
		Size size = m_bHorizontal?Size(length,breadth):Size(breadth,length);
		if( size != m_preferredSize )
		{
			m_preferredSize = size;
			_requestResize();
		}
	}
	
	//____ _refreshChildGeo() _________________________________________________________
	
	void PackPanel::_refreshChildGeo()
	{
	    if( m_hooks.isEmpty() )
	        return;
	    
		Size sz = size();
		
		int wantedLength = m_bHorizontal?m_preferredSize.w:m_preferredSize.h;
		int givenLength = m_bHorizontal?sz.w:sz.h;
		int givenBreadth = m_bHorizontal?sz.h:sz.w;
	
		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth
	
		if( !m_pSizeBroker || (wantedLength == givenLength && !m_pSizeBroker->mayAlterPreferredLengths()) )
		{
			Coord pos;
			PackHook * p = static_cast<PackHook*>(m_hooks.first());
	        Rect geo;
			while( p )
			{
				if( p->isVisible() )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = p->m_preferredSize.w;
						geo.h = sz.h;
						pos.x += p->m_preferredSize.w;
					}
					else
					{
						geo.w = sz.w;
						geo.h = p->m_preferredSize.h;
						pos.y += p->m_preferredSize.h;
					}
					geo -= p->m_padding;
	            
					if( geo != p->m_geo )
					{
						_requestRender(geo);
						_requestRender(p->m_geo);
	        
						int oldW = p->m_geo.w;
						int oldH = p->m_geo.h;
						p->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
							p->m_pWidget->_setSize( geo.size() );
	                
					}
				}
				else
				{
					if( p->m_geo.w != 0 && p->m_geo.h != 0 )
						_requestRender(p->m_geo);
	
					p->m_geo.x = pos.x;
					p->m_geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = 0;
						geo.h = sz.h;
					}
					else
					{
						geo.w = sz.w;
						geo.h = 0;
					}
				}
	
				p = p->_next();
			}
		}
		else
		{
			// Allocate and populate SizeBroker array
	
			int arrayBytes = sizeof(SizeBrokerItem)*m_hooks.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));
	
			int nItems = _populateSizeBrokerArray(pItemArea, givenBreadth);		
			
			// Retrieve length and set geo for all children, call _requestRender() and _setSize() where needed.
			
			m_pSizeBroker->setItemLengths( pItemArea, nItems, givenLength );
			
			PackHook * pH = static_cast<PackHook*>(m_hooks.first());
			SizeBrokerItem * pI = pItemArea;
	
			Coord pos;
			Rect geo;
			while( pH )
			{
				if( pH->isVisible() )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{	
						geo.w = pI->output;
						geo.h = sz.h;
						pos.x += pI->output;
					}
					else
					{	
						geo.w = sz.w;
						geo.h = pI->output;
						pos.y += pI->output;
					}
					geo -= pH->m_padding;
				
					if( geo != pH->m_geo )
					{					
						_requestRender(geo);
						_requestRender(pH->m_geo);
	
						int oldW = pH->m_geo.w;
						int oldH = pH->m_geo.h;
						pH->m_geo = geo;
						if( geo.w != oldW || geo.h != oldH )
							pH->m_pWidget->_setSize( geo.size() );
					}
					pI++;
				}
				else
				{
					if( pH->m_geo.w != 0 && pH->m_geo.h != 0 )
						_requestRender(pH->m_geo);
	
					pH->m_geo.x = pos.x;
					pH->m_geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = 0;
						geo.h = sz.h;
					}
					else
					{
						geo.w = sz.w;
						geo.h = 0;
					}
				}
				pH = pH->_next();
			}
			
			// Release SizeBroker array
			
			Base::memStackRelease(arrayBytes);
		}
	}
	
	//____ _populateSizeBrokerArray() ___________________________________________
	
	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray ) const
	{
		PackHook * pH = static_cast<PackHook*>(m_hooks.first());
		SizeBrokerItem * pI = pArray;
		
		if( m_bHorizontal )
		{
			while( pH )
			{
				if( pH->isVisible() )
				{
					pI->preferred = pH->m_preferredSize.w;
					pI->min = pH->_paddedMinSize().w;
					pI->max = pH->_paddedMaxSize().w;
					pI->weight = pH->m_weight;			
					pI++;
				}
				pH = pH->_next();
			}
		}
		else 
		{
			while( pH )
			{
				if( pH->isVisible() )
				{
					pI->preferred = pH->m_preferredSize.h;
					pI->min = pH->_paddedMinSize().h;
					pI->max = pH->_paddedMaxSize().h;
					pI->weight = pH->m_weight;			
					pI++;
				}
				pH = pH->_next();
			}			
		}
		
		return pI - pArray;
	}
	
	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const
	{
		PackHook * pH = static_cast<PackHook*>(m_hooks.first());
		SizeBrokerItem * pI = pArray;
		
		if( m_bHorizontal )
		{
			while( pH )
			{
				if( pH->isVisible() )
				{
					pI->preferred = pH->_paddedMatchingWidth(forcedBreadth);
					pI->min = pH->_paddedMinSize().w;
					pI->max = pH->_paddedMaxSize().w;
					pI->weight = pH->m_weight;			
					pI++;
				}
				pH = pH->_next();
			}
		}
		else 
		{
			while( pH )
			{
				if( pH->isVisible() )
				{
					pI->preferred = pH->_paddedMatchingHeight(forcedBreadth);
					pI->min = pH->_paddedMinSize().h;
					pI->max = pH->_paddedMaxSize().h;
					pI->weight = pH->m_weight;			
					pI++;
				}
				pH = pH->_next();
			}			
		}
		
		return pI - pArray;
	}

} // namespace wg
