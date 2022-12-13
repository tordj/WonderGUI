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

#include <assert.h>
#include <wg_stackpanel.h>
#include <wg_util.h>
#include <wg_patches.h>

#include <wg_dynamicslotvector.impl.h>
#include <wg_slotextras.impl.h>

namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<StackPanel::Slot>;
	template class PaddedSlotCollectionMethods<StackPanel::Slot,StackPanel::iterator,StackPanel>;
	template class HideableSlotCollectionMethods<StackPanel::Slot, StackPanel::iterator, StackPanel>;

	const TypeInfo StackPanel::TYPEINFO = { "StackPanel", &Panel::TYPEINFO };
	const TypeInfo StackPanel::Slot::TYPEINFO = { "StackPanel::Slot", &DynamicSlot::TYPEINFO };


	//____ constructor ____________________________________________________________

	StackPanel::StackPanel() : slots(this)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	StackPanel::~StackPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StackPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _matchingHeight() _______________________________________________________

	spx StackPanel::_matchingHeight( spx width, int scale ) const
	{
		scale = _fixScale(scale);

		spx height = 0;

		Slot * pSlot = slots._begin();
		Slot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			spx h = pSlot->_paddedMatchingHeight(width,scale);
			if( h > height )
				height = h;
			pSlot++;
		}

		return height;
	}

	//____ matchingWidth() _______________________________________________________

	spx StackPanel::_matchingWidth( spx height, int scale ) const
	{
		scale = _fixScale(scale);

		spx width = 0;

		Slot * pSlot = slots._begin();
		Slot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			spx w = pSlot->_paddedMatchingWidth(height,scale);
			if( w > width )
				width = w;
			pSlot++;
		}

		return width;
	}


	//____ _defaultSize() _____________________________________________________________

	SizeSPX StackPanel::_defaultSize(int scale) const
	{
		scale = _fixScale(scale);

		if (scale == m_scale)
			return m_defaultSize;
		else
			return _calcDefaultSize(scale);
	}

	//____ _resize() ___________________________________________________________

	void StackPanel::_resize( const SizeSPX& size, int scale )
	{
		//TODO: Optimize. If size is same then we only need to update those that have requested resize.

		Panel::_resize(size,scale);
		_updateChildGeo(slots._begin(),slots._end());
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	StackPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
	}

	//____ _firstChild() _______________________________________________________

	Widget * StackPanel::_firstChild() const
	{
		if( slots.isEmpty() )
			return nullptr;

		return slots._first()->_widget();
	}

	//____ _lastChild() ________________________________________________________

	Widget * StackPanel::_lastChild() const
	{
		if( slots.isEmpty() )
			return nullptr;

		return slots._last()->_widget();

	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void StackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( slots.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			Slot * pSlot = slots._first();
			
			while( !pSlot->m_bVisible )
			{
				pSlot++;
				if( pSlot == slots._end() )
				{
					package.pSlot = nullptr;
					return;
				}
			}
				
			package.pSlot = pSlot;
			package.geo = { pSlot->m_position, pSlot->_widget()->_size() };
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void StackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		Slot * pSlot = (Slot*) package.pSlot;

		if( pSlot == slots._last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;

			while( !pSlot->m_bVisible )
			{
				pSlot++;
				if( pSlot == slots._end() )
				{
					package.pSlot = nullptr;
					return;
				}
			}


			package.pSlot = pSlot;
			package.geo = { pSlot->m_position, pSlot->_widget()->_size() };
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void StackPanel::_didAddSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (Slot*) pSlot, ((Slot*)pSlot)+nb );
	}

	//____ _willEraseSlots() ____________________________________________________

	void StackPanel::_willEraseSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (Slot*) pSlot, ((Slot*)pSlot) + nb);
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (Slot*) pSlot, ((Slot*)pSlot) + nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (Slot*) pSlot, ((Slot*)pSlot) + nb);
	}

	//____ _repadSlots() _________________________________________________________

	void StackPanel::_repadSlots( StaticSlot * _pSlot, int nb, Border padding )
	{
		Slot* pSlot = static_cast<Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
			pSlot[i].m_padding	= padding;

		_updateChildGeo(pSlot,pSlot+nb);
		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeSPX newDefault =_calcDefaultSize(m_scale);
		if (newDefault != m_defaultSize || m_defaultSize != m_size)
			_requestResize();
	}

	void StackPanel::_repadSlots(StaticSlot * _pSlot, int nb, const Border * pPaddings)
	{
		Slot* pSlot = static_cast<Slot*>(_pSlot);

		for (int i = 0; i < nb; i++)
			pSlot[i].m_padding	= *pPaddings++;

		_updateChildGeo(pSlot,pSlot+nb);
		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeSPX newDefault = _calcDefaultSize(m_scale);
		if (newDefault != m_defaultSize || m_defaultSize != m_size)
			_requestResize();
	}

	//____ _setSizePolicy() ___________________________________________________

	void StackPanel::_setSizePolicy(Slot * pSlot, SizePolicy2D policy)
	{
		if (policy != pSlot->m_sizePolicy)
		{
			RectSPX oldGeo = { pSlot->m_position, pSlot->_widget()->_size() };
			pSlot->m_sizePolicy = policy;
			RectSPX newGeo = _childGeo(pSlot);

			if (newGeo != oldGeo)
			{
				_requestRender(oldGeo);
				_requestRender(newGeo);
				pSlot->_setSize(newGeo.size());
				pSlot->m_position = newGeo.pos();
			}
		};
	}

	//____ _setPlacement() ________________________________________________________

	void StackPanel::_setPlacement(Slot * pSlot, Placement placement)
	{
		if (placement != pSlot->m_placement)
		{
			RectSPX oldGeo = { pSlot->m_position, pSlot->_widget()->_size() };
			pSlot->m_placement = placement;
			RectSPX newGeo = _childGeo(pSlot);

			if (newGeo != oldGeo)
			{
				_requestRender(oldGeo);
				_requestRender(newGeo);
				pSlot->_setSize(newGeo.size());
				pSlot->m_position = newGeo.pos();
			}
		};
	}

	//____ _didMoveSlots() ________________________________________________________

	void StackPanel::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childPos() ________________________________________________________

	CoordSPX StackPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return _childGeo((const Slot *)pSlot).pos();
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		_childRequestRender( pSlot, { pSlot->m_position, pSlot->_widget()->_size() } );
	}

	void StackPanel::_childRequestRender( StaticSlot * _pSlot, const RectSPX& _rect )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		if( !pSlot->m_bVisible )
			return;

		// Put our rectangle into patches

		RectSPX rect = _rect + pSlot->m_position;

		PatchesSPX patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = slots._begin() ; pCover < pSlot ; pCover++ )
		{
			RectSPX geo = { pCover->m_position, pCover->_widget()->_size() };
			if( pCover->m_bVisible && geo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, geo, RectSPX(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectSPX * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _childRequestResize() ______________________________________________

	void StackPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		SizeSPX newDefault = _calcDefaultSize(m_scale);

		if(newDefault != m_defaultSize || m_defaultSize != m_size )
		{
			m_defaultSize = newDefault;
			_requestResize();
		}
		else
		{
			RectSPX oldGeo = { pSlot->m_position, pSlot->_widget()->_size() };
			RectSPX newGeo = _childGeo(pSlot);

			if (oldGeo != newGeo)
			{
				pSlot->m_position = newGeo.pos();
				pSlot->_setSize(newGeo.size());
				_requestRender(oldGeo);
				_requestRender(newGeo);
			}
		}

	}

	//____ _prevChild() __________________________________________________________

	Widget * StackPanel::_prevChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const Slot *>(_pSlot);

		if( pSlot > slots._begin() )
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * StackPanel::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const Slot *>(_pSlot);

		if( pSlot < slots._last() )
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void StackPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<Slot*>(pSlot));
	}

	//____ _replaceChild() ____________________________________________________

	void StackPanel::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
	{
		static_cast<Slot*>(pSlot)->_setWidget(pNewChild);
		_childRequestRender(pSlot);
		_childRequestResize(pSlot);
	}


	//____ _unhideChildren() _____________________________________________________

	void StackPanel::_unhideChildren( Slot * pSlot, Slot * pEnd )
	{
		bool	bRequestResize = false;

		while( pSlot != pEnd )
		{
			// Update m_defaultSize

			if( !pSlot->m_bVisible )
			{
				SizeSPX defaultSize = pSlot->_paddedDefaultSize(m_scale);

				if(defaultSize.w > m_defaultSize.w )
				{
					m_defaultSize.w = defaultSize.w;
					bRequestResize = true;
				}
				if(defaultSize.h > m_defaultSize.h )
				{
					m_defaultSize.h = defaultSize.h;
					bRequestResize = true;
				}

				// Make unhidden Widgets visible, adapt them to our size and scale and force a render.

				pSlot->m_bVisible = true;
				
				RectSPX geo = _childGeo(pSlot);
				pSlot->m_position = geo.pos();
				pSlot->_setSize(geo.size(),m_scale);
				_childRequestRender(pSlot);
			}
			pSlot++;
		}

		if( bRequestResize )
			_requestResize();
	}

	//____ _hideChildren() __________________________________________________

	void StackPanel::_hideChildren( Slot * pSlot, Slot * pEnd )
	{
		bool	bRequestResize = false;

		// Get dirty rectangles for all visible sections of widgets to be removed.

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
			{
				_childRequestRender( pSlot );
				pSlot->m_bVisible = false;
			}
			pSlot++;
		}
		// Update m_defaultSize

		SizeSPX	defaultSize;
		Slot * p = slots._begin();

		while( p != slots._end() )
		{
			if( p->m_bVisible )
			{
				SizeSPX sz = p->_paddedDefaultSize(m_scale);
				if( sz.w > defaultSize.w )
					defaultSize.w = sz.w;
				if( sz.h > defaultSize.h )
					defaultSize.h = sz.h;
			}
			p++;
		}

		if( defaultSize != m_defaultSize )
			bRequestResize = true;

		m_defaultSize = defaultSize;

		// Check if removal might affect height for current width
