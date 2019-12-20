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

#include <wg_cpaddedslotarray.impl.h>

namespace wg
{
	template class CDynamicSlotArray<StackSlot>;
	template class CPaddedSlotArray<StackSlot>;

	const char StackPanel::CLASSNAME[] = {"StackPanel"};


	void CStackSlotArray::setSizePolicy( int index, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( _slot(index), policy );
	}

	void CStackSlotArray::setSizePolicy( iterator it, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( it._slot(), policy );
	}


	SizePolicy2D CStackSlotArray::SizePolicy( int index ) const
	{
		//TODO: Assert!

		return _slot(index)->SizePolicy;
	}

	SizePolicy2D CStackSlotArray::SizePolicy( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->SizePolicy;
	}


	void CStackSlotArray::setOrigo( int index, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( _slot(index), origo );
	}

	void CStackSlotArray::setOrigo( iterator it, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( it._slot(), origo );
	}


	Origo CStackSlotArray::origo( int index ) const
	{
		//TODO: Assert!

		return _slot(index)->origo;
	}

	Origo CStackSlotArray::origo( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->origo;
	}


	void CStackSlotArray::_setSizePolicy( StackSlot * pSlot, SizePolicy2D policy )
	{
		if( policy != pSlot->SizePolicy )
		{
			RectI oldGeo = ((StackPanel*)_holder())->_childGeo(pSlot);
			pSlot->SizePolicy = policy;
			RectI newGeo = ((StackPanel*)_holder())->_childGeo(pSlot);

			if (newGeo.size() != oldGeo.size())
			{
				((StackPanel*)_holder())->_requestRender(oldGeo);
				((StackPanel*)_holder())->_requestRender(newGeo);
				pSlot->_setSize(newGeo.size());
			}
		};
	}

	void CStackSlotArray::_setOrigo( StackSlot * pSlot, Origo origo )
	{
		if( origo != pSlot->origo )
		{
			((StackPanel*)_holder())->_childRequestRender( pSlot );
			pSlot->origo = origo;
			((StackPanel*)_holder())->_childRequestRender( pSlot );
		};
	}



	//____ Constructor ____________________________________________________________

	StackPanel::StackPanel() : slots(this)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	StackPanel::~StackPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool StackPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StackPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	StackPanel_p StackPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StackPanel_p( static_cast<StackPanel*>(pObject) );

