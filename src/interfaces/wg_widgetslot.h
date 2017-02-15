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

#ifndef	WG_WIDGETSLOT_DOT_H
#define	WG_WIDGETSLOT_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

namespace wg 
{

	class WidgetSlot;
	typedef	StrongInterfacePtr<WidgetSlot,Interface_p>	WidgetSlot_p;
	typedef	WeakInterfacePtr<WidgetSlot,Interface_wp>	WidgetSlot_wp;
	
	class WidgetSlot : public Interface
	{
		
	public:
		WidgetSlot( Container * pContainer, ChildSlot * pSlot ) : m_pContainer(pContainer), m_pSlot(pSlot) {}

		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WidgetSlot_p		cast( const Interface_p& pInterface );
	
		inline WidgetSlot operator=(const Widget_p& pWidget ) { m_pContainer->_updateSlot(m_pSlot, pWidget.rawPtr()); }
		inline operator Widget_p() const { return Widget_p(m_pSlot->pWidget); }

		inline bool operator==(const Widget_p& other) const { return other.rawPtr() == m_pSlot->pWidget; }
		inline bool operator!=(const Widget_p& other) const { return other.rawPtr() != m_pSlot->pWidget; }
	
		inline operator bool() const { return m_pSlot->pWidget != nullptr; }

//		inline Widget& operator*() const{ return * m_pSlot->pWidget; };
		inline Widget* operator->() const { return m_pSlot->pWidget; }


		inline void clear() { m_pContainer->_updateSlot(m_pSlot, nullptr); }
	
	

	protected:
		virtual Object * 	_object() const;
		
		Container *	m_pContainer;
		ChildSlot *	m_pSlot;
	};
	
	

} // namespace wg
#endif //WG_WidgetSlot_DOT_H
