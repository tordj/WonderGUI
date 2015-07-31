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
	const char LayerHook::CLASSNAME[] = {"LayerHook"};
	
	
	//____ Constructor ____________________________________________________________
	
	Layer::Layer()
	{
		m_baseHook.m_pParent = this;
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
	
	
	//____ setBaseWidget() _________________________________________________________
	
	Hook_p Layer::setBaseWidget( const Widget_p& pWidget )
	{
		m_baseHook._setWidget(pWidget.rawPtr());
		_onBaseChanged();
		return &m_baseHook;
	}
	
	//____ baseWidget() ____________________________________________________________
	
	Widget_p Layer::baseWidget()
	{
		return m_baseHook._widget();
	}
	
	
	//____ removeBaseWidget() _____________________________________________________
	
	bool Layer::removeBaseWidget()
	{
		if( !m_baseHook._widget() )
			return false;
	
		m_baseHook._setWidget(0);
		_onBaseChanged();
	
		return true;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int Layer::matchingHeight( int width ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int Layer::matchingWidth( int height ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size Layer::preferredSize() const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->preferredSize();
		else
			return Size(1,1);
	}
	
	
	//____ _onRequestRender() _____________________________________________________
	
	void Layer::_onRequestRender( const Rect& rect, const LayerHook * pHook )
	{
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,m_size)) );
	
		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.
	
		LayerHook * pCover;
	
		if( pHook )
			pCover = pHook->_nextLayerHook();
		else
			pCover = _firstLayerHook();
	
		while( pCover )
		{
			if( pCover->m_geo.intersectsWith( rect ) )
				pCover->_widget()->_onMaskPatches( patches, pCover->m_geo, Rect(0,0,65536,65536 ), _getBlendMode() );
	
			pCover = pCover->_nextLayerHook();
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	
	//____ _firstHook() ___________________________________________________________
	
	Hook* Layer::_firstHook() const
	{
		if( m_baseHook._widget() )
			return const_cast<_BaseHook*>(&m_baseHook);
		else
			return _firstLayerHook();
	}
	
	//____ _lastHook() ____________________________________________________________
	
	Hook* Layer::_lastHook() const
	{
		Hook * p = _lastLayerHook();
	
		if( !p )
		{
			if( m_baseHook._widget() )
				return const_cast<_BaseHook*>(&m_baseHook);	
			return 0;
		}
	
		return p;
	}
	
	//____ _firstHookWithGeo() _____________________________________________________
	
	Hook * Layer::_firstHookWithGeo( Rect& geo ) const
	{
		if( m_baseHook._widget() )
		{
			geo = Rect(0,0,m_size);
			return const_cast<_BaseHook*>(&m_baseHook);
		}
		else
		{
			LayerHook * p = _firstLayerHook();
			if( p )
				geo = p->m_geo;
	
			return p;
		}
	}
	
	//____ _nextHookWithGeo() _______________________________________________________
	
	Hook * Layer::_nextHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		Hook * p = pHook->_nextHook();
		if( p )
			geo = ((LayerHook*)p)->m_geo;
	
		return p;
	}
	
	
	//____ _lastHookWithGeo() _____________________________________________________
	
	Hook * Layer::_lastHookWithGeo( Rect& geo ) const
	{
		LayerHook * p = _lastLayerHook();
		if( p )
		{
			geo = p->m_geo;
			return p;
		}
		else if( m_baseHook._widget() )
		{
			geo = Rect(0,0,m_size);
			return const_cast<_BaseHook*>(&m_baseHook);
		}
		else
			return 0;
	}
	
	//____ _prevHookWithGeo() _______________________________________________________
	
	Hook * Layer::_prevHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		Hook * p = pHook->_prevHook();
		if( p )
			geo = p->geo();
	
		return p;
	}
	
	
	//____ _onBaseChanged() _______________________________________________________
	
	void Layer::_onBaseChanged()
	{
		_onRequestRender( Rect(0,0,m_size), 0 );
		_requestResize();
	}
	
	//_____________________________________________________________________________
	void Layer::_BaseHook::_requestRender()
	{
		m_pParent->_onRequestRender( Rect( 0,0, m_pParent->m_size ), 0 );
	}
	
	//_____________________________________________________________________________
	void Layer::_BaseHook::_requestRender( const Rect& rect )
	{
		m_pParent->_onRequestRender( rect, 0 );
	}
	
	//_____________________________________________________________________________
	void Layer::_BaseHook::_requestResize()
	{
		m_pParent->_requestResize();					// Just forward to our parent
	}
	
	
	//____ LayerHook::isInstanceOf() __________________________________________
	
	bool LayerHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Hook::isInstanceOf(pClassName);
	}
	
	//____ LayerHook::className() _____________________________________________
	
	const char * LayerHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ LayerHook::cast() __________________________________________________
	
	LayerHook_p LayerHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return LayerHook_p( static_cast<LayerHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//_____________________________________________________________________________
	Coord LayerHook::globalPos() const
	{
		return parent()->globalPos() + m_geo.pos();
	}
	
	//_____________________________________________________________________________
	Rect LayerHook::globalGeo() const
	{
		return m_geo + parent()->globalPos();
	}
	
	//_____________________________________________________________________________
	void LayerHook::_requestRender()
	{
		parent()->_onRequestRender( m_geo, this );
	}
	
	//_____________________________________________________________________________
	void LayerHook::_requestRender( const Rect& rect )
	{
		parent()->_onRequestRender( rect + m_geo.pos(), this );
	}
	
	//_____________________________________________________________________________
	Hook * LayerHook::_prevHook() const
	{
		Hook * p = _prevLayerHook();
		if( !p )
		{
			Container * c = _parent();
			if( c != 0 )
			{
				Layer * l = static_cast<Layer*>(c);
				if( l->m_baseHook._widget() )	
					p = &l->m_baseHook;
			}
		}
		return p;
	}
	
	//_____________________________________________________________________________
	Hook * LayerHook::_nextHook() const
	{
		return _nextLayerHook();
	}
	
	
	//_____________________________________________________________________________
	Layer_p LayerHook::parent() const
	{ 
		return static_cast<Layer*>(_parent()); 
	}

} // namespace wg
