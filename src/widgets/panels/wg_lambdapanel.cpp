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

#include <wg_slotarray.impl.h>
#include <wg_cslotarray.impl.h>
#include <assert.h>


namespace wg
{
	using namespace Util;

	template class CSlotArray<LambdaSlot>;
	template class SlotArray<LambdaSlot>;


	const char LambdaPanel::CLASSNAME[] = {"LambdaPanel"};

	//____ CLambdaSlotArray::add() ________________________________________________

	CLambdaSlotArray::iterator CLambdaSlotArray::add( Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		LambdaSlot * pSlot = m_pSlotArray->add();
		pSlot->_setWidget(pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ CLambdaSlotArray::insert() ________________________________________________

	CLambdaSlotArray::iterator CLambdaSlotArray::insert( int index, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaSlot * pSlot = m_pSlotArray->insert(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	CLambdaSlotArray::iterator CLambdaSlotArray::insert( iterator pos, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaSlot * pSlot = m_pSlotArray->insert(pos._slot());
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ CLambdaSlotArray::setFunction() ________________________________________________

	void CLambdaSlotArray::setFunction(int index, std::function<Rect(Widget * pWidget, Size parentSize)> func)
	{
		//TODO: Assert

		LambdaSlot * pSlot = m_pSlotArray->slot(index);
		pSlot->pFunc = func;

		_holder()->_updateSlotGeo(pSlot, 1);
	}

	void CLambdaSlotArray::setFunction(iterator pos, std::function<Rect(Widget * pWidget, Size parentSize)> func)
	{
		//TODO: Assert

		LambdaSlot * pSlot = pos._slot();
		pSlot->pFunc = func;

		_holder()->_updateSlotGeo(pSlot, 1);
	}

	//____ CLambdaSlotArray::function() ________________________________________________

	std::function<Rect(Widget * pWidget, Size parentSize)> CLambdaSlotArray::function(int index) const
	{
		//TODO: Assert

		LambdaSlot * pSlot = m_pSlotArray->slot(index);
		return pSlot->pFunc;
	}

	std::function<Rect(Widget * pWidget, Size parentSize)> CLambdaSlotArray::function(iterator pos) const
	{
		//TODO: Assert

		LambdaSlot * pSlot = pos._slot();
		return pSlot->pFunc;
	}



	//____ Constructor ____________________________________________________________

	LambdaPanel::LambdaPanel() : m_children(this), children(&m_children,this), m_minSize(0,0), m_preferredSize(512*4,512*4), m_maxSize(INT_MAX,INT_MAX)
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
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->_widget();
	}

	//____ _lastChild() ________________________________________________________

	Widget * LambdaPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->_widget();
	}

	//____ _firstSlotWithGeo() _________________________________________________

	void LambdaPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			LambdaSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}

	}

	//____ _nextSlotWithGeo() _________________________________________________

	void LambdaPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		LambdaSlot * pSlot = (LambdaSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void LambdaPanel::_didAddSlots( BasicSlot * pSlot, int nb )
	{
		_unhideSlots(static_cast<LambdaSlot*>(pSlot), nb);
	}

	//____ _willRemoveSlots() _________________________________________________

	void LambdaPanel::_willRemoveSlots( BasicSlot * pSlot, int nb )
	{
		_hideSlots(static_cast<LambdaSlot*>(pSlot), nb);
	}

	//____ _hideSlots() __________________________________________________________

	void LambdaPanel::_hideSlots( BasicSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == true )
			{
				_onRequestRender(pSlot[i].geo, pSlot);
				pSlot[i].bVisible = false;					// Needs to be done AFTER _onRequestRender()!
			}
		}
	}

	//____ _unhideSlots() ________________________________________________________

