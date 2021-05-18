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
#include <wg_internal.h>

namespace wg
{

	const TypeInfo Layer::TYPEINFO = { "Layer", &Container::TYPEINFO };
	const TypeInfo Layer::Slot::TYPEINFO = { "Layer::Slot", &StaticSlot::TYPEINFO };


	//____ constructor ____________________________________________________________

	Layer::Layer() : mainSlot(this)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Layer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _matchingHeight() _______________________________________________________

	spx Layer::_matchingHeight( spx width, int scale ) const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->_matchingHeight( width, scale );
		else
			return Widget::_matchingHeight(width, scale);
	}

	//____ _matchingWidth() _______________________________________________________

	spx Layer::_matchingWidth( spx height, int scale ) const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->_matchingWidth( height, scale );
		else
			return Widget::_matchingWidth(height, scale);
	}

	//____ _preferredSize() _____________________________________________________________

	SizeSPX Layer::_preferredSize(int scale) const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->_preferredSize(scale);
		else
			return SizeSPX(64,64);
	}


	//____ _onRequestRender() _____________________________________________________

	void Layer::_onRequestRender( const RectSPX& rect, const Slot * pSlot )
	{
		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectSPX( rect, RectSPX(0,0,m_size)) );

		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.

		const Slot * pCover = _beginLayerSlots();
		const Slot * pEnd = pSlot ? pSlot : _endLayerSlots();

		int incNext = _sizeOfLayerSlot();

		while( pCover <  pEnd )
		{
			if( pCover->m_geo.intersectsWith( rect ) )
				OO(pCover->_widget())->_maskPatches( patches, pCover->m_geo, RectSPX(0,0, m_size ), _getBlendMode() );

			pCover = _incLayerSlot(pCover,incNext);
		}

		// Make request render calls

		for( const RectSPX * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}


	//____ _firstChild() ___________________________________________________________

	Widget* Layer::_firstChild() const
	{
		const Slot * p = _beginLayerSlots();
		if (p != _endLayerSlots())
			return p->_widget();

		return mainSlot._widget();
	}

	//____ _lastChild() ____________________________________________________________

	Widget* Layer::_lastChild() const
	{
		if (mainSlot._widget())
			return mainSlot._widget();
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
		else if (mainSlot._widget())
		{
			package.geo = m_size;
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
		else if (mainSlot._widget())
		{
			package.geo = m_size;
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
			OO(mainSlot)._setWidget(nullptr);
			_onRequestRender(RectSPX(0, 0, m_size), 0);
			_requestResize();
		}
	}

	//____ _replaceChild() _______________________________________________________

	void Layer::_replaceChild( StaticSlot * pSlot, Widget * pNewWidget )
	{
		if (pSlot == &mainSlot)
		{
			OO(mainSlot)._setWidget(pNewWidget);
			if( pNewWidget )
				OO(pNewWidget)->_resize(m_size);			//TODO: Should be content size here (and in all other _setWidget() methods?)
			_onRequestRender(RectSPX(0, 0, m_size), 0);
			_requestResize();
		}
	}

	//____ _resize() _______________________________________________________

	void Layer::_resize(const SizeSPX& size, int scale)
	{
		Container::_resize(size, scale);

		if ( mainSlot._widget())
			OO(mainSlot._widget())->_resize(size, scale);

		//TODO: Update size and scale of other children
	}


	//____ _childPos() _________________________________________________________

	CoordSPX Layer::_childPos( const StaticSlot * pSlot ) const
	{
		if( pSlot == &mainSlot )
			return CoordSPX();

		return ((Slot*)pSlot)->m_geo;
	}

	//____ _childRequestRender() _______________________________________________

	void Layer::_childRequestRender( StaticSlot * _pSlot )
	{
		if( _pSlot == &mainSlot )
			_onRequestRender( RectSPX( 0,0, m_size ), 0 );		//TODO: Take padding into account
		else
		{
			Slot * pSlot = reinterpret_cast<Slot*>(_pSlot);
			_onRequestRender( pSlot->m_geo, pSlot );
		}
	}

	void Layer::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
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

		return mainSlot._widget();
	}


} // namespace wg
