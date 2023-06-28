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
#ifndef WAVEFORMTOOLS_DOT_H
#define WAVEFORMTOOLS_DOT_H
#pragma once

#include <wg_waveform.h>
#include <wg_waveformfactory.h>

namespace wg
{
namespace WaveformTools
{
	void	init();
	void	exit();


	Waveform_p createDonut(const SizeSPX size, spx thickness, HiColor fillColor, spx outlineThickness, HiColor outlineColor, WaveformFactory * pFactory = nullptr );

	bool drawOutlinedDonut(Waveform * pWaveform, spx thickness, spx outlineThickness );




}

}

#endif //WAVEFORMTOOLS_DOT_H
