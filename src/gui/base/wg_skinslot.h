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

#ifndef	WG_SKINSLOT_DOT_H
#define	WG_SKINSLOT_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_skinslotmanager.h>

namespace wg
{


	class SkinSlot : protected SkinSlotPocket::Holder
	{
		friend class SkinSlotManager;
	public:
		class Holder
		{
		public:
			virtual float	_skinValue(const SkinSlot* pSlot) const = 0;
			virtual float	_skinValue2(const SkinSlot* pSlot) const = 0;
			virtual State	_skinState(const SkinSlot* pSlot) const = 0;

			virtual SizeSPX	_skinSize(const SkinSlot* pSlot) const = 0;

			virtual void	_skinRequestRender(const SkinSlot* pSlot) = 0;
			virtual void	_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect) = 0;

			virtual int		_scale() const = 0;
		};

		SkinSlot(Holder* pHolder) : m_pHolder(pHolder) {}
		~SkinSlot() { if (m_pSkin) m_pSkin->_decUseCount(); if (m_pPocket) SkinSlotManager::freePocket(m_pPocket); }


		void			set(Skin* pSkin);
		inline Skin*	get() const { return m_pSkin; }
		inline bool		isEmpty() const { return m_pSkin == nullptr; }

		//


		void			stateChanged(State newState, State oldState);
		void			valueChanged(float newValue, float oldValue, float newValue2 = -1, float oldValue2 = -1);

		void 			render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value = 1.f, float value2 = -1.f) const;

		inline SizeSPX	minSize(int scale) const { return m_pSkin ? m_pSkin->_minSize(scale) : SizeSPX(); }
		inline SizeSPX	defaultSize(int scale) const { return m_pSkin ? m_pSkin->_defaultSize(scale) : SizeSPX(); }

		inline SizeSPX	contentPaddingSize(int scale) const { return m_pSkin ? m_pSkin->_contentPaddingSize(scale) : SizeSPX(); }
		inline SizeSPX	sizeForContent(const SizeSPX& contentSize, int scale) const { return m_pSkin ? m_pSkin->_sizeForContent(contentSize, scale) : contentSize; }
		inline CoordSPX	contentOfs(int scale, State state) const { return m_pSkin ? m_pSkin->_contentOfs(scale, state) : CoordSPX(); }
		inline RectSPX	contentRect(const RectSPX& canvas, int scale, State state) const { return m_pSkin ? m_pSkin->_contentRect(canvas, scale, state) : canvas; }

		inline bool		isOpaque() const { return m_pSkin ? m_pSkin->_isOpaque() : false; }
		inline bool		isOpaque(State state) const { return m_pSkin ? m_pSkin->_isOpaque(state) : false; }
		inline bool		isOpaque(const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state) const
		{
			return m_pSkin ? m_pSkin->_isOpaque(rect, canvasSize, scale, state) : false;
		}

		inline bool		markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state,
								float value = 1.f, float value2 = -1.f) const
		{
			return m_pSkin ? m_pSkin->_markTest(ofs, canvas, scale, state, value, value2) : false;
		}

		inline bool		isContentShifting() { return m_pSkin ? m_pSkin->_isContentShifting() : false; }


	protected:

		//


		bool	_update(SkinSlotPocket* pPocket, int msPassed) override;
		void	_willRemovePocket(SkinSlotPocket* pPocket) override;


		void	_initPocket(State state);
		void	_releasePocket();

		Skin_p	m_pSkin;
		SkinSlotPocket* m_pPocket = nullptr;
		Holder* m_pHolder;
	};

};

#endif //WG_SKINSLOT_DOT_H