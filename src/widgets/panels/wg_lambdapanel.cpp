/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_lambdapanel.h>
#include <wg_patches.h>
#include <wg_util.h>

#include <wg_cdynamicslotarray.impl.h>
#include <assert.h>


namespace wg
{
	using namespace Util;

	template class CDynamicSlotArray<LambdaSlot>;


	const char LambdaPanel::CLASSNAME[] = {"LambdaPanel"};

	//____ CLambdaSlotArray::pushFront() ________________________________________________

	CLambdaSlotArray::iterator CLambdaSlotArray::pushFront( Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		LambdaSlot * pSlot = _pushFrontEmpty();
		pSlot->_setWidget(pWidget);
		pSlot->m_func = func;
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ CLambdaSlotArray::pushBack() ________________________________________________

	CLambdaSlotArray::iterator CLambdaSlotArray::pushBack(Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func)
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		LambdaSlot * pSlot = _pushBackEmpty();
		pSlot->_setWidget(pWidget);
		pSlot->m_func = func;
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ CLambdaSlotArray::insert() ________________________________________________

	CLambdaSlotArray::iterator CLambdaSlotArray::insert( int index, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaSlot * pSlot = _insertEmpty(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		pSlot->m_func = func;
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	CLambdaSlotArray::iterator CLambdaSlotArray::insert( iterator pos, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaSlot * pSlot = _insertEmpty(pos._slot());
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		pSlot->m_func = func;
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ Constructor ____________________________________________________________

	LambdaPanel::LambdaPanel() : slots(this), m_minSize(0,0), m_preferredSize(512*4,512*4), m_maxSize(INT_MAX,INT_MAX)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	LambdaPanel::~LambdaPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool LambdaPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * LambdaPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	LambdaPanel_p LambdaPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return LambdaPanel_p( static_cast<LambdaPanel*>(pObject) );

		return 0;
	}

	//____ setMinSize() _________________________________________________________________

	bool LambdaPanel::setMinSize(Size _min)
	{
		//TODO: Assert >= 0.

		SizeI min = qpixToRaw(_min);

		if( m_minSize != min )
		{
			if( min.w > m_maxSize.w || min.h > m_maxSize.h )
				return false;

			m_minSize = min;
			if( m_preferredSize.w < min.w )
				m_preferredSize.w = min.w;
			if( m_preferredSize.h < min.h )
				m_preferredSize.h = min.h;

			_requestResize();
		}
		return true;
	}

	//____ setMaxSize() ________________________________________________________

	bool LambdaPanel::setMaxSize(Size _max)
	{
		//TODO: Assert >= 0.

		SizeI max = qpixToRaw(_max);

 		if( m_maxSize != max )
		{
			if( max.w < m_minSize.w || max.h < m_minSize.h )
				return false;

			m_maxSize = max;
			if( m_preferredSize.w > max.w )
				m_preferredSize.w = max.w;
			if( m_preferredSize.h > max.h )
				m_preferredSize.h = max.h;

			_requestResize();
		}
		return true;
	}

	//____ setSizeLimits() _____________________________________________________

	bool LambdaPanel::setSizeLimits( Size min, Size max )
	{
		//TODO: Assert >= 0.

		if( min.w > max.w || min.h > max.h )
			return false;

		m_minSize = qpixToRaw(min);
		m_maxSize = qpixToRaw(max);
		limit( m_preferredSize.w, m_minSize.w, m_maxSize.w );
		limit( m_preferredSize.h, m_minSize.h, m_maxSize.h );

		_requestResize();
		return true;
	}

	//____ setPreferredSize() __________________________________________________

	bool LambdaPanel::setPreferredSize(Size _pref)
	{
		//TODO: Assert >= 0.

		SizeI pref = qpixToRaw(_pref);

		if( pref.w > m_maxSize.w || pref.h > m_maxSize.h || pref.w < m_minSize.w || pref.h < m_minSize.h )
			return false;

		m_preferredSize = pref;
		_requestResize();
		return true;
	}

	//____ _preferredSize() _____________________________________________________

	SizeI LambdaPanel::_preferredSize() const
	{
		return m_preferredSize;
	}

	//____ _firstChild() _______________________________________________________

	Widget * LambdaPanel::_firstChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._first()->_widget();
	}

	//____ _lastChild() ________________________________________________________

	Widget * LambdaPanel::_lastChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._last()->_widget();
	}

	//____ _firstSlotWithGeo() _________________________________________________

	void LambdaPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( slots.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			LambdaSlot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}

	}

	//____ _nextSlotWithGeo() _________________________________________________

	void LambdaPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		LambdaSlot * pSlot = (LambdaSlot*) package.pSlot;

