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
#include <wg_ipaddedslotarray.impl.h>

namespace wg
{
	using namespace Util;

	template class ISlotArray<PackSlot>;
	template class IPaddedSlotArray<PackSlot>;
	template class SlotArray<PackSlot>;

	const char PackPanel::CLASSNAME[] = {"PackPanel"};



	bool IPackSlots::setWeight(int index, float weight)
	{
		if (index < 0 || index >= m_pSlotArray->size() || weight < 0.f)
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pSlot, 1, weight);
		return true;
	}

	bool IPackSlots::setWeight(const SlotIterator& it, float weight)
	{
		//TODO: Add assert

		auto pSlot = static_cast<PackSlot*>(it._slot());
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pSlot, 1, weight);
		return true;
	}

	bool IPackSlots::setWeight(int index, int amount, float weight)
	{
		if (index < 0 || amount <= 0 || index + amount >= m_pSlotArray->size() || weight < 0.f)
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pSlot, amount, weight);
		return true;
	}

	bool IPackSlots::setWeight(const SlotIterator& beg, const SlotIterator& end, float weight)
	{
		//TODO: Add assert

		auto pBeg = static_cast<PackSlot*>(beg._slot());
		auto pEnd = static_cast<PackSlot*>(end._slot());
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pBeg, int(pEnd - pBeg), weight);
		return true;
	}

	bool IPackSlots::setWeight(int index, int amount, const std::initializer_list<float> weights)
	{
		if (index < 0 || amount <= 0 || index + amount >= m_pSlotArray->size() || amount >(int) weights.size())
			return false;

		auto pSlot = m_pSlotArray->slot(index);
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pSlot, amount, weights.begin());
		return true;
	}

	bool IPackSlots::setWeight(const SlotIterator& beg, const SlotIterator& end, const std::initializer_list<float> weights)
	{
		//TODO: Add assert

		auto pBeg = static_cast<PackSlot*>(beg._slot());
		auto pEnd = static_cast<PackSlot*>(end._slot());
		static_cast<PackPanel*>(m_pHolder)->_reweightSlots(pBeg, int(pEnd - pBeg), weights.begin());
		return true;
	}

	float IPackSlots::weight(int index) const
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return 0.f;

		return m_pSlotArray->slot(index)->weight;

	}

	float IPackSlots::weight(const SlotIterator& it) const
	{
		//TODO: Add assert

		auto pSlot = static_cast<PackSlot*>(it._slot());
		return pSlot->weight;
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
			_refreshGeometries();

		}
	}


	//____ setSizeBroker() _______________________________________________________

	void PackPanel::setSizeBroker( SizeBroker * pBroker )
	{
		if( m_pSizeBroker.rawPtr() != pBroker )
		{
			m_pSizeBroker = pBroker;
			_refreshGeometries();

		}
	}

	//____ _preferredSize() _______________________________________________________

	SizeI PackPanel::_preferredSize() const
	{
		SizeI size = m_preferredContentSize;
		if (m_pSkin)
			size += m_pSkin->_contentPadding();

		return size;
	}

	//____ _matchingHeight() _______________________________________________________

	int PackPanel::_matchingHeight( int width ) const
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

				_setItemLengths( pItemArea, nItems, width );

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

				height = _setPreferredLengths( pItemArea, nItems );

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
			height += m_pSkin->_contentPadding().h;

		return height;
	}

	//____ _matchingWidth() _______________________________________________________

	int PackPanel::_matchingWidth( int height ) const
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

				_setItemLengths( pItemArea, nItems, height );

				SizeBrokerItem * pI = pItemArea;

				for( auto pS = m_children.begin() ; pS != m_children.end() ; pS++ )
				{
					if( pS->bVisible )
					{
						int itemWidth = pS->paddedMatchingWidth( pI->output.raw );
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

				width = _setPreferredLengths( pItemArea, nItems );

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
			width += m_pSkin->_contentPadding().w;

		return width;
	}

	//____ _firstChild() __________________________________________________________

	Widget * PackPanel::_firstChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->_widget();
	}

	//____ _lastChild() __________________________________________________________

	Widget * PackPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->_widget();
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void PackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			PackSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	void PackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		PackSlot * pSlot = (PackSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void PackPanel::_didAddSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackSlot*) pSlot, nb);
	}

	//____ _didMoveSlots() _____________________________________________________

	void PackPanel::_didMoveSlots(Slot * pFrom, Slot * pTo, int nb)
	{
		//TODO: Optimize! Only update and re-render what is needed, but take
		// into account that SizeBroker might have weird rules and might affect
		// sizes in various ways when children change place...

		_refreshGeometries();
	}

	//____ _willRemoveSlots() _________________________________________________

	void PackPanel::_willRemoveSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackSlot*) pSlot, nb);
	}

	//____ _hideSlots() _______________________________________________________

	void PackPanel::_hideSlots(Slot * pSlot, int nb)
	{
		_hideChildren((PackSlot*) pSlot, nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void PackPanel::_unhideSlots(Slot * pSlot, int nb)
	{
		_unhideChildren((PackSlot*) pSlot, nb);
	}

	//____ _repadSlots() ______________________________________________________

	void PackPanel::_repadSlots(Slot * pSlot, int nb, BorderI padding)
	{
		for (int i = 0; i < nb; i++)
			((PackSlot*)pSlot)[i].padding = padding;

		_refreshGeometries();
	}

	void PackPanel::_repadSlots(Slot * pSlot, int nb, const BorderI * pPaddings)
	{
		for (int i = 0; i < nb; i++)
			((PackSlot*)pSlot)[i].padding = * pPaddings++;

		_refreshGeometries();
	}

	//____ _reweightSlots() ______________________________________________________

	void PackPanel::_reweightSlots(PackSlot * pSlot, int nb, float weight)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].weight = weight;

		if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
			_refreshGeometries();
		else
			_refreshChildGeo();
	}

	void PackPanel::_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].weight = * pWeights++;

		if (m_pSizeBroker && m_pSizeBroker->mayAlterPreferredLengths())
			_refreshGeometries();
		else
			_refreshChildGeo();
	}

	//____ _childPos() _______________________________________________________

	CoordI PackPanel::_childPos(Slot * pSlot) const
	{
		return ((PackSlot*)pSlot)->geo;
	}

	//____ _childSize() _______________________________________________________

	SizeI PackPanel::_childSize(Slot * pSlot) const
	{
		return ((PackSlot*)pSlot)->geo;
	}

	//____ _childRequestRender() ______________________________________________

	void PackPanel::_childRequestRender(Slot * pSlot)
	{
		_requestRender( ((PackSlot*)pSlot)->geo );

	}

	void PackPanel::_childRequestRender(Slot * pSlot, const RectI& rect)
	{
		_requestRender(rect + ((PackSlot*)pSlot)->geo.pos());
	}

	//____ _childRequestResize() ______________________________________________

	void PackPanel::_childRequestResize(Slot * _pSlot)
	{
		// Update cached preferred size of child

		PackSlot * pSlot = static_cast<PackSlot*>(_pSlot);
		pSlot->preferredSize = pSlot->paddedPreferredSize();
		pSlot->bResizeRequired = true;

		_refreshGeometries();
	}

	//____ _prevChild() _______________________________________________________

	Widget * PackPanel::_prevChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	Widget * PackPanel::_nextChild(const Slot * _pSlot) const
	{
		auto pSlot = static_cast<const PackSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void PackPanel::_releaseChild(Slot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		m_children.remove(static_cast<PackSlot*>(pSlot));
	}

	//____ _unhideChildren() _______________________________________________________

	void PackPanel::_unhideChildren(PackSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			if (pSlot[i].bVisible == false)
			{
				pSlot[i].bVisible = true;
				pSlot[i].preferredSize = pSlot[i].paddedPreferredSize();
			}
		}

		_refreshGeometries();
	}

	//____ _hideChildren() _______________________________________________________

	void PackPanel::_hideChildren(PackSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].bVisible = false;

		_refreshGeometries();
	}


	//____ _onWidgetsReordered() ____________________________________________________
