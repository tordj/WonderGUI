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

#include <wg_skinslotmanager.h>
#include <assert.h>

namespace wg
{
	MemPool* SkinSlotManager::s_pMemPool = nullptr;
	std::vector<SkinSlot::Pocket*>	SkinSlotManager::s_slotPockets;

	//____ init() _____________________________________________________________

	void SkinSlotManager::init()
	{
		s_pMemPool = new MemPool(16, sizeof(SkinSlot::Pocket));
	}

	//____ exit() _____________________________________________________________

	void SkinSlotManager::exit()
	{
		assert(s_pMemPool->entriesAllocated() == 0);
		delete s_pMemPool;
	}

	//____ update() ___________________________________________________________

	void SkinSlotManager::update(int msPassed)
	{
		std::remove_if(s_slotPockets.begin(),
			s_slotPockets.end(),
			[msPassed](SkinSlot::Pocket* pPocket) 
			{
				bool bDelete = pPocket->pSkinSlot->_update(msPassed); 
				if (bDelete) 
					s_pMemPool->freeEntry(pPocket); 
				return bDelete; 
			}
		);
	}

	//____ allocPocket() ______________________________________________________

	SkinSlot::Pocket* SkinSlotManager::allocPocket()
	{
		auto pBuff = s_pMemPool->allocEntry();

		auto pPocket = new(pBuff) SkinSlot::Pocket();

		s_slotPockets.push_back(pPocket);
		return pPocket;
	}

	//____ freePocket() _______________________________________________________

	void SkinSlotManager::freePocket(SkinSlot::Pocket* pPocket)
	{
		s_pMemPool->freeEntry(pPocket);

		auto it = std::find(s_slotPockets.begin(), s_slotPockets.end(), pPocket);
		s_slotPockets.erase(it);
	}
}