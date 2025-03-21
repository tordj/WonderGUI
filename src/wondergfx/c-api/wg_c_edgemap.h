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

#ifndef WG_C_EDGEMAP_DOT_H
#define WG_C_EDGEMAP_DOT_H
#pragma once

#include <wg_c_gfxtypes.h>
#include <wg_c_geo.h>
#include <wg_c_color.h>
#include <wg_c_gradient.h>

#ifdef __cplusplus
extern "C" {
#endif



	//____ wg_edgemapBP __________________________________________________________

	typedef struct wg_edgemapBP_struct			// NOT BINARY EQUIVALENT!
	{
		const wg_color*		colors;
		const wg_gradient*	gradients;
		int					segments;
		wg_sizeI			size;

	} wg_edgemapBP;


	//.____ Geometry ______________________________________________________

	wg_sizeI		wg_edgemapPixelSize(wg_obj edgemap);

	//.____ Appearance ____________________________________________________

	int				wg_setRenderSegments(wg_obj edgemap, int nSegments);
	int				wg_getRenderSegments(wg_obj edgemap);

	wg_color		wg_edgemapColor(wg_obj edgemap, int segment);
	wg_gradient		wg_edgemapGradient(wg_obj edgemap, int segment);

	const wg_color *	wg_edgemapColors(wg_obj edgemap);
	const wg_gradient *	wg_edgemapGradients(wg_obj edgemap);

	int				wg_setEdgemapColors( wg_obj edgemap, int begin, int end, const wg_color * pColors );
	int				wg_setEdgemapGradients( wg_obj edgemap, int begin, int end, const wg_gradient * pGradients );




	//.____ Content _______________________________________________________

	int		wg_edgemapSegments(wg_obj edgemap);

	int		wg_edgemapSamples(wg_obj edgemap);

	int 	wg_importSpxSamples(wg_obj edgemap, wg_sampleOrigo origo, const wg_spx* pSource, 
								int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
								int edgePitch, int samplePitch);

	int 	wg_importFloatSamples(	wg_obj edgemap, wg_sampleOrigo origo, const float* pSource, 
									int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
									int edgePitch, int samplePitch);

	int 	wg_exportSpxSamples(wg_obj edgemap, wg_sampleOrigo origo, wg_spx* pDestination, 
								int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd, 
								int edgePitch, int samplePitch);

	int 	wg_exportFloatSamples(	wg_obj edgemap, wg_sampleOrigo origo, float* pDestination, 
									int edgeBegin, int edgeEnd,	int sampleBegin, int sampleEnd, 
									int edgePitch, int samplePitch);


#ifdef __cplusplus
}
#endif

#endif