/*
	void PackPanel::_onWidgetsReordered()
	{
		_refreshChildGeo();
	}
*/
	//____ _refreshGeometries() _____________________________________________________

	void PackPanel::_refreshGeometries()
	{
		// Recalculate preferred sizes for widget and content.

		SizeI newPreferredContentSize = _calcPreferredSize();
		SizeI newPreferredSize = m_pSkin ? newPreferredContentSize + m_pSkin->_contentPadding() : newPreferredContentSize;

		// request resize or just refresh child geo, depending on what is needed.

		if (newPreferredContentSize != m_preferredContentSize || newPreferredSize != m_size)
		{
			m_preferredContentSize = newPreferredContentSize;
			_requestResize();
		}
		else
			_refreshChildGeo();
	}


	//____ _resize() ____________________________________________________________

	void PackPanel::_resize( const SizeI& size )
	{
		Panel::_resize(size);
		_refreshChildGeo(false);
	}

	//____ _calcPreferredSize() ______________________________________________________

	SizeI PackPanel::_calcPreferredSize()
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

			length = _setPreferredLengths( pItemArea, nItems );

			SizeBrokerItem * pI = pItemArea;
			for (auto pS = m_children.begin(); pS != m_children.end(); pS++)
			{
				if( pS->bVisible )
				{
					int b = m_bHorizontal?pS->paddedMatchingHeight(pI->output.raw):pS->paddedMatchingWidth(pI->output.raw);
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

		return m_bHorizontal?SizeI(length,breadth):SizeI(breadth,length);
	}

	//____ _refreshChildGeo() _________________________________________________________

	void PackPanel::_refreshChildGeo( bool bRequestRender )
	{
		if( m_children.isEmpty() )
			return;

		SizeI sz = m_size;
		CoordI contentOfs;

		if (m_pSkin)
		{
			sz -= m_pSkin->_contentPadding();
			contentOfs = m_pSkin->_contentOfs(StateEnum::Normal);			//TODO: Support offset changing in different states.
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
						{
							p->_widget()->_resize( geo.size() );
							p->bResizeRequired = false;
						}

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

				if( p->bResizeRequired )
				{
					p->_widget()->_resize(geo.size());
					p->bResizeRequired = false;
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

			_setItemLengths( pItemArea, nItems, givenLength );

			SizeBrokerItem * pI = pItemArea;

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
						geo.w = pI->output.raw;
						geo.h = sz.h;
						pos.x += pI->output.raw;
					}
					else
					{
						geo.w = sz.w;
						geo.h = pI->output.raw;
						pos.y += pI->output.raw;
					}
					geo -= p->padding;
					geo += contentOfs;

					if( geo != p->geo )
					{
						if( bRequestRender )
						{
							_requestRender(geo);
							_requestRender(p->geo);
						}
						int oldW = p->geo.w;
						int oldH = p->geo.h;
						p->geo = geo;
						if( geo.w != oldW || geo.h != oldH )
						{
							p->_widget()->_resize( geo.size() );
							p->bResizeRequired = false;
						}
					}
					pI++;
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

				if (p->bResizeRequired)
				{
					p->_widget()->_resize(geo.size());
					p->bResizeRequired = false;
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
					pI->preferred = QPix::fromRaw(pS->preferredSize.w);
					pI->min = QPix::fromRaw(pS->paddedMinSize().w);
					pI->max = QPix::fromRaw(pS->paddedMaxSize().w);
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
					pI->preferred = QPix::fromRaw(pS->preferredSize.h);
					pI->min = QPix::fromRaw(pS->paddedMinSize().h);
					pI->max = QPix::fromRaw(pS->paddedMaxSize().h);
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
					pI->preferred = QPix::fromRaw(pS->paddedMatchingWidth(forcedBreadth));
					pI->min = QPix::fromRaw(pS->paddedMinSize().w);
					pI->max = QPix::fromRaw(pS->paddedMaxSize().w);
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
					pI->preferred = QPix::fromRaw(pS->paddedMatchingHeight(forcedBreadth));
					pI->min = QPix::fromRaw(pS->paddedMinSize().h);
					pI->max = QPix::fromRaw(pS->paddedMaxSize().h);
					pI->weight = pS->weight;
					pI++;
				}
			}
		}

		return int(pI - pArray);
	}

	//____ _setItemLengths() ___________________________________________________________

	int PackPanel::_setItemLengths(SizeBrokerItem * pItems, int nItems, int availableLength) const
	{
		QPix totalLength = m_pSizeBroker->setItemLengths(pItems, nItems, QPix::fromRaw(availableLength) );

		// Align outputs so we end up on pixel boundaries

		int reminder = 0;
		for (int i = 0; i < nItems; i++)
		{
			reminder += pItems->output.raw;
			pItems->output.raw = reminder & 0xFFFFFFFC;
			reminder &= 0x3;
		}

		return totalLength.raw & 0xFFFFFFFC;
	}

	//____ _setPreferredLengths() _______________________________________________________

	int PackPanel::_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const
	{
		QPix totalLength = m_pSizeBroker->setPreferredLengths(pItems, nItems);

		// Align outputs so we end up on pixel boundaries

		int reminder = 0;
		for (int i = 0; i < nItems; i++)
		{
			reminder += pItems->output.raw;
			pItems->output.raw = reminder & 0xFFFFFFFC;
			reminder &= 0x3;
		}

		return totalLength.raw & 0xFFFFFFFC;
	}


} // namespace wg
