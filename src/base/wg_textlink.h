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

#include <string>


#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg 
{
	
	class WgCoord;
	
	class WgTextlink;
	typedef	WgStrongPtr<class WgTextlink,WgObject_p> WgTextLink_p;
	typedef	WgWeakPtr<class WgTextlink,WgObject_wp> WgTextLink_wp;
	
	
	//____ WgTextLink _____________________________________________________________
	
	
	class WgTextlink : public WgObject
	{
	public:
		static WgTextLink_p create( const std::string& link ) { return new WgTextlink(link); }
	
		const std::string&		link() const { return m_link; }
		bool					hasBeenAccessed() const { return m_bAccessed; }
	
	private:
		bool					m_bAccessed;
		std::string				m_link;
	
		WgTextlink( const std::string& link ) : m_bAccessed(false), m_link(link) {}
		~WgTextlink() {}
	};
	

} // namespace wg
#endif //WG_TEXTLINK_DOT_H
