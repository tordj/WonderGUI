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

#ifndef	WG_GRADIENT_DOT_H
#define	WG_GRADIENT_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_color.h>

namespace wg
{
	class Gradient
	{
	public:

		Gradient() {}

		Gradient(HiColor topLeft, HiColor topRight, HiColor bottomRight, HiColor bottomLeft) :
			topLeft(topLeft),
			topRight(topRight),
			bottomRight(bottomRight),
			bottomLeft(bottomLeft) {}

		Gradient(Origo start, HiColor startColor, HiColor endColor);

		inline void clear() { topLeft = topRight = bottomRight = bottomLeft = Color::White; }
		inline bool isOpaque() { return (topLeft.a + topRight.a + bottomRight.a + bottomLeft.a) == 4096 * 4; }

		HiColor		topLeft;
		HiColor		topRight;
		HiColor		bottomRight;
		HiColor		bottomLeft;
	};


}



#endif //WG_GRADIENT_DOT_H
