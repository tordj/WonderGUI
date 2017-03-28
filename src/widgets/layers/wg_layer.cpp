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

#include <wg_layer.h>
#include <wg_patches.h>

namespace wg 
{
	
	const char Layer::CLASSNAME[] = {"Layer"};
	
	
	//____ Constructor ____________________________________________________________
	
	Layer::Layer() : base( &m_baseSlot, this )
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Layer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Container::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Layer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Layer_p Layer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Layer_p( static_cast<Layer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	
	//____ matchingHeight() _______________________________________________________
	
	int Layer::matchingHeight( int width ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int Layer::matchingWidth( int height ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size Layer::preferredSize() const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->preferredSize();
		else
			return Size(1,1);
	}
	
	
	//____ _onRequestRender() _____________________________________________________
	
	void Layer::_onRequestRender( const Rect& rect, const LayerSlot * pSlot )
	{
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,m_size)) );
	
		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.
	
		const LayerSlot * pCover;
		const LayerSlot * pEnd = _endLayerSlots();
	
		int incNext = _sizeOfLayerSlot();
	
		if( pSlot )
			pCover = _incLayerSlot(pSlot,incNext);
		else
			pCover = _beginLayerSlots();
	
		while( pCover <  pEnd )
		{
			if( pCover->geo.intersectsWith( rect ) )
				pCover->pWidget->_maskPatches( patches, pCover->geo, Rect(0,0,INT_MAX,INT_MAX ), _getBlendMode() );
	
			pCover = _incLayerSlot(pSlot,incNext);
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	
	//____ _firstChild() ___________________________________________________________
	
	Widget* Layer::_firstChild() const
	{		
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget;
		else
		{
			LayerSlot * p = _beginLayerSlots();
			if( p != _endLayerSlots() )
				return p->pWidget;
	
			return nullptr;
		}	
	}
	
	//____ _lastChild() ____________________________________________________________
	
	Widget* Layer::_lastChild() const
	{
		LayerSlot * pSlot = _endLayerSlots();
	
		if( pSlot == _beginLayerSlots() )
			return m_baseSlot.pWidget;

		pSlot = _decLayerSlot(pSlot,_sizeOfLayerSlot());
		return pSlot->pWidget;
	}
	
	//____ _firstChildWithGeo() _____________________________________________________
	
	void Layer::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		if( m_baseSlot.pWidget )
		{
			package.geo = Rect(0,0,m_size);
			package.pWidget = m_baseSlot.pWidget;
			package.pMagic = (void *) &m_baseSlot;
		}
		else
		{
			LayerSlot * p = _beginLayerSlots();
			if( p < _endLayerSlots() )
			{
				package.geo = p->geo;
				package.pWidget = p->pWidget;
				package.pMagic = p;
			}
			else
				package.pWidget = nullptr;
		}
	}
	
	//____ _nextChildWithGeo() _______________________________________________________
	
	void Layer::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		LayerSlot * p = (LayerSlot*) package.pMagic;

		if( p == &m_baseSlot )
			p = _beginLayerSlots();
		else
			p = _incLayerSlot(p,_sizeOfLayerSlot());

		if( p < _endLayerSlots() )
		{
			package.geo = ((LayerSlot*)p)->geo;
			package.pWidget = p->pWidget;
			package.pMagic = p;
		}
		else
			package.pWidget = nullptr;
	}

	//____ _cloneContent() _______________________________________________________
	
	void Layer::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}
	
	//____ _setWidget() _______________________________________________________
	
	void Layer::_setWidget( Slot * pSlot, Widget * pNewWidget )
	{
		_onRequestRender( Rect(0,0,m_size), 0 );
		_requestResize();
	}

	//____ _setSize() _______________________________________________________

	void Layer::_setSize(const Size& size)
	{
		Container::_setSize(size);

		if (m_baseSlot.pWidget)
			m_baseSlot.pWidget->_setSize(size);
	}


	//____ _childPos() _________________________________________________________
	
	Coord Layer::_childPos( void * pChildRef ) const
	{
		if( pChildRef == &m_baseSlot )
			return {0,0};
		
		return ((LayerSlot*)pChildRef)->geo;
	}

	//____ _childSize() ________________________________________________________

	Size Layer::_childSize( void * pChildRef ) const
	{
		if( pChildRef == &m_baseSlot )
			return m_size;
		
		return ((LayerSlot*)pChildRef)->geo;
	}

	//____ _childRequestRender() _______________________________________________

	void Layer::_childRequestRender( void * pChildRef )
	{
		if( pChildRef == &m_baseSlot )
			_onRequestRender( Rect( 0,0, m_size ), 0 );		//TODO: Take padding into account
		else
		{
			LayerSlot * pSlot = reinterpret_cast<LayerSlot*>(pChildRef);
			_onRequestRender( pSlot->geo, pSlot );
		}
	}
	
	void Layer::_childRequestRender( void * pChildRef, const Rect& rect )
	{
		if( pChildRef == &m_baseSlot )
			_onRequestRender( rect, 0 );		//TODO: Take padding into account
		else
		{
			LayerSlot * pSlot = reinterpret_cast<LayerSlot*>(pChildRef);
			_onRequestRender( rect + pSlot->geo.pos(), pSlot );
		}
	}

	//____ _childRequestResize() _______________________________________________
 
	void Layer::_childRequestResize( void * pChildRef )
	{
		_requestResize();			//TODO: Smarter handling, not request resize unless we need to.
	}

	//____ _prevChild() ________________________________________________________

	Widget * Layer::_prevChild( void * pChildRef ) const
	{
		if( pChildRef == &m_baseSlot )
			return nullptr;
		
		if( pChildRef == _beginLayerSlots() )
			return m_baseSlot.pWidget;
			
		LayerSlot * p = _decLayerSlot((LayerSlot*)pChildRef,_sizeOfLayerSlot());
		return p->pWidget;
	}

	//____ _nextChild() ________________________________________________________

	Widget * Layer::_nextChild( void * pChildRef ) const
	{
		if( pChildRef == &m_baseSlot )
		{
			if( _beginLayerSlots() < _endLayerSlots() )
				return _beginLayerSlots()->pWidget;
		}
		else
		{
			LayerSlot * p = _incLayerSlot((LayerSlot*)pChildRef,_sizeOfLayerSlot());
			if( p < _endLayerSlots() )
				return p->pWidget;
		}

		return nullptr;
	}
	

} // namespace wg
