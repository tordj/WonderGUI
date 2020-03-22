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
#include <wg_receiver.h>

namespace wg
{

	const TypeInfo Receiver::TYPEINFO = { "Receiver", &Object::TYPEINFO };



	//____ typeInfo() _________________________________________________________

	const TypeInfo& Receiver::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _onRouteAdded() _________________________________________________________

	void  Receiver::_onRouteAdded()
	{
	}

	//____ _onRouteRemoved() _______________________________________________________

	void  Receiver::_onRouteRemoved()
	{
	}

} // namespace wg
