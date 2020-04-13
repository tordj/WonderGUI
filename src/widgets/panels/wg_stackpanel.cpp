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
#include <wg_internal.h>

#include <wg_cpaddedslotvector.impl.h>

namespace wg
{
	template class CDynamicSlotVector<StackPanel::Slot>;
	template class CPaddedSlotVector<StackPanel::Slot>;

	const TypeInfo StackPanel::TYPEINFO = { "StackPanel", &Panel::TYPEINFO };
	const TypeInfo StackPanel::Slot::TYPEINFO = { "StackPanel::Slot", &PaddedSlot::TYPEINFO };


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

	//____ matchingHeight() _______________________________________________________

	MU StackPanel::matchingHeight( MU width ) const
	{
		MU height = 0;

		Slot * pSlot = slots._begin();
		Slot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			MU h = pSlot->_widget()->matchingHeight(width);
			if( h > height )
				height = h;
			pSlot++;
		}

		return height;
	}

	//____ matchingWidth() _______________________________________________________

	MU StackPanel::matchingWidth( MU height ) const
	{
		MU width = 0;

		Slot * pSlot = slots._begin();
		Slot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			MU w = pSlot->_widget()->matchingWidth(height);
			if( w > width )
				width = w;
			pSlot++;
		}

		return width;
	}


	//____ preferredSize() _____________________________________________________________

	Size StackPanel::preferredSize() const
	{
		return m_preferredSize;
	}

	//____ _cloneContent() ______________________________________________________

	void StackPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement
	}

	//____ _resize() ___________________________________________________________

	void StackPanel::_resize( const Size& size )
	{
		//TODO: Optimize. If size is same then we only need to update those that have requested resize.

		Panel::_resize(size);
		_adaptChildrenToSize();
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
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);
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
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void StackPanel::_didAddSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (Slot*) pSlot, nb );
	}

	//____ _willEraseSlots() ____________________________________________________

	void StackPanel::_willEraseSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (Slot*) pSlot, nb );
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (Slot*) pSlot, nb );
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (Slot*) pSlot, nb );
	}

	//____ _repadSlots() _________________________________________________________

	void StackPanel::_repadSlots( StaticSlot * pSlot, int nb, Border padding )
	{
		for( int i = 0 ; i < nb ; i++ )
			((Slot*)pSlot)[i].m_padding = padding;

		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		Size newPreferred =_calcPreferredSize();
		if (newPreferred != m_preferredSize || m_preferredSize != m_size)
			_requestResize();
	}

	void StackPanel::_repadSlots(StaticSlot * pSlot, int nb, const Border * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((Slot*)pSlot)[i].m_padding = * pPaddings++;

		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		Size newPreferred = _calcPreferredSize();
		if (newPreferred != m_preferredSize || m_preferredSize != m_size)
			_requestResize();
	}

	//____ _setSizePolicy() ___________________________________________________

	void StackPanel::_setSizePolicy(Slot * pSlot, SizePolicy2D policy)
	{
		if (policy != pSlot->m_sizePolicy)
		{
			Rect oldGeo = _childGeo(pSlot);
			pSlot->m_sizePolicy = policy;
			Rect newGeo = _childGeo(pSlot);

			if (newGeo.size() != oldGeo.size())
			{
				_requestRender(oldGeo);
				_requestRender(newGeo);
				pSlot->_setSize(newGeo.size());
			}
		};
	}

	//____ _setOrigo() ________________________________________________________

	void StackPanel::_setOrigo(Slot * pSlot, Origo origo)
	{
		if (origo != pSlot->m_origo)
		{
			_childRequestRender(pSlot);
			pSlot->m_origo = origo;
			_childRequestRender(pSlot);
		};
	}

	//____ _didMoveSlots() ________________________________________________________

	void StackPanel::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childPos() ________________________________________________________

	Coord StackPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return _childGeo((const Slot *)pSlot).pos();
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( StaticSlot * pSlot )
	{
		_childRequestRender( pSlot, _childGeo((Slot*) pSlot) );
	}

	void StackPanel::_childRequestRender( StaticSlot * _pSlot, const Rect& _rect )
	{
		Slot * pSlot = (Slot *) _pSlot;

		if( !pSlot->m_bVisible )
			return;

		// Put our rectangle into patches

		Rect rect = _rect + _childPos(pSlot);


		Patches patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = slots._begin() ; pCover < pSlot ; pCover++ )
		{
			Rect geo = _childGeo(pCover);
			if( pCover->m_bVisible && geo.intersectsWith( rect ) )
				OO(pCover->_widget())->_maskPatches( patches, geo, Rect(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _childRequestResize() ______________________________________________

	void StackPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<Slot*>(_pSlot);

		Size newPreferred = _calcPreferredSize();

		if( newPreferred != m_preferredSize || m_preferredSize != m_size )
		{
			m_preferredSize = newPreferred;
			_requestResize();
		}
		else
		{
			pSlot->_setSize(_childGeo((Slot*)pSlot).size());
			_requestRender();
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
		_childRequestResize(pSlot);
	}


	//____ _unhideChildren() _____________________________________________________

	void StackPanel::_unhideChildren( Slot * pSlot, int nb )
	{
		bool	bRequestResize = false;

		// Update m_preferredSize

		for( int i = 0 ; i < nb ; i++ )
		{
			if( !pSlot[i].m_bVisible )
			{
				Size preferred = pSlot[i]._paddedPreferredSize();

				if( preferred.w > m_preferredSize.w )
				{
					m_preferredSize.w = preferred.w;
					bRequestResize = true;
				}
				if( preferred.h > m_preferredSize.h )
				{
					m_preferredSize.h = preferred.h;
					bRequestResize = true;
				}
			}
		}

		if( bRequestResize )
			_requestResize();

		// Make unhidden Widgets visible, adapt them to our size and force a render.

		for( int i = 0 ; i < nb ; i++ )
		{
			if( !pSlot[i].m_bVisible )
			{
				pSlot[i].m_bVisible = true;
				OO(pSlot[i]._widget())->_resize(_childGeo(pSlot).size() );
				_childRequestRender( pSlot + i );
			}
		}
	}

	//____ _hideChildren() __________________________________________________

	void StackPanel::_hideChildren( Slot * pRemove, int nb )
	{
		bool	bRequestResize = false;

		// Get dirty rectangles for all visible sections of widgets to be removed.

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pRemove[i].m_bVisible )
			{
				_childRequestRender( pRemove + i );
				pRemove[i].m_bVisible = false;
			}
		}
		// Update m_preferredSize

		Size	preferredSize;
		Slot * p = slots._begin();

		while( p != slots._end() )
		{
			if( p->m_bVisible )
			{
				Size sz = p->_paddedPreferredSize();
				if( sz.w > preferredSize.w )
					preferredSize.w = sz.w;
				if( sz.h > preferredSize.h )
					preferredSize.h = sz.h;
				p++;
			}
		}

		if( preferredSize != m_preferredSize )
			bRequestResize = true;

		m_preferredSize = preferredSize;

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
		_refreshPreferredSize();
		_adaptChildrenToSize();
		_requestRender();
	}
*/

	//____ _calcPreferredSize() _____________________________________________________

	Size StackPanel::_calcPreferredSize()
	{
		Size	preferredSize;

		auto * pSlot = slots._begin();
		auto * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
			{
				Size sz = pSlot->_paddedPreferredSize();
				if( sz.w > preferredSize.w )
					preferredSize.w = sz.w;
				if( sz.h > preferredSize.h )
					preferredSize.h = sz.h;
			}
			pSlot++;
		}

		return preferredSize;
	}

	//____ _adaptChildrenToSize() ___________________________________________________________

	void StackPanel::_adaptChildrenToSize()
	{
		auto * pSlot = slots._begin();
		auto * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
				OO(pSlot->_widget())->_resize( _childGeo(pSlot) );
			pSlot++;
		}
	}

	//____ _childGeo() ___________________________________________________________

	Rect StackPanel::_childGeo( const Slot * pSlot ) const
	{
		Rect base = Rect( m_size ) - pSlot->m_padding;

		if( base.w <= 0 || base.h <= 0 )
			return Rect();

		switch( pSlot->m_sizePolicy )
		{
			default:
		case SizePolicy2D::Original:
			{
				Size	size = pSlot->_widget()->preferredSize();
				Rect geo = Util::origoToRect( pSlot->m_origo, base, size );

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
				Size	orgSize = pSlot->_widget()->preferredSize();
				Size	size;

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

				return Util::origoToRect( pSlot->m_origo, base, size );
			}
		}
	}




} // namespace wg
