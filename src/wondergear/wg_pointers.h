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

#include <wg_object.h>
#include <wg_strongptr.h>
#include <wg_weakptr.h>
#include <wg_componentptr.h>

template <class Type_p>
Type_p wg_dynamic_cast(wg::Object * pObject) {

	return dynamic_cast<typename Type_p::raw_type>(pObject);
}

template <class Type_p>
Type_p wg_static_cast(wg::Object * pObject) {

	return static_cast<typename Type_p::raw_type>(pObject);
}


#endif //WG_POINTERS_DOT_H

