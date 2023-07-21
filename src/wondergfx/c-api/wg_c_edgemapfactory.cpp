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

#include <wg_c_edgemapfactory.h>
#include <wg_c_internal.h>
#include <wg_edgemapfactory.h>

using namespace wg;

inline EdgemapFactory* getPtr(wg_obj obj) {
	return static_cast<EdgemapFactory*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createEdgemap(wg_obj factory, const wg_edgemapBP* blueprint)
{
	auto p = getPtr(factory)->createEdgemap(*(const Edgemap::Blueprint*)blueprint);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj wg_createEdgemapFromFloats(wg_obj factory, const wg_edgemapBP* blueprint, wg_waveOrigo origo,
	const float* pSamples, int edges, int edgePitch, int samplePitch)
{
	auto p = getPtr(factory)->createEdgemap(*(const Edgemap::Blueprint*)blueprint, (WaveOrigo) origo, pSamples, edges, edgePitch, samplePitch);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}

wg_obj wg_createEdgemapFromSpx(wg_obj factory, const wg_edgemapBP* blueprint, wg_waveOrigo origo,
	const wg_spx* pSamples, int edges, int edgePitch, int samplePitch)
{
	auto p = getPtr(factory)->createEdgemap(*(const Edgemap::Blueprint*)blueprint, (WaveOrigo)origo, (const spx*) pSamples, edges, edgePitch, samplePitch);
	p->retain();
	return (wg_obj) static_cast<Object*>(p.rawPtr());
}
