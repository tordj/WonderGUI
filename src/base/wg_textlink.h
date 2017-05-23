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

#ifndef WG_TEXTLINK_DOT_H
#define WG_TEXTLINK_DOT_H
#pragma once

#include <string>


#include <wg_pointers.h>

namespace wg 
{
	
	class Coord;
	
	class TextStyle;
	typedef	StrongPtr<TextStyle,Object_p>		TextStyle_p;
	typedef	WeakPtr<TextStyle,Object_wp>		TextStyle_wp;
		
	class TextLink;
	typedef	StrongPtr<class TextLink,Object_p> TextLink_p;
	typedef	WeakPtr<class TextLink,Object_wp> TextLink_wp;
	
	
	//____ TextLink _____________________________________________________________
	
	
	class TextLink : public Object
	{
	public:
		//.____ Creation __________________________________________

		static TextLink_p create( const std::string& link ) { return TextLink_p(new TextLink(link)); }
		static TextLink_p create( const std::string& link, TextStyle * pStyle ) { return TextLink_p(new TextLink(link, pStyle)); }

		//.____ Identification __________________________________________
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static TextLink_p		cast( Object * pObject );

		//.____ Misc ___________________________________________________________		
		
		const std::string&		link() const { return m_link; }
		TextStyle_p				style() const;
	
	private:
		TextLink( const std::string& link );
		TextLink( const std::string& link, TextStyle * style );
		~TextLink();

		std::string				m_link;
		TextStyle_p				m_pStyle;
	
	};
	

} // namespace wg
#endif //WG_TEXTLINK_DOT_H
