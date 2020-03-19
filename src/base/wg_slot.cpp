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

#include <wg_slot.h>
#include <wg_paddedslot.h>
#include <wg_selectableslot.h>

namespace wg
{
	const TypeInfo	StaticSlot::TYPEINFO = { "StaticSlot", nullptr };

	const TypeInfo	DynamicSlot::TYPEINFO = { "DynamicSlot", &StaticSlot::TYPEINFO };

	const TypeInfo	PaddedSlot::TYPEINFO = { "PaddedSlot", &DynamicSlot::TYPEINFO };

	const TypeInfo	SelectableSlot::TYPEINFO = { "SelectableSlot", &DynamicSlot::TYPEINFO };
}