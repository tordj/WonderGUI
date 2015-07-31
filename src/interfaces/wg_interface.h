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
#ifndef WG_INTERFACE_DOT_H

namespace wg 
{
	#define WG_INTERFACE_DOT_H
	
	class Interface_p;
	class Interface_wp;
	
	class Object;
	class Object_p;
	
	
	/**
	 * @brief Provides access to items embedded into a Widget or Object.
	 *
	 * Interfaces are provided by Widgets and other reference counted Objects
	 * to provide API access to their embedded items such as labels and
	 * icons.
	 *
	 * The interface concept of WonderGUI serves two purposes:
	 *
	 * First it
	 * provides a nice API-level abstraction to keep methods for accessing
	 * different components of a widget logically separated while providing
	 * an identical way to access embedded items in all widgets that
	 * contains them.
	 *
	 * Secondly it provides a safe way to pass around pointers to the embedded
	 * items since interface pointers do reference counting on the object
	 * providing the interface, while pointing at the interface directly.
	 *
	 **/
	
	class Interface
	{
		friend class Interface_p;
		friend class Interface_wp;
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static Interface_p	cast( const Interface_p& pInterface );			// Provided just for completeness sake.
		Interface_p			ptr();												///< @brief Get a pointer to this interface.
		Object_p				holder();											///< @breif Get a pointer to the object providing this interface.
	
	protected:
		virtual Object * 		_object() const = 0;
	};
	
	

} // namespace wg
#endif //WG_INTERFACE_DOT_H
