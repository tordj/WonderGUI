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

#ifndef WG_PADDEDSLOT_DOT_H
#define WG_PADDEDSLOT_DOT_H
#pragma once

#include <wg_slot.h>

namespace wg
{


	//____ PaddedSlot ____________________________________________________________

	class PaddedSlot : public Slot 	/** @private */
	{
	public:
		PaddedSlot() : bVisible(false) {}

		SizeI		paddedPreferredSize() const { return pWidget->preferredSize() + padding; }
		SizeI		paddedMinSize() const { return pWidget->minSize() + padding; }
		SizeI		paddedMaxSize() const { return pWidget->maxSize() + padding; }
		int			paddedMatchingWidth(int paddedHeight) const { return pWidget->matchingWidth(paddedHeight - padding.height()) + padding.width(); }
		int			paddedMatchingHeight(int paddedWidth) const { return pWidget->matchingHeight(paddedWidth - padding.width()) + padding.height(); }

		BorderI		padding;
		bool		bVisible;
	};
}
#endif //WG_PADDEDSLOT_DOT_H
