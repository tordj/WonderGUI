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

	Layer::Layer() : mainSlot(this)
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

	Layer_p Layer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Layer_p( static_cast<Layer*>(pObject) );

		return 0;
	}



	//____ _matchingHeight() _______________________________________________________

	int Layer::_matchingHeight( int width ) const
	{
		if( _mainSlot()->_widget() )
			return _mainSlot()->_matchingHeight( width );
		else
			return Widget::_matchingHeight(width);
	}

	//____ _matchingWidth() _______________________________________________________

	int Layer::_matchingWidth( int height ) const
	{
		if( _mainSlot()->_widget() )
			return _mainSlot()->_matchingWidth( height );
		else
			return Widget::_matchingWidth(height);
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI Layer::_preferredSize() const
	{
		if( _mainSlot()->_widget() )
			return _mainSlot()->_preferredSize();
		else
			return SizeI(1,1);
	}


	//____ _onRequestRender() _____________________________________________________

	void Layer::_onRequestRender( const RectI& rect, const Slot * pSlot )
	{
		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectI( rect, RectI(0,0,m_size)) );

		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.

		const Slot * pCover = _beginLayerSlots();
		const Slot * pEnd = pSlot ? pSlot : _endLayerSlots();

		int incNext = _sizeOfLayerSlot();

		while( pCover <  pEnd )
		{
			if( pCover->m_geo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->m_geo, RectI(0,0,INT_MAX,INT_MAX ), _getBlendMode() );

			pCover = _incLayerSlot(pCover,incNext);
		}

		// Make request render calls

		for( const RectI * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}


	//____ _firstChild() ___________________________________________________________

	Widget* Layer::_firstChild() const
	{
		const Slot * p = _beginLayerSlots();
		if (p != _endLayerSlots())
			return p->_widget();

		return _mainSlot()->_widget();
	}

	//____ _lastChild() ____________________________________________________________

	Widget* Layer::_lastChild() const
	{
		if (_mainSlot()->_widget())
			return _mainSlot()->_widget();
		else
		{
			const Slot * pSlot = _endLayerSlots();
			if (pSlot > _beginLayerSlots())
			{
				pSlot = _decLayerSlot(pSlot, _sizeOfLayerSlot());
				return pSlot->_widget();
			}

			return nullptr;
		}
	}

	//____ _firstSlotWithGeo() _____________________________________________________

	void Layer::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		const Slot * p = _beginLayerSlots();
		if( p < _endLayerSlots() )
		{
			package.geo = p->m_geo;
			package.pSlot = p;
		}
		else if (_mainSlot()->_widget())
		{
			package.geo = RectI(0, 0, m_size);
			package.pSlot = &mainSlot;
		}
		else
			package.pSlot = nullptr;
	}

	//____ _nextSlotWithGeo() _______________________________________________________

	void Layer::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		const Slot * p = (Slot*) package.pSlot;

		if (p == (StaticSlot*) &mainSlot)
		{
			package.pSlot = nullptr;
			return;
		}

		p = _incLayerSlot(p,_sizeOfLayerSlot());
		if( p < _endLayerSlots() )
		{
			package.geo = ((Slot*)p)->m_geo;
			package.pSlot = p;
		}
		else if (_mainSlot()->_widget())
		{
			package.geo = RectI(0, 0, m_size);
			package.pSlot = &mainSlot;
		}
		else
			package.pSlot = nullptr;
	}

	//____ _cloneContent() _______________________________________________________

	void Layer::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}

	//____ _releaseChild() ____________________________________________________

	void Layer::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot)
		{
			_mainSlot()->_setWidget(nullptr);
			_onRequestRender(RectI(0, 0, m_size), 0);
			_requestResize();
		}
	}

	//____ _replaceChild() _______________________________________________________

	void Layer::_replaceChild( StaticSlot * pSlot, Widget * pNewWidget )
	{
		if (pSlot == &mainSlot)
		{
			_mainSlot()->_setWidget(pNewWidget);
			if( pNewWidget )
				pNewWidget->_resize(m_size);			//TODO: Should be content size here (and in all other _setWidget() methods?)
			_onRequestRender(RectI(0, 0, m_size), 0);
			_requestResize();
		}
	}

	//____ _resize() _______________________________________________________

	void Layer::_resize(const SizeI& size)
	{
		Container::_resize(size);

		if (_mainSlot()->_widget())
			_mainSlot()->_widget()->_resize(size);
	}


	//____ _childPos() _________________________________________________________

	CoordI Layer::_childPos( const StaticSlot * pSlot ) const
	{
		if( pSlot == &mainSlot )
			return {0,0};

		return ((Slot*)pSlot)->m_geo;
	}

	//____ _childRequestRender() _______________________________________________

	void Layer::_childRequestRender( StaticSlot * _pSlot )
	{
		if( _pSlot == &mainSlot )
			_onRequestRender( RectI( 0,0, m_size ), 0 );		//TODO: Take padding into account
		else
		{
			Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);
			_onRequestRender( pSlot->m_geo, pSlot );
		}
	}

	void Layer::_childRequestRender( StaticSlot * _pSlot, const RectI& rect )
	{
		if( _pSlot == &mainSlot )
			_onRequestRender( rect, 0 );		//TODO: Take padding into account
		else
		{
			Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);
			_onRequestRender( rect + pSlot->m_geo.pos(), pSlot );
		}
	}
/*
	//____ _childRequestResize() _______________________________________________

	void Layer::_childRequestResize( StaticSlot * pSlot )
	{
		_requestResize();			//TODO: Smarter handling, not request resize unless we need to.
	}
*/
	//____ _prevChild() ________________________________________________________

	Widget * Layer::_prevChild( const StaticSlot * pSlot ) const
	{
		if( pSlot == &mainSlot )
			pSlot = _endLayerSlots();

		if (pSlot == _beginLayerSlots())
			return nullptr;

		Slot * p = _decLayerSlot((Slot*)pSlot,_sizeOfLayerSlot());
		return p->_widget();
	}

	//____ _nextChild() ________________________________________________________

	Widget * Layer::_nextChild( const StaticSlot * pSlot ) const
	{
		if (pSlot == &mainSlot)
			return nullptr;

		Slot * p = _incLayerSlot((Slot*)pSlot, _sizeOfLayerSlot());
		if (p < _endLayerSlots())
			return p->_widget();

		return _mainSlot()->_widget();
	}


} // namespace wg
