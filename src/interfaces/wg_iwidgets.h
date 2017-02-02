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

#ifndef	WG_IWIDGETS_DOT_H
#define	WG_IWIDGETS_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

namespace wg 
{
	
	
	class IWidgets;
	typedef	StrongInterfacePtr<IWidgets,Interface_p>		IWidgets_p;
	typedef	WeakInterfacePtr<IWidgets,Interface_wp>	IWidgets_wp;
	
	class IWidgets : public Interface
	{
		
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static IWidgets_p	cast( const Interface_p& pInterface );
	
		inline Widget_p	firstChild() const { return Widget_p(_firstChild()); }
		inline Widget_p	lastChild() const { return Widget_p(_lastChild()); }
	
	protected:
	
		virtual Widget *		_firstChild() const = 0;
		virtual Widget *		_lastChild() const = 0;
	};
	
	

} // namespace wg
#endif //WG_IWIDGETS_DOT_H
