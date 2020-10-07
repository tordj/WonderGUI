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
#include <wg_skinslot.h>

#include <vector>

namespace wg
{



	class SkinSlotManager
	{
	public:
		static void init();
		static void exit();

		static void update(int msPassed);

		static SkinSlot::Pocket* allocPocket();
		static void freePocket(SkinSlot::Pocket* pEntry);


	private:

		static MemPool* s_pMemPool;

		static std::vector<SkinSlot::Pocket*>	s_slotPockets;

	};
};


#endif 