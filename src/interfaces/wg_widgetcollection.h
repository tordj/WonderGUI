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

#ifndef	WG_WIDGETCOLLECTION_DOT_H
#define	WG_WIDGETCOLLECTION_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_SLOT_DOT_H
#	include <wg_slot.h>
#endif


namespace wg 
{

	class WidgetCollection;
	typedef	StrongInterfacePtr<WidgetCollection,Interface_p>	WidgetCollection_p;
	typedef	WeakInterfacePtr<WidgetCollection,Interface_wp>		WidgetCollection_wp;
	
	class WidgetCollection : public Interface
	{
		
	public:
		WidgetCollection( WidgetHolder * pHolder, Slot * pSlot ) : m_pHolder(pHolder), m_pSlot(pSlot) {}

		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WidgetCollection_p		cast( const Interface_p& pInterface );
	

		inline operator bool() const { return m_pSlot->pWidget != nullptr; }

//		inline Widget& operator*() const{ return * m_pSlot->pWidget; };
		inline Widget* operator->() const { return m_pSlot->pWidget; }


		inline Widget_p get( int index) const { return Widget_p(m_pSlot->pWidget); }
		inline void clear() { m_pHolder->_replaceChild(m_pSlot, nullptr); }
	
	

	protected:
		virtual Object * 	_object() const;
		
		WidgetHolder *	m_pHolder;
		SlotArray *		m_pSlots;
	};
	
	

} // namespace wg
#endif //WG_WIDGETCOLLECTION_DOT_H
