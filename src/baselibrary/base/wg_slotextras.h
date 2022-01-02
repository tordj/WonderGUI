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

#ifndef WG_SLOTEXTRAS_DOT_H
#define WG_SLOTEXTRAS_DOT_H
#pragma once

#include <wg_slotiterator.h>
#include <wg_geo.h>


namespace wg
{
	template<class SlotType, class HolderType> class PaddedSlotCollectionMethods
	{
	public:

		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(const SlotArrayIterator<SlotType>& beg, const SlotArrayIterator<SlotType>& end, Border padding);
		bool		setPadding(int index, int amount, std::initializer_list<Border> padding);
		bool		setPadding(const SlotArrayIterator<SlotType>& beg, const SlotArrayIterator<SlotType>& end, std::initializer_list<Border> padding);

	protected:

		virtual HolderType* _holder() = 0;

		virtual SlotType* _slot(int index) = 0;

//		virtual SlotType* _begin() const = 0;
//		virtual SlotType* _end() const = 0;
	};

	template<class SlotType, class HolderType> class SelectableSlotCollectionMethods
	{
	public:

		void	select(int index, int amount);
		void	select(const  SlotArrayIterator<SlotType>& beg, const  SlotArrayIterator<SlotType>& end);

		void	unselect(int index, int amount);
		void	unselect(const  SlotArrayIterator<SlotType>& beg, const  SlotArrayIterator<SlotType>& end);

		void	selectAll();
		void	unselectAll();
	protected:

		virtual HolderType* _holder() = 0;

		virtual SlotType*	_slot(int index) = 0;
		virtual int			_size() const = 0;

//		virtual SlotType* _begin() const = 0;
//		virtual SlotType* _end() const = 0;
	};



}



#endif //WG_SLOTEXTRAS_DOT_H
