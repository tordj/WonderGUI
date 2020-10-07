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

			virtual Object* _object() = 0;
		};

		struct Pocket
		{
			SkinSlot*	pSkinSlot;

			State		transitionFromState;
			State		transitionToState;
			int			transitionMillisec[StateBits_Nb];

			float		transitionProgress[StateBits_Nb];
		};

		SkinSlot(Holder* pHolder) : m_pHolder(pHolder) {}
		~SkinSlot() { if (m_pSkin) m_pSkin->_incUseCount(); }


		// Operators for the holder

		inline Skin* operator->() const { return m_pSkin; }
		inline operator Skin*() const { return m_pSkin; }
		inline operator Skin_p() const { return m_pSkin; }

		inline SkinSlot& operator=(Skin* pSkin) { _setSkin(pSkin); return *this; }

		//



	protected:

		void _setSkin(Skin* pSkin);
		void _changeState(State newState, State oldState);
		bool _update(int msPassed);

		void _initPocket();
		void _releasePocket();

		Skin_p	m_pSkin;
		Holder* m_pHolder;
		Pocket* m_pPocket = nullptr;
	};
};

#endif //WG_SKINSLOT_DOT_H