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
#ifndef	WG_POINTERS_DOT_H
#define	WG_POINTERS_DOT_H
#pragma once

#include <wg_strongptr.h>
#include <wg_weakptr.h>
#include <wg_strongcomponentptr.h>
#include <wg_weakcomponentptr.h>

namespace wg
{

	template <class Type_p>
	Type_p wg_cast(Object* pObject) {

		if (pObject && pObject->isInstanceOf(Type_p::classname()) )
			return static_cast<typename Type_p::raw_type>(pObject);

		return nullptr;
	}
}


#endif //WG_POINTERS_DOT_H

