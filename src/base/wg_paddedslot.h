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

	class PaddedSlot : public DynamicSlot 	/** @private */
	{
		template<class S> friend class CDynamicSlotArray;
		template<class S> friend class CPaddedSlotArray;
	public:

		//____ Holder _________________________________________________________

		class Holder : public SlotHolder
		{
		};

		PaddedSlot(Holder* pHolder) : DynamicSlot(pHolder), bVisible(false) {}
	protected:
		SizeI		_paddedPreferredSize() const { return _preferredSize() + padding; }
		SizeI		_paddedMinSize() const { return _minSize() + padding; }
		SizeI		_paddedMaxSize() const { return _maxSize() + padding; }
		int			_paddedMatchingWidth(int paddedHeight) const { return _matchingWidth(paddedHeight - padding.height()) + padding.width(); }
		int			_paddedMatchingHeight(int paddedWidth) const { return _matchingHeight(paddedWidth - padding.width()) + padding.height(); }

		BorderI		padding;
		bool		bVisible;
	};
}
#endif //WG_PADDEDSLOT_DOT_H
