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

#ifndef	WG_HIDEABLECHILDREN_DOT_H
#define	WG_HIDEABLECHILDREN_DOT_H
#pragma once

#include <wg_slot.h>
#include <wg_dynamicchildgroup.h>

namespace wg
{
	
	//____ HideableChildrenHolder ____________________________________________________________

	class HideableChildrenHolder : public DynamicChildGroupHolder		/** @private */
	{
		virtual void	_hideSlots(Slot * pSlot, int nb) = 0;
		virtual void	_unhideSlots(Slot * pSlot, int nb) = 0;
	};
	
	
	//____ HideableChildren ________________________________________________________

	template<class SlotType, class HolderType> class HideableChildren : public DynamicChildGroup<SlotType,HolderType>
	{
	public:
		using		iterator = SlotIterator<SlotType>;
		using		ChildGroup<SlotType,HolderType>::m_pSlotArray;
		using		ChildGroup<SlotType,HolderType>::m_pHolder;
		
		HideableChildren( SlotArray<SlotType> * pSlotArray, HolderType * pHolder ) : DynamicChildGroup<SlotType,HolderType>(pSlotArray, pHolder) {}

		void	hide( int index ) 
		{
			//TODO: Assert
			
			m_pHolder->_hideSlots( m_pSlotArray->slot(index), 1 );
		};

		void	hide( iterator it ) 
		{
			//TODO: Assert
				
			m_pHolder->_hideSlots( it._slot(), 1 );
		};


		void	hide( int index, int amount ) 
		{
			//TODO: Assert
				
			m_pHolder->_hideSlots( m_pSlotArray->slot(index), amount );
		};

		void	hide( iterator beg, iterator end ) 
		{
			//TODO: Assert
				
			m_pHolder->_hideSlots( beg._slot(), end._slot() - beg._slot() );
		};

		
		void	unhide( int index )
		{
			//TODO: Assert
			
			m_pHolder->_unhideSlots( m_pSlotArray->slot(index), 1 );
		};

		void	unhide( iterator it )
		{
			//TODO: Assert

			m_pHolder->_unhideSlots( it._slot(), 1 );
		};

		void	unhide( int index, int amount )
		{
			//TODO: Assert
				
			m_pHolder->_unhideSlots( m_pSlotArray->slot(index), amount );
		};

		void	unhide( iterator beg, iterator end )
		{
			//TODO: Assert

			m_pHolder->_unhideSlots( beg._slot(), end._slot() - beg._slot() );
		};


		bool	isVisible( int index ) 
		{ 
			//TODO: Assert

			return m_pSlotArray->slot(index)->bVisible; 
		}

		bool		isVisible( iterator it ) 
		{ 
			//TODO: Assert

			return it._slot()->bVisible; 
		}

	};	
	
};

#endif	//WG_HIDEABLECHILDREN_DOT_H
