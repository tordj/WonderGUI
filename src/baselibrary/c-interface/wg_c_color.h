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

#ifndef WG_C_COLOR_DOT_H
#define WG_C_COLOR_DOT_H
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct wg_color
	{
		union
		{
			struct
			{
				int16_t b;
				int16_t g;
				int16_t r;
				int16_t a;
			};
			int64_t argb;
		};
	};

	struct wg_color8
	{
		union
		{
			struct
			{
				uint8_t b;
				uint8_t g;
				uint8_t r;
				uint8_t a;
			};
			uint32_t argb;
		};
	};

#ifdef __cplusplus
}
#endif

#endif
