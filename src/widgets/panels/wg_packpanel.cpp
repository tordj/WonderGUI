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

#include <initializer_list>

#include <wg_packpanel.h>
#include <wg_base.h>


#include <wg_slotarray.impl.h>
#include <wg_iweightedchildren.impl.h>

namespace wg
{
	INSTANTIATE_WEIGHTEDCHILDREN(PackPanelSlot, WeightedChildrenHolder)

	template class SlotArray<PackPanelSlot>;

	const char PackPanel::CLASSNAME[] = {"PackPanel"};


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

	PackPanel_p PackPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return PackPanel_p( static_cast<PackPanel*>(pObject) );

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

	void PackPanel::setSizeBroker( SizeBroker * pBroker )
	{
		if( m_pSizeBroker.rawPtr() != pBroker )
		{
			m_pSizeBroker = pBroker;
			_updatePreferredSize();
			_refreshChildGeo();
		}
	}

	//____ preferredSize() _______________________________________________________

	SizeI PackPanel::preferredSize() const
	{
		SizeI size = m_preferredContentSize;
		if (m_pSkin)
			size += m_pSkin->contentPadding();

		return size;
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
					if( pS->bVisible )
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
					if( pS->bVisible && pS->preferredSize.h > height )
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
					if( pS->bVisible )
						height += pS->paddedMatchingHeight( width );
				}
			}
		}

		if (m_pSkin)
			height += m_pSkin->contentPadding().h;

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
					if( pS->bVisible )
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
					if( pS->bVisible && pS->preferredSize.w > width )
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
					if( pS->bVisible )
						width += pS->paddedMatchingWidth( height );
				}
			}
		}

		if (m_pSkin)
			width += m_pSkin->contentPadding().w;

		return width;
	}

	//____ _firstChild() __________________________________________________________

	Widget * PackPanel::_firstChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->pWidget;
	}

	//____ _lastChild() __________________________________________________________

	Widget * PackPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->pWidget;
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void PackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			PackPanelSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		PackPanelSlot * pSlot = (PackPanelSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
	}

	//____ _incSlot() ____________________________________________________________

	Slot * PackPanel::_incSlot(Slot * pSlot) const
	{
		return (static_cast<PackPanelSlot*>(pSlot) + 1);
	}

	//____ _decSlot() ____________________________________________________________

	Slot * PackPanel::_decSlot(Slot * pSlot) const
	{
		return (static_cast<PackPanelSlot*>(pSlot) - 1);
	}
	//____ _didAddSlots() _____________________________________________________

	void PackPanel::_didAddSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _didMoveSlots() _____________________________________________________

	void PackPanel::_didMoveSlots(Slot * pFrom, Slot * pTo, int nb)
	{
		//TODO: Optimize! Only update and re-render what is needed, but take
		// into account that SizeBroker might have weird rules and might affect
		// sizes in various ways when children change place...

		_updatePreferredSize();
		_refreshChildGeo();
	}

	//____ _willRemoveSlots() _________________________________________________

	void PackPanel::_willRemoveSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _hideSlots() _______________________________________________________

	void PackPanel::_hideSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void PackPanel::_unhideSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackPanelSlot*) pSlot, nb);
	}

	//____ _repadSlots() ______________________________________________________

	void PackPanel::_repadSlots(Slot * pSlot, int nb, BorderI padding)
	{
		for (int i = 0; i < nb; i++)
			((PackPanelSlot*)pSlot)[i].padding = padding;

		_updatePreferredSize();
		_refreshChildGeo();
	}

	void PackPanel::_repadSlots(Slot * pSlot, int nb, const BorderI * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((PackPanelSlot*)pSlot)[i].padding = * pPaddings++;

		_updatePreferredSize();
		_refreshChildGeo();
	}

	//____ _reweightSlots() ______________________________________________________

	void PackPanel::_reweightSlots(Slot * pSlot, int nb, float weight)
	{
		for (int i = 0; i < nb; i++)
			((PackPanelSlot*)pSlot)[i].weight = weight;

		if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
			_updatePreferredSize();

		_refreshChildGeo();
	}

	void PackPanel::_reweightSlots(Slot * pSlot, int nb, const float * pWeights)
	{
		for (int i = 0; i < nb; i++)
			((PackPanelSlot*)pSlot)[i].weight = * pWeights++;

		if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
			_updatePreferredSize();

		_refreshChildGeo();
	}

	//____ _childPos() _______________________________________________________

	CoordI PackPanel::_childPos(Slot * pSlot) const
	{
		return ((PackPanelSlot*)pSlot)->geo;
	}

	//____ _childSize() _______________________________________________________

	SizeI PackPanel::_childSize(Slot * pSlot) const
	{
		return ((PackPanelSlot*)pSlot)->geo;
	}

	//____ _childRequestRender() ______________________________________________

	void PackPanel::_childRequestRender(Slot * pSlot)
	{
		_requestRender( ((PackPanelSlot*)pSlot)->geo );

	}

	void PackPanel::_childRequestRender(Slot * pSlot, const RectI& rect)
	{
		_requestRender(rect + ((PackPanelSlot*)pSlot)->geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void PackPanel::_childRequestResize(Slot * _pSlot)
	{
		// Update cached preferred size of child

		PackPanelSlot * pSlot = static_cast<PackPanelSlot*>(_pSlot);
		pSlot->preferredSize = pSlot->paddedPreferredSize();

		_refreshAllWidgets();
	}

	//____ _prevChild() _______________________________________________________

	Widget * PackPanel::_prevChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackPanelSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1].pWidget;

		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * PackPanel::_nextChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackPanelSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1].pWidget;

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackPanel::_releaseChild(Slot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		m_children.remove(static_cast<PackPanelSlot*>(pSlot));
	}

	//____ _unhideChildren() _______________________________________________________

	void PackPanel::_unhideChildren(PackPanelSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].bVisible == false)
			{
				pSlot[i].bVisible = true;
				pSlot[i].preferredSize = pSlot[i].paddedPreferredSize();
			}
		}

		_refreshAllWidgets();
	}

	//____ _hideChildren() _______________________________________________________

	void PackPanel::_hideChildren(PackPanelSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].bVisible = false;

		_refreshAllWidgets();
	}


	//____ _onWidgetsReordered() ____________________________________________________
