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

#ifndef	WG_CHILDGROUP_DOT_H
#define	WG_CHILDGROUP_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_SLOTARRAY_DOT_H
#	include <wg_slotarray.h>
#endif


namespace wg 
{

	template<class SlotType> class ChildGroup;
	typedef	StrongInterfacePtr<ChildGroup<class SlotType>,Interface_p>		ChildGroup_p;
	typedef	WeakInterfacePtr<ChildGroup<class SlotType>,Interface_wp>		ChildGroup_wp;
	
	template<class SlotType> class ChildGroup : public Interface
	{
		
	public:
		ChildGroup( SlotArray<SlotType> * pSlotArray ) : m_pSlotArray(pSlotArray) {}

		inline const Widget*& operator[](int index) const { return m_pSlotArray->slot(index)->pWidget; }

		inline int		size() const { return m_pSlotArray->size(); } 
		inline Widget_p get( int index) const { return Widget_p(m_pSlotArray->slot(index)->pWidget)); }
		inline void		clear() { m_pSlotArray->clear(); }
	
	protected:
		Object * _object() const {	return m_pSlotArray->object(); }

		SlotArray<SlotType> * m_pSlotArray;
	};
	
	

} // namespace wg
#endif //WG_CHILDGROUP_DOT_H
