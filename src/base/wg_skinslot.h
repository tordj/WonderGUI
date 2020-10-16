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

namespace wg
{



	class SkinSlot
	{
		friend class SkinSlotManager;
	public:
		class Holder
		{
		public:
			virtual void	_skinRequestRender(const SkinSlot* pSlot) = 0;
			virtual void	_skinRequestRender(const SkinSlot* pSlot, const Rect& rect) = 0;

			virtual Size	_skinSize(const SkinSlot* pSlot) const = 0;
			virtual Coord	_skinGlobalPos(const SkinSlot* pSlot) const = 0;

			virtual State	_skinState(const SkinSlot* pSlot) const = 0;
			virtual float	_skinValue(const SkinSlot* pSlot) const = 0;
			virtual float	_skinValue2(const SkinSlot* pSlot) const = 0;


//			virtual Object* _object() = 0;
		};

		struct Pocket
		{
			SkinSlot*			pSkinSlot;
			bool				bAnimated;		// Set if skin is animated beyond transitions.
			int					animationCounter;	// Count animation progress in millisec.

			Bitmask<uint8_t>	transitionFrom;	// Bitmask representation of state we transition from.
			Bitmask<uint8_t>	transitionTo;	// Bitmask representation of tstate we transition to.
			float				fractionalState[StateBits_Nb];
		};

		SkinSlot(Holder* pHolder) : m_pHolder(pHolder) {}
		~SkinSlot() { if (m_pSkin) m_pSkin->_incUseCount(); }


		// Operators for the holder

//		inline Skin* operator->() const { return m_pSkin; }
//		inline operator Skin*() const { return m_pSkin; }

//		inline SkinSlot& operator=(Skin* pSkin) { setSkin(pSkin); return *this; }
//		inline operator Skin_p() const { return m_pSkin; }



		void			setSkin(Skin* pSkin);
		inline Skin_p	skin() const { return m_pSkin; }
		inline bool		isEmpty() const { return m_pSkin == nullptr; }

		void			stateChanged(State newState, State oldState);
		void			valueChanged(float newValue, float oldValue, float newValue2 = -1, float oldValue2 = -1);

		void 			render(GfxDevice* pDevice, const Rect& canvas, State state, float value = 1.f, float value2 = -1.f ) const;

		inline Size		minSize() const { return m_pSkin ? m_pSkin->minSize() : Size(); }
		inline Size		preferredSize() const { return m_pSkin ? m_pSkin->preferredSize() : Size(); }

		inline Size		contentPaddingSize() const { return m_pSkin ? m_pSkin->contentPaddingSize() : Size(); }
		inline Size		sizeForContent(const Size& contentSize) const { return m_pSkin ? m_pSkin->sizeForContent(contentSize) : contentSize; }
		inline Coord	contentOfs(State state) const { return m_pSkin ? m_pSkin->contentOfs(state) : Coord(); }
		inline Rect		contentRect(const Rect& canvas, State state) const { return m_pSkin ? m_pSkin->contentRect(canvas,state): canvas; }

		inline bool		isOpaque() const { return m_pSkin ? m_pSkin->isOpaque() : false; }
		inline bool		isOpaque(State state) const { return m_pSkin ? m_pSkin->isOpaque(state) : false; }
		inline bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const 
						{ 
							return m_pSkin ? m_pSkin->isOpaque(rect, canvasSize, state) : false; 
						}

		inline bool		markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold,
									float value = 1.f, float value2 = -1.f) const 
						{
							return m_pSkin ? m_pSkin->markTest(ofs, canvas, state, opacityTreshold, value, value2) : false;
						}

		inline bool		isContentShifting() { return m_pSkin ? m_pSkin->isContentShifting() : false; }

		//



	protected:

		bool	_update(int msPassed);

		void	_initPocket(State state);
		void	_releasePocket();

		Skin_p	m_pSkin;
		Holder* m_pHolder;
		Pocket* m_pPocket = nullptr;
	};
};

#endif //WG_SKINSLOT_DOT_H