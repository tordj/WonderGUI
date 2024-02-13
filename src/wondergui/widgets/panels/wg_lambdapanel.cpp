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

#include <wg_dynamicslotvector.impl.h>
#include <wg_panel.impl.h>
#include <assert.h>


namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<LambdaPanelSlot>;
	template class Panel<LambdaPanelSlot>;


	const TypeInfo LambdaPanel::TYPEINFO = { "LambdaPanel", &Panel::TYPEINFO };
	const TypeInfo LambdaPanelSlot::TYPEINFO = { "LambdaPanelSlot", &DynamicSlot::TYPEINFO };

//____ LambdaPanelSlot::setFunction() ____________________________________________________

	void LambdaPanelSlot::setFunction(const std::function<Rect(Widget * pWidget, Size parentSize)>& func)
	{
		m_func = func;
		static_cast<LambdaPanel*>(_holder())->_updateSlotGeo(this, 1);
	}

	//____ LambdaPanelSlot::_setBlueprint() ____________________________________________________

	bool LambdaPanelSlot::_setBlueprint( const Blueprint& bp )
	{
		m_func = bp.func;
		return true;
	}

	//____ constructor ____________________________________________________________

	LambdaPanel::LambdaPanel() : m_minSize(0,0), m_defaultSize(512,512), m_maxSize(16000000, 16000000)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	LambdaPanel::~LambdaPanel()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& LambdaPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setMinSize() _________________________________________________________________

	bool LambdaPanel::setMinSize(Size min)
	{
		//TODO: Assert >= 0.

		if( m_minSize != min )
		{
			if( min.w > m_maxSize.w || min.h > m_maxSize.h )
				return false;

			m_minSize = min;
			if( m_defaultSize.w < min.w )
				m_defaultSize.w = min.w;
			if( m_defaultSize.h < min.h )
				m_defaultSize.h = min.h;

			_requestResize();
		}
		return true;
	}

	//____ setMaxSize() ________________________________________________________

	bool LambdaPanel::setMaxSize(Size max)
	{
		//TODO: Assert >= 0.


 		if( m_maxSize != max )
		{
			if( max.w < m_minSize.w || max.h < m_minSize.h )
				return false;

			m_maxSize = max;
			if( m_defaultSize.w > max.w )
				m_defaultSize.w = max.w;
			if( m_defaultSize.h > max.h )
				m_defaultSize.h = max.h;

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

		m_minSize = min;
		m_maxSize = max;
		limit( m_defaultSize.w, m_minSize.w, m_maxSize.w );
		limit( m_defaultSize.h, m_minSize.h, m_maxSize.h );

		_requestResize();
		return true;
	}

	//____ setDefaultSize() __________________________________________________

	bool LambdaPanel::setDefaultSize(Size defaultSize)
	{
		//TODO: Assert >= 0.

		if(defaultSize.w > m_maxSize.w || defaultSize.h > m_maxSize.h || defaultSize.w < m_minSize.w || defaultSize.h < m_minSize.h )
			return false;

		m_defaultSize = defaultSize;
		_requestResize();
		return true;
	}

	//____ _defaultSize() _____________________________________________________

	SizeSPX LambdaPanel::_defaultSize(int scale) const
	{
		return SizeSPX(m_defaultSize*scale);
	}

	//____ _minSize() _____________________________________________________

	SizeSPX LambdaPanel::_minSize(int scale) const
	{
		return SizeSPX(m_minSize * scale);
	}

	//____ _maxSize() _____________________________________________________

	SizeSPX LambdaPanel::_maxSize(int scale) const
	{
		return SizeSPX(m_maxSize * scale);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	LambdaPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return LambdaPanelSlot::TYPEINFO;
	}

	//____ _didAddSlots() ________________________________________________________

	void LambdaPanel::_didAddSlots( StaticSlot * pSlot, int nb )
	{
		_unhideSlots(static_cast<LambdaPanelSlot*>(pSlot), nb);
	}

	//____ _willEraseSlots() _________________________________________________

	void LambdaPanel::_willEraseSlots( StaticSlot * pSlot, int nb )
	{
		_hideSlots(static_cast<LambdaPanelSlot*>(pSlot), nb);
	}

	//____ _hideSlots() __________________________________________________________

	void LambdaPanel::_hideSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

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
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

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
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

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

		auto pFrom = static_cast<LambdaPanelSlot*>(_pFrom);
		auto pTo = static_cast<LambdaPanelSlot*>(_pTo);

		if (pTo->m_bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				auto p = pTo+1;
				while (p <= pFrom)
				{
					RectSPX cover = RectSPX::overlap(pTo->m_geo, p->m_geo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, pTo);
					p++;
				}
			}
			else							// Move us backward
			{
				// Request render on our siblings for the area we previously have covered.

				auto p = pFrom;
				while (p < pTo)
				{
					RectSPX cover = RectSPX::overlap(pTo->m_geo, p->m_geo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}

	//____ _childRequestRender() _________________________________________________

	void LambdaPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( pSlot->m_geo, pSlot );
	}

	void LambdaPanel::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
	{
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->m_geo.pos(), pSlot );
	}

	//____ _childRequestResize() ______________________________________________

	void LambdaPanel::_childRequestResize( StaticSlot * pSlot )
	{
		_updateGeo(static_cast<LambdaPanelSlot*>(pSlot), true);
	}

	//____ _releaseChild() ____________________________________________________

	void LambdaPanel::_releaseChild(StaticSlot * pSlot)
	{
		slots.erase(static_cast<LambdaPanelSlot*>(pSlot));
	}

	//____ _replaceChild() ______________________________________________________

	void LambdaPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewChild)
	{
		if( pNewChild == nullptr )
		{
			Base::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Slot in LambdaPanel must contain pointer to widget and not nullptr.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}
		
		auto pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

		slots._releaseGuardPointer(pNewChild, &pSlot);
		pSlot->_setWidget(pNewChild);

   		if (pSlot->m_bVisible)
		{
			_updateGeo(pSlot);
			_onRequestRender(pSlot->m_geo, pSlot);
		}
	}

	//____ _resize() ____________________________________________________________

	void LambdaPanel::_resize( const SizeSPX& size, int scale )
	{
		Panel::_resize( size, scale );

		for (auto pSlot = slots.begin() ; pSlot != slots.end() ; pSlot++)
			_updateGeo(pSlot);
	}

	//____ _updateGeo() _______________________________________________________

	void LambdaPanel::_updateGeo(LambdaPanelSlot * pSlot, bool bForceResize )
	{
		//TODO: Don't requestRender if slot is hidden.

		RectSPX geo;

		if (pSlot->m_func)
		{
			Rect ptsGeo = pSlot->m_func(pSlot->_widget(), spxToPts(m_size,m_scale));

			geo = align(ptsToSpx(ptsGeo,m_scale));
		}
		else
			geo = { 0,0, align(pSlot->_widget()->_defaultSize(m_scale)) };

		if (geo != pSlot->m_geo)
		{
			if (pSlot->m_bVisible)
			{
				// Clip our geometry and put it in a dirtyrect-list

				PatchesSPX patches;
				patches.add(RectSPX::overlap(pSlot->m_geo, RectSPX(0, 0, m_size)));
				patches.add(RectSPX::overlap(geo, RectSPX(0, 0, m_size)));

				// Remove portions of patches that are covered by opaque upper siblings

				auto pCover = pSlot + 1;
				while (pCover < slots.end())
				{
					if (pCover->m_bVisible && (pCover->m_geo.isOverlapping(pSlot->m_geo) || pCover->m_geo.isOverlapping(geo)) )
						pCover->_widget()->_maskPatches(patches, pCover->m_geo, RectSPX(0, 0, 0x7FFFFFC0, 0x7FFFFFC0), _getBlendMode());

					pCover++;
				}

				// Make request render calls

				for (const RectSPX * pRect = patches.begin(); pRect < patches.end(); pRect++)
					_requestRender(*pRect);

			}
		}

		pSlot->m_geo = geo;
		auto pWidget = pSlot->_widget();

		if (bForceResize || pWidget->_size() != geo.size() || pWidget->_scale() != m_scale )
			pSlot->_setSize(geo, m_scale);
	}

	//____ _onRequestRender() ____________________________________________________

	void LambdaPanel::_onRequestRender( const RectSPX& rect, const LambdaPanelSlot * pSlot )
	{
		if (!pSlot->m_bVisible)
			return;

		// Clip our geometry and put it in a dirtyrect-list

		PatchesSPX patches;
		patches.add(RectSPX::overlap(rect, RectSPX(0, 0, m_size)));

		// Remove portions of patches that are covered by opaque upper siblings

		for (auto pCover = slots.begin(); pCover < pSlot ; pCover++)
		{
			if (pCover->m_bVisible && pCover->m_geo.isOverlapping(rect))
				pCover->_widget()->_maskPatches(patches, pCover->m_geo, RectSPX(0, 0, 0x7FFFFFC0, 0x7FFFFFC0), _getBlendMode());
		}

		// Make request render calls

		for (const RectSPX * pRect = patches.begin(); pRect < patches.end(); pRect++)
			_requestRender(*pRect);

	}



} // namespace wg
