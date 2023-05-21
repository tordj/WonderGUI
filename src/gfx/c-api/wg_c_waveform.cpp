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

#include <wg_c_waveform.h>
#include <wg_waveform.h>

#include <type_traits>


using namespace wg;


inline Waveform* getPtr(wg_obj obj) {
	return static_cast<Waveform*>(reinterpret_cast<Object*>(obj));
}


wg_sizeI wg_waveformPixelSize(wg_obj waveform)
{
	SizeI sz = getPtr(waveform)->pixelSize();
	return { sz.w,sz.h };
}


int wg_setRenderSegments(wg_obj waveform, int nSegments)
{
	return getPtr(waveform)->setRenderSegments(nSegments);
}


int wg_getRenderSegments(wg_obj waveform)
{
	return getPtr(waveform)->renderSegments();
}

wg_color wg_waveformColor(wg_obj waveform, int segment)
{
	auto color = getPtr(waveform)->color(segment);
	return { color.b, color.g, color.r, color.a };
}

wg_gradient wg_waveformGradient(wg_obj waveform, int segment)
{
	auto gradient = getPtr(waveform)->gradient(segment);
	return *(wg_gradient*)&gradient;
}

int wg_waveformSegments(wg_obj waveform)
{
	return getPtr(waveform)->segments();
}

int wg_waveformSamples(wg_obj waveform)
{
	return getPtr(waveform)->samples();
}

int wg_importSpxSamples(wg_obj waveform, wg_waveOrigo origo, const wg_spx* pSource,
	int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd,
	int edgePitch, int samplePitch)
{
	return getPtr(waveform)->importSamples((WaveOrigo)origo, (const spx*)pSource, 
		edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
}

int	wg_importFloatSamples(wg_obj waveform, wg_waveOrigo origo, const float* pSource,
	int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd,
	int edgePitch, int samplePitch)
{
	return getPtr(waveform)->importSamples((WaveOrigo)origo, pSource,
		edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
}

int	wg_exportSpxSamples(wg_obj waveform, wg_waveOrigo origo, wg_spx* pDestination,
	int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd,
	int edgePitch, int samplePitch)
{
	return getPtr(waveform)->exportSamples((WaveOrigo)origo, (spx*)pDestination,
		edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
}

int wg_exportFloatSamples(wg_obj waveform, wg_waveOrigo origo, float* pDestination,
	int edgeBegin, int edgeEnd, int sampleBegin, int sampleEnd,
	int edgePitch, int samplePitch)
{
	return getPtr(waveform)->exportSamples((WaveOrigo)origo, pDestination,
		edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
}
