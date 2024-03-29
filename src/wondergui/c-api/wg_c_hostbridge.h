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

#ifndef WG_C_HOSTBRIDGE_DOT_H
#define WG_C_HOSTBRIDGE_DOT_H
#pragma once

#include <wg_c_types.h>

#ifdef __cplusplus
extern "C" {
#endif

	int		wg_hidePointer( void * pHostBridge );
	int		wg_showPointer( void * pHostBridge );

	int		wg_setPointerStyle( void * pHostBridge, wg_pointerStyle style );

	int		wg_getClipboardText( void * pHostBridge, int maxChars, char * pDest );
	int		wg_setClipboardText( void * pHostBridge, const char * pText );

	int		wg_requestWindowFocus( void * pHostBridge, uintptr_t windowRef);
	int		wg_yieldWindowFocus(void* pHostBridge, uintptr_t windowRef);

	int		wg_lockHidePointer(void* pHostBridge);
	int		wg_unlockShowPointer(void* pHostBridge);


#ifdef __cplusplus
}
#endif

#endif


