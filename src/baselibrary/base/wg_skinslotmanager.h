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
#ifndef WG_SKINSLOTMANAGER_DOT_H
#define WG_SKINSLOTMANAGER_DOT_H
#pragma once

#include <wg_mempool.h>

#include <vector>

namespace wg
{



	struct SkinSlotPocket
	{
		class Holder
		{
		public:
			virtual bool _update(SkinSlotPocket* pPocket, int msPassed) = 0;
			virtual void _willRemovePocket(SkinSlotPocket* pPocket) = 0;
		};

		Holder*				pHolder;
		bool				bAnimated;		// Set if skin is animated beyond transitions.
		int					animationCounter;	// Count animation progress in millisec.

		Bitmask<uint8_t>	transitionFrom;	// Bitmask representation of state we transition from.
		Bitmask<uint8_t>	transitionTo;	// Bitmask representation of tstate we transition to.
		float				fractionalState[StateBits_Nb];
		void* instance = 0;
	};

	class SkinSlotManager
	{

	public:


		static void init();
		static void exit();

		static void update(int msPassed);

		static SkinSlotPocket* allocPocket();
		static void freePocket(SkinSlotPocket* pEntry);


	private:


		static MemPool* s_pMemPool;

		static std::vector<SkinSlotPocket*>	s_slotPockets;

	};
};


#endif 
