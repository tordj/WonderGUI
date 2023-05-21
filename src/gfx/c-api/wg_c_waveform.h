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

#ifndef WG_C_WAVEFORM_DOT_H
#define WG_C_WAVEFORM_DOT_H
#pragma once

#include <wg_c_gfxtypes.h>
#include <wg_c_geo.h>
#include <wg_c_color.h>
#include <wg_c_gradient.h>

#ifdef __cplusplus
extern "C" {
#endif



	//____ wg_waveformBP __________________________________________________________

	typedef struct wg_waveformBP_struct			// NOT BINARY EQUIVALENT!
	{
		const wg_color*		colors;
		const wg_gradient*	gradients;
		int					segments;
		wg_sizeI			size;

	} wg_waveformBP;


	//.____ Geometry ______________________________________________________

	wg_sizeI		wg_waveformPixelSize(wg_obj waveform);

	//.____ Appearance ____________________________________________________

	int				wg_setRenderSegments(wg_obj waveform, int nSegments);
	int				wg_getRenderSegments(wg_obj waveform);

	wg_color		wg_waveformColor(wg_obj waveform, int segment);
	wg_gradient		wg_waveformGradient(wg_obj waveform, int segment);

	//.____ Content _______________________________________________________

	int		wg_waveformSegments(wg_obj waveform);

	int		wg_waveformSamples(wg_obj waveform);

	int 	wg_importSpxSamples(wg_obj waveform, wg_waveOrigo origo, const wg_spx* pSource, 
								int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
								int edgePitch, int samplePitch);

	int 	wg_importFloatSamples(	wg_obj waveform, wg_waveOrigo origo, const float* pSource, 
									int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
									int edgePitch, int samplePitch);

	int 	wg_exportSpxSamples(wg_obj waveform, wg_waveOrigo origo, wg_spx* pDestination, 
								int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
								int edgePitch, int samplePitch);

	int 	wg_exportFloatSamples(	wg_obj waveform, wg_waveOrigo origo, float* pDestination, 
									int edgeBegin, int edgeEnd,	int sampleBegin, int sampleEnd, 
									int edgePitch, int samplePitch);


#ifdef __cplusplus
}
#endif

#endif
