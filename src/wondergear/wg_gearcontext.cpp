
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


#include <wg_gearbase.h>
#include <wg_memstack.h>
#include <wg_mempool.h>
#include <wg_object.h>

#include <iostream>
#include <algorithm>
#include <cassert>

namespace wg
{
	const TypeInfo GearContext::TYPEINFO = { "GearContext", &Object::TYPEINFO };


	//____ typeInfo() _________________________________________________________

	const TypeInfo& GearContext::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setErrorHandler() _________________________________________________________

	void GearContext::setErrorHandler(std::function<void(Error&)> handler)
	{
		m_pErrorHandler = handler;
	}

	//____ errorHandler() ____________________________________________________________

	std::function<void(Error&)>	GearContext::errorHandler() const
	{
		return m_pErrorHandler;
	}



} // namespace wg
