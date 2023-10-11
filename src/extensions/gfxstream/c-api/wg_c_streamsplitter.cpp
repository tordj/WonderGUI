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

#include <wg_c_streamsplitter.h>
#include <wg_streamsplitter.h>

using namespace wg;

inline StreamSplitter* getPtr(wg_obj obj) {
	return static_cast<StreamSplitter*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createStreamSplitter()
{
	auto p = StreamSplitter::create();
	p->retain();
	return p.rawPtr();
}

wg_obj wg_createStreamSplitterWithOutputs(wg_component output1, wg_component output2)
{
	StreamSink_p pSink1(reinterpret_cast<Object*>(output1.object), *reinterpret_cast<StreamSink*>(output1.pInterface));
	StreamSink_p pSink2(reinterpret_cast<Object*>(output2.object), *reinterpret_cast<StreamSink*>(output2.pInterface));

	auto p = StreamSplitter::create( { pSink1, pSink2 });
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
	StreamSink_p pSink(reinterpret_cast<Object*>(output.object), *reinterpret_cast<StreamSink*>(output.pInterface));
	getPtr(streamSplitter)->addOutput(pSink);

}

void wg_removeStreamSplitterOutput(wg_obj streamSplitter, wg_component output)
{
	StreamSink_p pSink(reinterpret_cast<Object*>(output.object), *reinterpret_cast<StreamSink*>(output.pInterface));
	getPtr(streamSplitter)->removeOutput(pSink);
}
