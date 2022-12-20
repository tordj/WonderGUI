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

#include <wg_c_gfxstreamsplitter.h>
#include <wg_gfxstreamsplitter.h>

using namespace wg;

inline GfxStreamSplitter* getPtr(wg_obj obj) {
	return static_cast<GfxStreamSplitter*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createStreamSplitter()
{
	auto p = GfxStreamSplitter::create();
	p->retain();
	return p.rawPtr();
}

wg_obj wg_createStreamSplitterWithOutputs(wg_component output1, wg_component output2)
{
	GfxStreamSink_p pSink1(reinterpret_cast<Object*>(output1.object), *reinterpret_cast<GfxStreamSink*>(output1.interface));
	GfxStreamSink_p pSink2(reinterpret_cast<Object*>(output2.object), *reinterpret_cast<GfxStreamSink*>(output2.interface));

	auto p = GfxStreamSplitter::create( { pSink1, pSink2 });
	p->retain();
	return p.rawPtr();
}

wg_component wg_getStreamSplitterInput(wg_obj streamSplitter)
{
	auto p = getPtr(streamSplitter);
	return { static_cast<Object*>(p), &p->input };
}

void wg_clearStreamSplitterOutputs(wg_obj streamSplitter)
{
	getPtr(streamSplitter)->clearOutputs();

}

void wg_addStreamSplitterOutput(wg_obj streamSplitter, wg_component output)
{
	GfxStreamSink_p pSink(reinterpret_cast<Object*>(output.object), *reinterpret_cast<GfxStreamSink*>(output.interface));
	getPtr(streamSplitter)->addOutput(pSink);

}

void wg_removeStreamSplitterOutput(wg_obj streamSplitter, wg_component output)
{
	GfxStreamSink_p pSink(reinterpret_cast<Object*>(output.object), *reinterpret_cast<GfxStreamSink*>(output.interface));
	getPtr(streamSplitter)->removeOutput(pSink);
}
