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

#ifndef WG_C_OBJECT_DOT_H
#define WG_C_OBJECT_DOT_H
#pragma once

#include <wg_c_types.h>

#ifdef __cplusplus
extern "C" {
#endif


	typedef	void(*wg_finalizer_func)(void*);


	const wg_typeInfo* wg_getTypeInfo(wg_obj);
	int					wg_isInstanceOf(wg_obj, const wg_typeInfo* pTypeInfo);

	void				wg_setFinalizer(wg_obj, wg_finalizer_func pFinalizer);
	wg_finalizer_func	wg_finalizer(wg_obj);

	void				wg_retain(wg_obj);
	void				wg_release(wg_obj);

	int					wg_refcount(wg_obj);


#ifdef __cplusplus
}
#endif

#endif


