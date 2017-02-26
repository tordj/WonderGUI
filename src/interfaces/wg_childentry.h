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

#ifndef	WG_CHILDENTRY_DOT_H
#define	WG_CHILDENTRY_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_SLOTCAN_DOT_H
#	include <wg_slotcan.h>
#endif


namespace wg 
{

	template<class SlotType> class ChildEntry;
	typedef	StrongInterfacePtr<ChildEntry<class SlotType>,Interface_p>	ChildEntry_p;
	typedef	WeakInterfacePtr<ChildEntry<class SlotType>,Interface_wp>	ChildEntry_wp;
	
	template<class SlotType> class ChildEntry : public Interface
	{
		
	public:
		ChildEntry( SlotCan<SlotType> * pSlotCan ) : m_pSlotCan(pSlotCan) {}
/*
		static const char CLASSNAME[] = {"ChildEntry<SlotType>"};

		bool isInstanceOf( const char * pClassName ) const
		{ 
			if( pClassName==CLASSNAME )
				return true;

			return Interface::isInstanceOf(pClassName);
		}

		const char * className( void ) const
		{ 
			return CLASSNAME; 
		}

		ChildEntry_p cast( const Interface_p& pInterface )
		{
			if( pInterface && pInterface->isInstanceOf(CLASSNAME) )
				return ChildEntry_p( pInterface.getRealObjectPtr(), static_cast<ChildEntry*>( pInterface.rawPtr()) );

			return 0;
		}
*/		
	
		inline ChildEntry operator=(const Widget_p& pWidget ) { m_pSlotCan->setWidget(pWidget.rawPtr()); return *this; }
		inline operator Widget_p() const { return Widget_p(m_pSlotCan->slot.pWidget); }

		inline bool operator==(const Widget_p& other) const { return other.rawPtr() == m_pSlotCan->slot.pWidget; }
		inline bool operator!=(const Widget_p& other) const { return other.rawPtr() != m_pSlotCan->slot.pWidget; }
	
		inline operator bool() const { return m_pSlotCan->slot.pWidget != nullptr; }

//		inline Widget& operator*() const{ return * m_pSlotCan->pWidget; };
		inline Widget* operator->() const { return m_pSlotCan->slot.pWidget; }


		inline Widget_p get() const { return Widget_p(m_pSlotCan->slot.pWidget); }
		inline void clear() { m_pSlotCan->_setWidget(nullptr); }

	protected:
		Object * _object() const {	return m_pSlotCan->object(); }

		SlotCan<SlotType> *	m_pSlotCan;
	};
	
	

} // namespace wg
#endif //WG_CHILDENTRY_DOT_H
