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

#ifndef	WG_IHIDEABLECHILDREN_DOT_H
#define	WG_IHIDEABLECHILDREN_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_idynamicchildren.h>

namespace wg
{

	//____ HideableChildrenHolder ____________________________________________________________

	class HideableChildrenHolder : public DynamicChildrenHolder		/** @private */
	{
	public:
		virtual void	_hideSlots(Slot * pSlot, int nb) = 0;
		virtual void	_unhideSlots(Slot * pSlot, int nb) = 0;
	};


	//____ IHideableChildren ________________________________________________________

	template<class SlotType, class HolderType> class IHideableChildren : public IDynamicChildren<SlotType,HolderType>
	{
	public:
		using		iterator = ChildIteratorSubclass<SlotType>;
		using		IChildrenSubclass<SlotType,HolderType>::m_pSlotArray;
		using		IChildrenSubclass<SlotType,HolderType>::m_pHolder;

		/** @private */

		IHideableChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : IDynamicChildren<SlotType,HolderType>(pSlotArray, pHolder) {}

		//.____ Appearance ____________________________________________________

		void	hide( int index );
		void	hide( iterator it );
		void	hide( int index, int amount );
		void	hide( iterator beg, iterator end );

		void	unhide( int index );
		void	unhide( iterator it );
		void	unhide( int index, int amount );
		void	unhide( iterator beg, iterator end );

		void	hideAll();
		void	unhideAll();

		bool	isVisible( int index );
		bool	isVisible( iterator it );
	};

};

#endif	//WG_IHIDEABLECHILDREN_DOT_H
