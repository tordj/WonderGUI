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

#include <wg_geocomponent.h>

namespace wg
{

	const TypeInfo	GeoComponent::TYPEINFO = { "GeoComponent", &Component::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GeoComponent::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _object() __________________________________________________________

	Object * GeoComponent::_object()
	{ 
		return m_pHolder->_object(); 
	}

	const Object * GeoComponent::_object() const
	{ 
		return m_pHolder->_object(); 
	}

} // namespace wg
