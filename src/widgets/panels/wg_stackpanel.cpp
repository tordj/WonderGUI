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

#include <wg_stackpanel.h>
#include <wg_util.h>
#include <wg_patches.h>

#include <wg_slotarray.impl.h>
#include <wg_ipaddedslotarray.impl.h>

namespace wg
{
	template class ISlotArray<StackSlot>;
	template class IPaddedSlotArray<StackSlot>;
	template class SlotArray<StackSlot>;

	const char StackPanel::CLASSNAME[] = {"StackPanel"};


	void IStackSlots::setSizePolicy( int index, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( m_pSlotArray->slot(index), policy );
	}

	void IStackSlots::setSizePolicy( iterator it, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( it._slot(), policy );
	}


	SizePolicy2D IStackSlots::SizePolicy( int index ) const
	{
		//TODO: Assert!

		return m_pSlotArray->slot(index)->SizePolicy;
	}

	SizePolicy2D IStackSlots::SizePolicy( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->SizePolicy;
	}


	void IStackSlots::setOrigo( int index, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( m_pSlotArray->slot(index), origo );
	}

	void IStackSlots::setOrigo( iterator it, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( it._slot(), origo );
	}


	Origo IStackSlots::origo( int index ) const
	{
		//TODO: Assert!

		return m_pSlotArray->slot(index)->origo;
	}

	Origo IStackSlots::origo( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->origo;
	}


	void IStackSlots::_setSizePolicy( StackSlot * pSlot, SizePolicy2D policy )
	{
		if( policy != pSlot->SizePolicy )
		{
			RectI oldGeo = ((StackPanel*)m_pHolder)->_childGeo(pSlot);
			pSlot->SizePolicy = policy;
			RectI newGeo = ((StackPanel*)m_pHolder)->_childGeo(pSlot);

			if (newGeo.size() != oldGeo.size())
			{
				((StackPanel*)m_pHolder)->_requestRender(oldGeo);
				((StackPanel*)m_pHolder)->_requestRender(newGeo);
				pSlot->setSize(newGeo.size());
			}
		};
	}

	void IStackSlots::_setOrigo( StackSlot * pSlot, Origo origo )
	{
		if( origo != pSlot->origo )
		{
			((StackPanel*)m_pHolder)->_childRequestRender( pSlot );
			pSlot->origo = origo;
			((StackPanel*)m_pHolder)->_childRequestRender( pSlot );
		};
	}



	//____ Constructor ____________________________________________________________

	StackPanel::StackPanel() : children(&m_children,this)
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

		StackSlot * pSlot = m_children.begin();
		StackSlot * pEnd = m_children.end();

		while( pSlot != pEnd )
		{
			int h = pSlot->matchingHeight(width);
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

		StackSlot * pSlot = m_children.begin();
		StackSlot * pEnd = m_children.end();

		while( pSlot != pEnd )
		{
			int h = pSlot->matchingWidth(height);
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
		if( m_children.isEmpty() )
			return nullptr;

		return m_children.first()->_widget();
	}

	//____ _lastChild() ________________________________________________________

	Widget * StackPanel::_lastChild() const
	{
		if( m_children.isEmpty() )
			return nullptr;

		return m_children.last()->_widget();

	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void StackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			StackSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void StackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		StackSlot * pSlot = (StackSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void StackPanel::_didAddSlots( Slot * pSlot, int nb )
	{
		_unhideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _willRemoveSlots() ____________________________________________________

	void StackPanel::_willRemoveSlots( Slot * pSlot, int nb )
	{
		_hideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( Slot * pSlot, int nb )
	{
		_hideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( Slot * pSlot, int nb )
	{
		_unhideChildren( (StackSlot*) pSlot, nb );
	}

	//____ _repadSlots() _________________________________________________________

	void StackPanel::_repadSlots( Slot * pSlot, int nb, BorderI padding )
	{
		for( int i = 0 ; i < nb ; i++ )
			((StackSlot*)pSlot)[i].padding = padding;

		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeI newPreferred =_calcPreferredSize();
		if (newPreferred != m_preferredSize || m_preferredSize != m_size)
			_requestResize();
	}

	void StackPanel::_repadSlots(Slot * pSlot, int nb, const BorderI * pPaddings)
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

	void StackPanel::_didMoveSlots(Slot * pFrom, Slot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childPos() ________________________________________________________

	CoordI StackPanel::_childPos( Slot * pSlot ) const
	{
		return _childGeo((StackSlot *)pSlot).pos();
	}

	//____ _childSize() _______________________________________________________

	SizeI StackPanel::_childSize( Slot * pSlot ) const
	{
		return static_cast<StackSlot*>(pSlot)->size();
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( Slot * pSlot )
	{
		_childRequestRender( pSlot, _childGeo((StackSlot*) pSlot) );
	}

	void StackPanel::_childRequestRender( Slot * _pSlot, const RectI& _rect )
	{
		StackSlot * pSlot = (StackSlot *) _pSlot;

		if( !pSlot->bVisible )
			return;

		// Put our rectangle into patches

		RectI rect = _rect + _childPos(pSlot);


		Patches patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = m_children.begin() ; pCover < pSlot ; pCover++ )
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

	void StackPanel::_childRequestResize( Slot * _pSlot )
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
			pSlot->setSize(_childGeo((StackSlot*)pSlot).size());
			_requestRender();
		}

	}

	//____ _prevChild() __________________________________________________________

	Widget * StackPanel::_prevChild( const Slot * _pSlot ) const
	{
		auto pSlot = static_cast<const StackSlot *>(_pSlot);

		if( pSlot > m_children.begin() )
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * StackPanel::_nextChild( const Slot * _pSlot ) const
	{
		auto pSlot = static_cast<const StackSlot *>(_pSlot);

		if( pSlot < m_children.last() )
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void StackPanel::_releaseChild(Slot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		m_children.remove(static_cast<StackSlot*>(pSlot));
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
				SizeI preferred = pSlot[i].paddedPreferredSize();

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
		StackSlot * p = m_children.begin();

		while( p != m_children.end() )
		{
			if( p->bVisible )
			{
				SizeI sz = p->paddedPreferredSize();
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

		auto * pSlot = m_children.begin();
		auto * pEnd = m_children.end();

		while( pSlot != pEnd )
		{
			if( pSlot->bVisible )
			{
				SizeI sz = pSlot->paddedPreferredSize();
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
		auto * pSlot = m_children.begin();
		auto * pEnd = m_children.end();

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
				SizeI	size = pSlot->preferredSize();
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
				SizeI	orgSize = pSlot->preferredSize();
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