		return 0;
	}

	//____ _matchingHeight() _______________________________________________________

	int StackPanel::_matchingHeight( int width ) const
	{
		int height = 0;

		StackSlot * pSlot = slots._begin();
		StackSlot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			int h = pSlot->_matchingHeight(width);
			if( h > height )
				height = h;
			pSlot++;
		}

		return height;
	}

	//____ _matchingWidth() _______________________________________________________

	int StackPanel::_matchingWidth( int height ) const
	{
		int width = 0;

		StackSlot * pSlot = slots._begin();
		StackSlot * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			int h = pSlot->_matchingWidth(height);
			if( h > height )
				height = h;
			pSlot++;
		}

		return width;
	}


	//____ _preferredSize() _____________________________________________________________

	SizeI StackPanel::_preferredSize() const
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

	void StackPanel::_resize( const SizeI& size )
	{
		//TODO: Optimize. If size is same then we only need to update those that have requested resize.

		Panel::_resize(size);
		_adaptChildrenToSize();
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
			StackSlot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void StackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		StackSlot * pSlot = (StackSlot*) package.pSlot;

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
		_unhideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _willRemoveSlots() ____________________________________________________

	void StackPanel::_willRemoveSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _repadSlots() _________________________________________________________

	void StackPanel::_repadSlots( StaticSlot * pSlot, int nb, BorderI padding )
	{
		for( int i = 0 ; i < nb ; i++ )
			((StackSlot*)pSlot)[i].padding = padding;

		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeI newPreferred =_calcPreferredSize();
		if (newPreferred != m_preferredSize || m_preferredSize != m_size)
			_requestResize();
	}

	void StackPanel::_repadSlots(StaticSlot * pSlot, int nb, const BorderI * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((StackSlot*)pSlot)[i].padding = * pPaddings++;

		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeI newPreferred = _calcPreferredSize();
		if (newPreferred != m_preferredSize || m_preferredSize != m_size)
			_requestResize();
	}


	//____ _didMoveSlots() ________________________________________________________

	void StackPanel::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childPos() ________________________________________________________

	CoordI StackPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return _childGeo((const StackSlot *)pSlot).pos();
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( StaticSlot * pSlot )
	{
		_childRequestRender( pSlot, _childGeo((StackSlot*) pSlot) );
	}

	void StackPanel::_childRequestRender( StaticSlot * _pSlot, const RectI& _rect )
	{
		StackSlot * pSlot = (StackSlot *) _pSlot;

		if( !pSlot->bVisible )
			return;

		// Put our rectangle into patches

		RectI rect = _rect + _childPos(pSlot);


		Patches patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = slots._begin() ; pCover < pSlot ; pCover++ )
		{
			RectI geo = _childGeo(pCover);
			if( pCover->bVisible && geo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, geo, RectI(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectI * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _childRequestResize() ______________________________________________

	void StackPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<StackSlot*>(_pSlot);

		SizeI newPreferred = _calcPreferredSize();

		if( newPreferred != m_preferredSize || m_preferredSize != m_size )
		{
			m_preferredSize = newPreferred;
			_requestResize();
		}
		else
		{
			pSlot->_setSize(_childGeo((StackSlot*)pSlot).size());
			_requestRender();
		}

	}

	//____ _prevChild() __________________________________________________________

	Widget * StackPanel::_prevChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const StackSlot *>(_pSlot);

		if( pSlot > slots._begin() )
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * StackPanel::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const StackSlot *>(_pSlot);

		if( pSlot < slots._last() )
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void StackPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		slots._remove(static_cast<StackSlot*>(pSlot));
	}

	//____ _replaceChild() ____________________________________________________

	void StackPanel::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
	{
		//TODO: Implement!!!

		assert(false);
	}


	//____ _unhideChildren() _____________________________________________________

	void StackPanel::_unhideChildren( StackSlot * pSlot, int nb )
	{
		bool	bRequestResize = false;

		// Update m_preferredSize

		for( int i = 0 ; i < nb ; i++ )
		{
			if( !pSlot[i].bVisible )
			{
				SizeI preferred = pSlot[i]._paddedPreferredSize();

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
			if( !pSlot[i].bVisible )
			{
				pSlot[i].bVisible = true;
				pSlot[i]._widget()->_resize(_childGeo(pSlot).size() );
				_childRequestRender( pSlot + i );
			}
		}
	}

	//____ _hideChildren() __________________________________________________

	void StackPanel::_hideChildren( StackSlot * pRemove, int nb )
	{
		bool	bRequestResize = false;

		// Get dirty rectangles for all visible sections of widgets to be removed.

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pRemove[i].bVisible )
			{
				_childRequestRender( pRemove + i );
				pRemove[i].bVisible = false;
			}
		}
		// Update m_preferredSize

		SizeI	preferredSize;
		StackSlot * p = slots._begin();

		while( p != slots._end() )
		{
			if( p->bVisible )
			{
				SizeI sz = p->_paddedPreferredSize();
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

	SizeI StackPanel::_calcPreferredSize()
	{
		SizeI	preferredSize;

		auto * pSlot = slots._begin();
		auto * pEnd = slots._end();

		while( pSlot != pEnd )
		{
			if( pSlot->bVisible )
			{
				SizeI sz = pSlot->_paddedPreferredSize();
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
			if( pSlot->bVisible )
				pSlot->_widget()->_resize( _childGeo(pSlot) );
			pSlot++;
		}
	}

	//____ _childGeo() ___________________________________________________________

	RectI StackPanel::_childGeo( const StackSlot * pSlot ) const
	{
		RectI base = RectI( m_size ) - pSlot->padding;

		if( base.w <= 0 || base.h <= 0 )
			return RectI(0,0,0,0);

		switch( pSlot->SizePolicy )
		{
			default:
		case SizePolicy2D::Original:
			{
				SizeI	size = pSlot->_preferredSize();
				RectI geo = Util::origoToRect( pSlot->origo, base, size );

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
				SizeI	orgSize = pSlot->_preferredSize();
				SizeI	size;

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

				return Util::origoToRect( pSlot->origo, base, size );
			}
		}
	}




} // namespace wg
