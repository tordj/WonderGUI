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

#ifndef	WG_CSKINSLOT_DOT_H
#define	WG_CSKINSLOT_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_skinslotmanager.h>
#include <wg_geocomponent.h>

namespace wg
{

	class CSkinSlot;
	typedef	StrongComponentPtr<CSkinSlot>	CSkinSlot_p;
	typedef	WeakComponentPtr<CSkinSlot>	CSkinSlot_wp;


	class CSkinSlot : public GeoComponent, protected SkinSlotPocket::Holder
	{
		friend class CSkinSlotManager;
	public:
		class Holder : public GeoComponent::Holder
		{
		public:
			virtual void	_skinChanged(const CSkinSlot* pSlot, Skin* pNewSkin, Skin* pOldSkin) = 0;
			virtual float	_skinValue(const CSkinSlot* pSlot) const = 0;
			virtual float	_skinValue2(const CSkinSlot* pSlot) const = 0;
		};

		CSkinSlot(Holder* pHolder) : GeoComponent(pHolder) {}
		~CSkinSlot() { if (m_pSkin) m_pSkin->_decUseCount(); if (m_pPocket) SkinSlotManager::freePocket(m_pPocket); }

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________

		void			set(Skin* pSkin);
		inline bool		isEmpty() const { return m_pSkin == nullptr; }

		//.____ Misc __________________________________________________________

		inline CSkinSlot_p  ptr() { return CSkinSlot_p(this); }

		//.____ Operators __________________________________________

		inline const Skin* operator->() const { return m_pSkin; }
		inline operator Skin*() const { return m_pSkin; }

		inline CSkinSlot& operator=(Skin* pSkin) { set(pSkin); return *this; }
		inline operator Skin_p() const { return m_pSkin; }

		//

	protected:

		// Methods for holder

		void			_stateChanged(State newState, State oldState);
		void			_valueChanged(float newValue, float oldValue, float newValue2 = -1, float oldValue2 = -1);

		void 			_render(GfxDevice* pDevice, const Rect& canvas, State state, float value = 1.f, float value2 = -1.f) const;

		inline Size		_minSize() const { return m_pSkin ? m_pSkin->minSize() : Size(); }
		inline Size		_preferredSize() const { return m_pSkin ? m_pSkin->preferredSize() : Size(); }

		inline Size		_contentPaddingSize() const { return m_pSkin ? m_pSkin->contentPaddingSize() : Size(); }
		inline Size		_sizeForContent(const Size& contentSize) const { return m_pSkin ? m_pSkin->sizeForContent(contentSize) : contentSize; }
		inline Coord	_contentOfs(State state) const { return m_pSkin ? m_pSkin->contentOfs(state) : Coord(); }
		inline Rect		_contentRect(const Rect& canvas, State state) const { return m_pSkin ? m_pSkin->contentRect(canvas, state) : canvas; }

		inline bool		_isOpaque() const { return m_pSkin ? m_pSkin->isOpaque() : false; }
		inline bool		_isOpaque(State state) const { return m_pSkin ? m_pSkin->isOpaque(state) : false; }
		inline bool		_isOpaque(const Rect& rect, const Size& canvasSize, State state) const
		{
			return m_pSkin ? m_pSkin->isOpaque(rect, canvasSize, state) : false;
		}

		inline bool		_markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold,
			float value = 1.f, float value2 = -1.f) const
		{
			return m_pSkin ? m_pSkin->markTest(ofs, canvas, state, opacityTreshold, value, value2) : false;
		}

		inline bool		_isContentShifting() { return m_pSkin ? m_pSkin->isContentShifting() : false; }

		//


		bool	_update(SkinSlotPocket* pPocket, int msPassed);

		void	_initPocket(State state);
		void	_releasePocket();

		Skin_p	m_pSkin;
		SkinSlotPocket* m_pPocket = nullptr;
	};



	class OCSkinSlot : public CSkinSlot
	{
	public:
		using CSkinSlot::_stateChanged;
		using CSkinSlot::_valueChanged;
		using CSkinSlot::_render;
		using CSkinSlot::_minSize;
		using CSkinSlot::_preferredSize;
		using CSkinSlot::_contentPaddingSize;
		using CSkinSlot::_sizeForContent;
		using CSkinSlot::_contentOfs;
		using CSkinSlot::_contentRect;
		using CSkinSlot::_isOpaque;
		using CSkinSlot::_markTest;
		using CSkinSlot::_isContentShifting;
	};

	inline OCSkinSlot& OO(CSkinSlot& skin) { return reinterpret_cast<OCSkinSlot&>(skin); }
	inline const OCSkinSlot& OO(const CSkinSlot& skin) { return reinterpret_cast<const OCSkinSlot&>(skin); }



};

#endif //WG_CSKINSLOT_DOT_H