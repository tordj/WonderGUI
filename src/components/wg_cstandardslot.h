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
#ifndef WG_CSTANDARDSLOT_DOT_H
#define WG_CSTANDARDSLOT_DOT_H
#pragma once

#include <wg_cslot.h>

namespace wg
{
	class CStandardSlot;
	typedef	StrongComponentPtr<CStandardSlot>	CStandardSlot_p;
	typedef	WeakComponentPtr<CStandardSlot>		CStandardSlot_wp;

	class CStandardSlot : public CDynamicSlotImpl<DynamicSlot>
	{
	public:
		using CDynamicSlotImpl<DynamicSlot>::operator=;

		/** @private */
		CStandardSlot(SlotHolder * pHolder) : CDynamicSlotImpl(pHolder) {}

		//.____ Misc __________________________________________________________

		inline CStandardSlot_p  ptr() { return CStandardSlot_p(this); }

	};



}

#endif //WG_CDYNAMICSLOT_DOT_H
