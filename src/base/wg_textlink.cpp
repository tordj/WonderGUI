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

#include <wg_textlink.h>
#include <wg_textstyle.h>

namespace wg
{

	const TypeInfo TextLink::TYPEINFO = { "TextLink", &Object::TYPEINFO };

	//____ constructor _________________________________________________________

	TextLink::TextLink( const std::string& link ) : m_link(link)
	{
	}

	TextLink::TextLink( const std::string& link, TextStyle * style ) : m_link(link), m_pStyle(style)
	{
	}

	//____ destructor __________________________________________________________

	TextLink::~TextLink()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& TextLink::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ style() _____________________________________________________________

	TextStyle_p TextLink::style() const
	{
		return m_pStyle;
	}


} // namespace wg