/*
	void PackPanel::_onWidgetsReordered()
	{
		_refreshChildGeo();
	}
*/
	//____ _refreshAllWidgets() _____________________________________________________

	void PackPanel::_refreshAllWidgets()
	{
		_updatePreferredSize();
		_refreshChildGeo();
	}



	//____ _setSize() ____________________________________________________________

	void PackPanel::_setSize( const SizeI& size )
	{
		Panel::_setSize(size);
		_refreshChildGeo(false);
	}


	//____ _updatePreferredSize() ______________________________________________________

	void PackPanel::_updatePreferredSize()
	{
		int length = 0;
		int breadth = 0;

		if( m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths() )
		{
			// Allocate and populate SizeBroker array

			int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

			int nItems = _populateSizeBrokerArray(pItemArea);

			// Retrieve preferred length and breadth

			length = m_pSizeBroker->setPreferredLengths( pItemArea, nItems );

			SizeBrokerItem * pI = pItemArea;
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					int b = m_bHorizontal?pS->paddedMatchingHeight(pI->output):pS->paddedMatchingWidth(pI->output);
					if( b > breadth )
						breadth = b;
					pI++;
				}
			}

			// Release temporary memory area

			Base::memStackRelease(arrayBytes);

		}
		else
		{
			if( m_bHorizontal )
			{
				for (auto p = m_children.begin(); p != m_children.end(); p++)
				{
					if( p->bVisible )
					{
						length += p->preferredSize.w;
						if( p->preferredSize.h > breadth )
							breadth = p->preferredSize.h;
					}
				}
			}
			else
			{
				for (auto p = m_children.begin(); p != m_children.end(); p++)
				{
					if( p->bVisible )
					{
						length += p->preferredSize.h;
						if( p->preferredSize.w > breadth )
							breadth = p->preferredSize.w;
					}
				}
			}
		}

		//

		SizeI size = m_bHorizontal?SizeI(length,breadth):SizeI(breadth,length);



//TODO: This optimization was incorrect. Wrap-text might need a different MatchingHeight although preferred size remains the same.
// This happens when a line, that isn't the longest line, needs to wrap. Find a better optimization.