/*
		int height = pToBeRemoved->_widget()->matchingHeight(m_size.w);
		if( height >= m_size.h )
			bRequestResize = true;
*/
		//

		if( bRequestResize )
			_requestResize();
	}


	//____ _refreshAllWidgets() ____________________________________________________
/*
	void StackPanel::_refreshAllWidgets()
	{
		_refreshDefaultSize();
		_adaptChildrenToSize();
		_requestRender();
	}
*/

	//____ _calcDefaultSize() _____________________________________________________

	SizeSPX StackPanel::_calcDefaultSize(int scale) const
	{
		SizeSPX	defaultSize;

		auto * pSlot = slots._begin();
		auto * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
			{
				SizeSPX sz = pSlot->_paddedDefaultSize(scale);
				if( sz.w > defaultSize.w )
					defaultSize.w = sz.w;
				if( sz.h > defaultSize.h )
					defaultSize.h = sz.h;
			}
			pSlot++;
		}

		return defaultSize;
	}

	//____ _updateChildGeo() ___________________________________________________________

	void StackPanel::_updateChildGeo(Slot* pSlot, Slot* pEnd)
	{
		while( pSlot != pEnd )
		{
			if (pSlot->m_bVisible)
			{
				RectSPX geo = _childGeo(pSlot);
				pSlot->m_position = geo.pos();
				pSlot->_setSize( geo.size(),m_scale );
			}
			pSlot++;
		}
	}

	//____ _childGeo() ___________________________________________________________

	RectSPX StackPanel::_childGeo( const Slot * pSlot ) const
	{
		RectSPX base = RectSPX( m_size ) - align(ptsToSpx(pSlot->m_padding,m_scale));

		if( base.w <= 0 || base.h <= 0 )
			return RectSPX();

		switch( pSlot->m_sizePolicy )
		{
			default:
		case SizePolicy2D::Original:
			{
				SizeSPX	size = pSlot->_widget()->_defaultSize(m_scale);
				RectSPX geo = Util::placementToRect( pSlot->m_placement, base, size );

				if( geo.w > base.w )
				{
					geo.x = 0;
					geo.w = base.w;
				}

				if( geo.h > base.h )
				{
					geo.y = 0;
					geo.h = base.h;
				}
				return geo;
			}
			case SizePolicy2D::Stretch:
			{
				return base;
			}
			case SizePolicy2D::Scale:
			{
				SizeSPX	orgSize = pSlot->_widget()->_defaultSize(m_scale);
				SizeSPX	size;

				float	fracX = orgSize.w / (float) base.w;
				float	fracY = orgSize.h / (float) base.h;

				if( fracX > fracY )
				{
					size.w = base.w;
					size.h = int (orgSize.h / fracX);
				}
				else
				{
					size.h = base.h;
					size.w = (int) (orgSize.w / fracY);
				}

				return Util::placementToRect( pSlot->m_placement, base, size );
			}
		}
	}




} // namespace wg
