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
#ifndef WG_CBASICSLOT_DOT_H
#define WG_CBASICSLOT_DOT_H
#pragma once

#include <wg_cslot.h>

namespace wg
{
	class CBasicSlot : public CSlotImpl<BasicSlot>
	{
	public:
		using CSlotImpl<BasicSlot>::operator=;

		class Holder : public CSlotImpl<BasicSlot>::Holder
		{
		};

		CBasicSlot(Holder * pHolder) : CSlotImpl(pHolder) {}

		//.____ Misc __________________________________________________________

		inline StrongComponentPtr<CBasicSlot>	ptr() { return StrongComponentPtr<CBasicSlot>(this); }

	};

	typedef	StrongComponentPtr<CBasicSlot>	CBasicSlot_p;
	typedef	WeakComponentPtr<CBasicSlot>	CBasicSlot_wp;


}

#endif //WG_CBASICSLOT_DOT_H
