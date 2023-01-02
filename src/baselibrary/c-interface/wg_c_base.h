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


#ifndef WG_C_BASE_DOT_H
#define WG_C_BASE_DOT_H
#pragma once

#include <wg_c_types.h>

#ifdef __cplusplus
extern "C" {
#endif

//TODO: Not a complete wrapper, just contains what is needed for wg-display project to work.

	int		wg_init(void);
	int		wg_exit(void);
	int		wg_isInitialized(void);

	void	wg_setErrorHandler(void (*errorHandler)(const wg_errorInfo * pError) );


#ifdef __cplusplus
}
#endif

#endif //WG_C_BASE_DOT_H
