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

#ifndef WG_C_GEO_DOT_H
#define WG_C_GEO_DOT_H
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


	typedef int		wg_spx;
	typedef float	wg_pts;


	struct wg_coordI
	{
		int	x, y;
	};

	struct wg_coordF
	{
		float	x, y;
	};

	struct wg_coord
	{
		wg_pts	x, y;
	};


	struct wg_coordSPX
	{
		wg_spx	x, y;
	};


	struct wg_sizeI
	{
		int	w, h;
	};

	struct wg_sizeF
	{
		float	w, h;
	};

	struct wg_size
	{
		wg_pts	w, h;
	};


	struct wg_sizeSPX
	{
		wg_spx	w, h;
	};


	struct wg_borderI
	{
		int	top, right, bottom, left;
	};

	struct wg_borderF
	{
		float	top, right, bottom, left;
	};

	struct wg_border
	{
		wg_pts	top, right, bottom, left;
	};

	struct wg_borderSPX
	{
		wg_spx	top, right, bottom, left;
	};


	struct wg_rectI
	{
		int	x, y, w, h;
	};

	struct wg_rectF
	{
		float	x, y, w, h;
	};

	struct wg_rect
	{
		wg_pts	x, y, w, h;
	};


	struct wg_rectSPX
	{
		wg_spx	x, y, w, h;
	};

#ifdef __cplusplus
}
#endif

#endif 