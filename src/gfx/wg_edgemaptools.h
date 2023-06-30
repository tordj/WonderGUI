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
#ifndef EDGEMAPTOOLS_DOT_H
#define EDGEMAPTOOLS_DOT_H
#pragma once

#include <wg_edgemap.h>
#include <wg_edgemapfactory.h>

namespace wg
{
namespace EdgemapTools
{
	void	init();
	void	exit();


	Edgemap_p createDonut(const SizeSPX size, spx thickness, HiColor fillColor, spx outlineThickness, HiColor outlineColor, EdgemapFactory * pFactory = nullptr );

	bool drawOutlinedDonut(Edgemap * pEdgemap, spx thickness, spx outlineThickness );




}

}

#endif //EDGEMAPTOOLS_DOT_H
