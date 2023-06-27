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

#include <tuple>

#include <wg_c_gfxstreamplayer.h>

#include <wg_gfxstreamplayer.h>

using namespace wg;

inline GfxStreamPlayer* getPtr(wg_obj obj) {
	return static_cast<GfxStreamPlayer*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createStreamPlayer(wg_obj device, wg_obj surfaceFactory, wg_obj waveformFactory)
{
	GfxDevice* pDevice = static_cast<GfxDevice*>(reinterpret_cast<Object*>(device));
	SurfaceFactory* pSurfaceFactory = static_cast<SurfaceFactory*>(reinterpret_cast<Object*>(surfaceFactory));
	WaveformFactory* pWaveformFactory = static_cast<WaveformFactory*>(reinterpret_cast<Object*>(waveformFactory));

	auto p = GfxStreamPlayer::create(pDevice, pSurfaceFactory, pWaveformFactory);
	p->retain();
	return p.rawPtr();
}

wg_component wg_getStreamPlayerInput(wg_obj streamPlayer)
{
	auto p = getPtr(streamPlayer);
	return { static_cast<Object*>(p), &p->input };
}

void wg_resetStreamPlayer(wg_obj streamPlayer)
{
	auto p = getPtr(streamPlayer);
	p->reset();
}


void wg_setStreamPlayerStoreDirtyRects(wg_obj streamPlayer, int bStore)
{
	getPtr(streamPlayer)->setStoreDirtyRects(bStore);
}

void wg_setStreamPlayerMaxDirtyRects(wg_obj streamPlayer, int max)
{
	getPtr(streamPlayer)->setMaxDirtyRects(max);
}

int wg_getStreamPlayerDirtyRects(wg_obj streamPlayer, wg_canvasRef canvas, const wg_rectI** pRects)
{
	int size;
	const RectI* pDirtyRects;
	std::tie(size,pDirtyRects) = getPtr(streamPlayer)->dirtyRects( (CanvasRef) canvas);
	*pRects = (wg_rectI*) pDirtyRects;
	return size;
}

void wg_clearStreamPlayerDirtyRects(wg_obj streamPlayer)
{
	getPtr(streamPlayer)->clearDirtyRects();
}
