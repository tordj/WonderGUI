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


#ifndef	WG_CONTEXT_DOT_H
#define	WG_CONTEXT_DOT_H


class WgSurfaceFactory;

#include <wg3_gfxdevice.h>

struct WgContext
{
    wg::GfxDevice_p pDevice = nullptr;
    WgSurfaceFactory * pFactory = nullptr;
    float         scale = 1.f;
};

#endif //WG_CONTEXT_DOT_H

