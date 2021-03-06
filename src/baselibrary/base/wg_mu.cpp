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

#include <wg_mu.h>

namespace wg
{
	int		MU::s_qpixPerPoint = 4;
	float	MU::s_scale = 1.f;

	const MU	MU::Max = MU::fromQpix(0xFFFFFF*4);
	const MU	MU::Min = MU::fromQpix(-0xFFFFFF*4);


	MU abs(MU in) 
	{ 
		MU res; 
		res.qpix = in.qpix < 0 ? -in.qpix : in.qpix; 
		return res; 
	}


};