	void LambdaPanel::_unhideSlots( BasicSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == false )
			{
				pSlot[i].bVisible = true;
				_updateGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].geo, pSlot);
			}
		}
	}


	//____ _updateSlotGeo() ________________________________________________________

	void LambdaPanel::_updateSlotGeo(BasicSlot * _pSlot, int nb)
	{
		auto pSlot = static_cast<LambdaSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
		{
			_updateGeo(&pSlot[i]);
			if (pSlot[i].bVisible == false)
				_onRequestRender(pSlot[i].geo, pSlot);		//TODO: This looks weird...
		}
	}



	//____ _didMoveSlots() ________________________________________________________

	void LambdaPanel::_didMoveSlots(BasicSlot * _pFrom, BasicSlot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		auto pFrom = static_cast<LambdaSlot*>(_pFrom);
		auto pTo = static_cast<LambdaSlot*>(_pTo);

		if (pTo->bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				LambdaSlot * p = pTo+1;
				while (p <= pFrom)
				{
					RectI cover(pTo->geo, p->geo);

					if (p->bVisible && !cover.isEmpty())
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
					RectI cover(pTo->geo, p->geo);

					if (p->bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}


	//____ _childPos() __________________________________________________________

	CoordI LambdaPanel::_childPos( const BasicSlot * pSlot ) const
	{
		return ((LambdaSlot*)pSlot)->geo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void LambdaPanel::_childRequestRender( BasicSlot * _pSlot )
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);
		_onRequestRender( pSlot->geo, pSlot );
	}

	void LambdaPanel::_childRequestRender( BasicSlot * _pSlot, const RectI& rect )
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->geo.pos(), pSlot );
	}

	//____ _childRequestResize() ______________________________________________

	void LambdaPanel::_childRequestResize( BasicSlot * pSlot )
	{
		_updateGeo(static_cast<LambdaSlot*>(pSlot), true);
	}

	//____ _prevChild() __________________________________________________________

	Widget * LambdaPanel::_prevChild( const BasicSlot * _pSlot ) const
	{
		const LambdaSlot * pSlot = static_cast<const LambdaSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1]._widget();

		return nullptr;

	}

	//____ _nextChild() __________________________________________________________

	Widget * LambdaPanel::_nextChild( const BasicSlot * _pSlot ) const
	{
		const LambdaSlot * pSlot = static_cast<const LambdaSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void LambdaPanel::_releaseChild(BasicSlot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		m_children.remove(static_cast<LambdaSlot*>(pSlot));
	}

	//____ _replaceChild() ______________________________________________________

	void LambdaPanel::_replaceChild(BasicSlot * _pSlot, Widget * pNewChild)
	{
		LambdaSlot * pSlot = static_cast<LambdaSlot*>(_pSlot);

		children._releaseGuardPointer(pNewChild, &pSlot);
		pSlot->_setWidget(pNewChild);

   		if (pSlot->bVisible)
		{
			_updateGeo(pSlot);
			_onRequestRender(pSlot->geo, pSlot);
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

		for (auto& slot : m_children)
			_updateGeo(&slot);
	}

	//____ _updateGeo() _______________________________________________________

	void LambdaPanel::_updateGeo(LambdaSlot * pSlot, bool bForceResize )
	{
		//TODO: Don't requestRender if slot is hidden.

		RectI geo;
			
		if (pSlot->pFunc)
			geo = pixelAligned(qpixToRaw(pSlot->pFunc(pSlot->_widget(), rawToQpix(m_size))));
		else
			geo = { 0,0,pSlot->_preferredSize() };

		if (geo != pSlot->geo)
		{
			if (pSlot->bVisible)
			{
				// Clip our geometry and put it in a dirtyrect-list

				Patches patches;
				patches.add(RectI(pSlot->geo, RectI(0, 0, m_size)));
				patches.add(RectI(geo, RectI(0, 0, m_size)));

				// Remove portions of patches that are covered by opaque upper siblings

				const LambdaSlot * pCover = pSlot + 1;
				while (pCover < m_children.end())
				{
					if (pCover->bVisible && (pCover->geo.intersectsWith(pSlot->geo) || pCover->geo.intersectsWith(geo)) )
						pCover->_widget()->_maskPatches(patches, pCover->geo, RectI(0, 0, 65536, 65536), _getBlendMode());

					pCover++;
				}

				// Make request render calls

				for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
					_requestRender(*pRect);

			}
		}

		pSlot->geo = geo;

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _onRequestRender() ____________________________________________________

	void LambdaPanel::_onRequestRender( const RectI& rect, const LambdaSlot * pSlot )
	{
		if (!pSlot->bVisible)
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add(RectI(rect, RectI(0, 0, m_size)));

		// Remove portions of patches that are covered by opaque upper siblings

		for (LambdaSlot * pCover = m_children.begin(); pCover < pSlot ; pCover++)
		{
			if (pCover->bVisible && pCover->geo.intersectsWith(rect))
				pCover->_widget()->_maskPatches(patches, pCover->geo, RectI(0, 0, 65536, 65536), _getBlendMode());
		}

		// Make request render calls

		for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
			_requestRender(*pRect);

	}



} // namespace wg
