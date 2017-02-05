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

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg
{
	class Widget;
	class Container;

	class WidgetCollection;
	typedef	StrongInterfacePtr<WidgetCollection,Interface_p>	WidgetCollection_p;
	typedef	WeakInterfacePtr<WidgetCollection,Interface_wp>		WidgetCollection_wp;


	class WidgetCollection : public Interface
	{
	public:
		WidgetCollection( Container * pContainer ) : m_pContainer(pContainer) {}

		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WidgetCollection_p	cast( const Interface_p& pInterface );				// Provided just for completeness sake.
		inline WidgetCollection_p	ptr() { return WidgetCollection_p(_object(),this); } 


		class Iterator
		{
		public: 
			virtual Iterator& operator++();   // ++Iterator
			virtual Iterator operator++(int); // Iterator++
			
			virtual Iterator& operator--();   // --Iterator
			virtual Iterator operator--(int); // Iterator--
			
			inline Widget& operator*() const { return * this->m_pWidget; }
			inline Widget* operator->() const{ return this->m_pWidget; }
			
		protected:
			Iterator() {};
			
			Widget *m_pWidget;
			void *  m_pHook;
		};

		virtual Iterator begin() = 0;
		virtual Iterator end() = 0;
		virtual size_t size() = 0;

	protected:
		Container * m_pContainer;

		Object * _object() const;

  };

} // namespace wg
#endif	// WG_WIDGETCOLLECTION_DOT_H
