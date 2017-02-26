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

#ifndef	WG_SLOTCAN_DOT_H
#define	WG_SLOTCAN_DOT_H

#ifndef WG_SLOT_DOT_H
#	include <wg_slot.h>
#endif

namespace wg 
{
	class Object;
	class Widget;


	//____ SlotCanHolder _________________________________________________________

	class SlotCanHolder
	{
	public:
		virtual Object * _object() = 0;
		virtual const Object * _object() const = 0;

		virtual void	_setWidget( Slot * pSlot, Widget * pNewWidget ) = 0;
	};

	//____ SlotCan _________________________________________________________________
	
	template<class SlotType> class SlotCan
	{
	public:
		SlotCan( SlotCanHolder * pHolder ) : m_pHolder(pHolder) {}

		inline Object *			object() { return m_pHolder->_object(); } 
		inline const Object *	object() const { return m_pHolder->_object(); } 
	
		inline void	setWidget( Widget * pWidget ) {	m_pHolder->_setWidget( &slot, pWidget ); }

		SlotType	slot;
	protected:
	
		SlotCanHolder * m_pHolder;
	};
	
	
	

} // namespace wg
#endif //WG_SLOTCAN_DOT_H