//		if( size != m_preferredContentSize )
		{
			m_preferredContentSize = size;
			_requestResize();
		}
	}

	//____ _refreshChildGeo() _________________________________________________________

	void PackPanel::_refreshChildGeo( bool bRequestRender )
	{
		if( m_children.isEmpty() )
			return;

		SizeI sz = size();
		CoordI contentOfs;

		if (m_pSkin)
		{
			sz -= m_pSkin->contentPadding();
			contentOfs = m_pSkin->contentOfs(StateEnum::Normal);			//TODO: Support offset changing in different states.
		}

		int wantedLength = m_bHorizontal?m_preferredContentSize.w:m_preferredContentSize.h;
		int givenLength = m_bHorizontal?sz.w:sz.h;
		int givenBreadth = m_bHorizontal?sz.h:sz.w;

		// Optimized special case, just copy preferred to length.
		//TODO: We probably need to use matchingWidth()/matchingHeight() here anyway... prefered length might change with given breadth

		if( !m_pSizeBroker || (wantedLength == givenLength && !m_pSizeBroker->mayAlterPreferredLengths()) )
		{
			CoordI pos;
			RectI geo;
			for (auto p = m_children.begin(); p != m_children.end(); p++)
			{
				if( p->bVisible )
				{
					geo.x = pos.x;
					geo.y = pos.y;
					if( m_bHorizontal )
					{
						geo.w = p->preferredSize.w;
						geo.h = sz.h;
						pos.x += p->preferredSize.w;
					}
					else
					{
						geo.w = sz.w;
						geo.h = p->preferredSize.h;
						pos.y += p->preferredSize.h;
					}
					geo -= p->padding;
					geo += contentOfs;

					if( geo != p->geo )
					{
						if (bRequestRender)
						{
							_requestRender(geo);
							_requestRender(p->geo);
						}

						int oldW = p->geo.w;
						int oldH = p->geo.h;
						p->geo = geo;
						if( geo.w != oldW || geo.h != oldH )
							p->pWidget->_setSize( geo.size() );

					}
				}
				else
				{
					if( bRequestRender && p->geo.w != 0 && p->geo.h != 0 )
						_requestRender(p->geo);

					p->geo.x = pos.x + contentOfs.x;
					p->geo.y = pos.y + contentOfs.y;
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
			}
		}
		else
		{
			// Allocate and populate SizeBroker array

			int arrayBytes = sizeof(SizeBrokerItem)*m_children.size();
			SizeBrokerItem * pItemArea = reinterpret_cast<SizeBrokerItem*>(Base::memStackAlloc(arrayBytes));

			int nItems = _populateSizeBrokerArray(pItemArea, givenBreadth);

			// Retrieve length and set geo for all children, call _requestRender() and _setSize() where needed.

			m_pSizeBroker->setItemLengths( pItemArea, nItems, givenLength );

			SizeBrokerItem * pI = pItemArea;

			CoordI pos;
			RectI geo;
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
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
					geo -= pS->padding;
					geo += contentOfs;

					if( geo != pS->geo )
					{
						if( bRequestRender )
						{
							_requestRender(geo);
							_requestRender(pS->geo);
						}
						int oldW = pS->geo.w;
						int oldH = pS->geo.h;
						pS->geo = geo;
						if( geo.w != oldW || geo.h != oldH )
							pS->pWidget->_setSize( geo.size() );
					}
					pI++;
				}
				else
				{
					if( bRequestRender && pS->geo.w != 0 && pS->geo.h != 0 )
						_requestRender(pS->geo);

					pS->geo.x = pos.x + contentOfs.x;
					pS->geo.y = pos.y + contentOfs.y;
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
			}

			// Release SizeBroker array

			Base::memStackRelease(arrayBytes);
		}
	}

	//____ _populateSizeBrokerArray() ___________________________________________

	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray ) const
	{
		SizeBrokerItem * pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					pI->preferred = pS->preferredSize.w;
					pI->min = pS->paddedMinSize().w;
					pI->max = pS->paddedMaxSize().w;
					pI->weight = pS->weight;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					pI->preferred = pS->preferredSize.h;
					pI->min = pS->paddedMinSize().h;
					pI->max = pS->paddedMaxSize().h;
					pI->weight = pS->weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	int PackPanel::_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const
	{
		SizeBrokerItem * pI = pArray;

		if( m_bHorizontal )
		{
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					pI->preferred = pS->paddedMatchingWidth(forcedBreadth);
					pI->min = pS->paddedMinSize().w;
					pI->max = pS->paddedMaxSize().w;
					pI->weight = pS->weight;
					pI++;
				}
			}
		}
		else
		{
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					pI->preferred = pS->paddedMatchingHeight(forcedBreadth);
					pI->min = pS->paddedMinSize().h;
					pI->max = pS->paddedMaxSize().h;
					pI->weight = pS->weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

} // namespace wg
