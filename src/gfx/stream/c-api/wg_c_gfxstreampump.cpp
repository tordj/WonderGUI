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


#include <wg_c_gfxstreampump.h>
#include <wg_gfxstreampump.h>

using namespace wg;

inline GfxStreamPump* getPtr(wg_obj obj) {
	return static_cast<GfxStreamPump*>(reinterpret_cast<Object*>(obj));
}


wg_obj wg_createStreamPump()
{
	auto p = GfxStreamPump::create();
	p->retain();
	return p.rawPtr();
}

wg_obj wg_createStreamPumpWithInputOutput(wg_component input, wg_component output)
{
	GfxStreamSource_p pInput(reinterpret_cast<Object*>(input.object), * reinterpret_cast<GfxStreamSource*>(input.pInterface));
	GfxStreamSink_p pOutput(reinterpret_cast<Object*>(output.object), *reinterpret_cast<GfxStreamSink*>(output.pInterface));

	auto p = GfxStreamPump::create(pInput,pOutput);
	p->retain();
	return p.rawPtr();
}

void wg_setStreamPumpInput(wg_obj streamPump, wg_component input)
{
	GfxStreamSource_p pInput(reinterpret_cast<Object*>(input.object), *reinterpret_cast<GfxStreamSource*>(input.pInterface));
	getPtr(streamPump)->setInput(pInput);
}

void wg_setStreamPumpOutput(wg_obj streamPump, wg_component output)
{
	GfxStreamSink_p pOutput(reinterpret_cast<Object*>(output.object), *reinterpret_cast<GfxStreamSink*>(output.pInterface));
	getPtr(streamPump)->setOutput(pOutput);
}


wg_gfxChunkId wg_peekChunk(wg_obj streamPump)
{
	return (wg_gfxChunkId) getPtr(streamPump)->peekChunk();
}

int wg_pumpChunk(wg_obj streamPump)
{
	return getPtr(streamPump)->pumpChunk();
}

int wg_pumpUntilFrame(wg_obj streamPump)
{
	return getPtr(streamPump)->pumpUntilFrame();
}

int wg_pumpFrame(wg_obj streamPump)
{
	return getPtr(streamPump)->pumpFrame();
}

int wg_pumpAllFramesOptimizeClipping(wg_obj streamPump, int optimizationDepth )
{
	return getPtr(streamPump)->pumpAllFramesOptimizeClipping(optimizationDepth);
}


int wg_pumpAll(wg_obj streamPump)
{
	return getPtr(streamPump)->pumpAll();
}