		if( pSlot == slots._last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void LambdaPanel::_didAddSlots( StaticSlot * pSlot, int nb )
	{
		_unhideSlots(static_cast<LambdaSlot*>(pSlot), nb);
	}

	//____ _willEraseSlots() _________________________________________________

	void LambdaPanel::_willEraseSlots( StaticSlot * pSlot, int nb )
	{
		_hideSlots(static_cast<LambdaSlot*>(pSlot), nb);
	}

	//____ _hideSlots() __________________________________________________________

	void LambdaPanel::_hideSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == true )
			{
				_onRequestRender(pSlot[i].m_geo, pSlot);
				pSlot[i].m_bVisible = false;					// Needs to be done AFTER _onRequestRender()!
			}
		}
	}

	//____ _unhideSlots() ________________________________________________________

	void LambdaPanel::_unhideSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == false )
			{
				pSlot[i].m_bVisible = true;
				_updateGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].m_geo, pSlot);
			}
		}
	}


	//____ _updateSlotGeo() ________________________________________________________

	void LambdaPanel::_updateSlotGeo(StaticSlot * _pSlot, int nb)
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			_updateGeo(&pSlot[i]);
			if (pSlot[i].m_bVisible == false)
				_onRequestRender(pSlot[i].m_geo, pSlot);		//TODO: This looks weird...
		}
	}



	//____ _didMoveSlots() ________________________________________________________

	void LambdaPanel::_didMoveSlots(StaticSlot * _pFrom, StaticSlot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		auto pFrom = static_cast<LambdaSlot*>(_pFrom);
		auto pTo = static_cast<LambdaSlot*>(_pTo);

		if (pTo->m_bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				LambdaSlot * p = pTo+1;
				while (p <= pFrom)
				{
					RectI cover(pTo->m_geo, p->m_geo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, pTo);
					p++;
				}
			}
			else							// Move us backward
			{
				// Request render on our siblings for the area we previously have covered.

				LambdaSlot * p = pFrom;
				while (p < pTo)
				{
					RectI cover(pTo->m_geo, p->m_geo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}


	//____ _childPos() __________________________________________________________

	CoordI LambdaPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return ((LambdaSlot*)pSlot)->m_geo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void LambdaPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);
		_onRequestRender( pSlot->m_geo, pSlot );
	}

	void LambdaPanel::_childRequestRender( StaticSlot * _pSlot, const RectI& rect )
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->m_geo.pos(), pSlot );
	}

	//____ _childRequestResize() ______________________________________________

	void LambdaPanel::_childRequestResize( StaticSlot * pSlot )
	{
		_updateGeo(static_cast<LambdaSlot*>(pSlot), true);
	}

	//____ _prevChild() __________________________________________________________

	Widget * LambdaPanel::_prevChild( const StaticSlot * _pSlot ) const
	{
		const LambdaSlot * pSlot = static_cast<const LambdaSlot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;

	}

	//____ _nextChild() __________________________________________________________

	Widget * LambdaPanel::_nextChild( const StaticSlot * _pSlot ) const
	{
		const LambdaSlot * pSlot = static_cast<const LambdaSlot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void LambdaPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<LambdaSlot*>(pSlot));
	}

	//____ _replaceChild() ______________________________________________________

	void LambdaPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewChild)
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);

		slots._releaseGuardPointer(pNewChild, &pSlot);
		pSlot->_setWidget(pNewChild);

   		if (pSlot->m_bVisible)
		{
			_updateGeo(pSlot);
			_onRequestRender(pSlot->m_geo, pSlot);
		}
	}

	//____ _cloneContent() _______________________________________________________

	void LambdaPanel::_cloneContent( const Widget * _pOrg )
	{
		//TODO: Implement
	}

	//____ _resize() ____________________________________________________________

	void LambdaPanel::_resize( const SizeI& size )
	{
		Panel::_resize( size );

		for (auto * pSlot = slots._begin() ; pSlot != slots._end() ; pSlot++)
			_updateGeo(pSlot);
	}

	//____ _updateGeo() _______________________________________________________

	void LambdaPanel::_updateGeo(LambdaSlot * pSlot, bool bForceResize )
	{
		//TODO: Don't requestRender if slot is hidden.

		RectI geo;
			
		if (pSlot->m_func)
			geo = pixelAligned(qpixToRaw(pSlot->m_func(pSlot->_widget(), rawToQpix(m_size))));
		else
			geo = { 0,0,pSlot->_preferredSize() };

		if (geo != pSlot->m_geo)
		{
			if (pSlot->m_bVisible)
			{
				// Clip our geometry and put it in a dirtyrect-list

				Patches patches;
				patches.add(RectI(pSlot->m_geo, RectI(0, 0, m_size)));
				patches.add(RectI(geo, RectI(0, 0, m_size)));

				// Remove portions of patches that are covered by opaque upper siblings

				const LambdaSlot * pCover = pSlot + 1;
				while (pCover < slots._end())
				{
					if (pCover->m_bVisible && (pCover->m_geo.intersectsWith(pSlot->m_geo) || pCover->m_geo.intersectsWith(geo)) )
						pCover->_widget()->_maskPatches(patches, pCover->m_geo, RectI(0, 0, 65536, 65536), _getBlendMode());

					pCover++;
				}

				// Make request render calls

				for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
					_requestRender(*pRect);

			}
		}

		pSlot->m_geo = geo;

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _onRequestRender() ____________________________________________________

	void LambdaPanel::_onRequestRender( const RectI& rect, const LambdaSlot * pSlot )
	{
		if (!pSlot->m_bVisible)
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add(RectI(rect, RectI(0, 0, m_size)));

		// Remove portions of patches that are covered by opaque upper siblings

		for (LambdaSlot * pCover = slots._begin(); pCover < pSlot ; pCover++)
		{
			if (pCover->m_bVisible && pCover->m_geo.intersectsWith(rect))
				pCover->_widget()->_maskPatches(patches, pCover->m_geo, RectI(0, 0, 65536, 65536), _getBlendMode());
		}

		// Make request render calls

		for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
			_requestRender(*pRect);

	}



} // namespace wg
