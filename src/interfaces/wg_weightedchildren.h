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

#ifndef	WG_WEIGHTEDCHILDREN_DOT_H
#define	WG_WEIGHTEDCHILDREN_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_paddedchildren.h>

namespace wg
{

	//____ WeightedChildrenHolder ____________________________________________________________

	class WeightedChildrenHolder : public PaddedChildrenHolder		/** @private */
	{
	public:
		virtual void		_reweightSlots(Slot * pSlot, int nb, float weight) = 0;
		virtual void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights) = 0;
		virtual void		_refreshChildGeo() = 0;
	};


	//____ WeightedChildren ________________________________________________________

	template<class SlotType, class HolderType> class WeightedChildren : public PaddedChildren<SlotType, HolderType>
	{
	public:
		using		iterator = ChildIterator<SlotType>;
		using		Children<SlotType, HolderType>::m_pSlotArray;
		using		Children<SlotType, HolderType>::m_pHolder;

		/** @private */

		WeightedChildren(SlotArray<SlotType> * pSlotArray, HolderType * pHolder) : PaddedChildren<SlotType, HolderType>(pSlotArray, pHolder) {}

		//.____ Geometry _______________________________________________________

		bool		setWeight(int index, float weight);
		bool		setWeight(iterator it, float weight);
		bool		setWeight(int index, int amount, float weight);
		bool		setWeight(iterator beg, iterator end, float weight);
		bool		setWeight(int index, int amount, const std::initializer_list<float> weights);
		bool		setWeight(iterator beg, iterator end, const std::initializer_list<float> weights);

		float		weight(int index) const;
		float		weight(iterator it) const;
	};
};

#endif	//WG_WEIGHTEDCHILDREN_DOT_H
