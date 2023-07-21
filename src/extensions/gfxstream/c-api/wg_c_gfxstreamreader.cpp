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

#include <wg_c_gfxstreamreader.h>
#include <wg_gfxstreamreader.h>

using namespace wg;

inline GfxStreamReader* getPtr(wg_obj obj) {
	return static_cast<GfxStreamReader*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createStreamReader(wg_readStream_func callback)
{
	auto p = GfxStreamReader::create(callback);
	p->retain();
	return p.rawPtr();
}

wg_component wg_getStreamReaderOutput(wg_obj streamReader)
{
	auto p = getPtr(streamReader);
	return { static_cast<Object*>(p), &p->output };
}

void wg_resetStreamReader(wg_obj streamReader)
{
	getPtr(streamReader)->reset();
}

int wg_streamReaderCapacity(wg_obj streamReader)
{
	return getPtr(streamReader)->bufferCapacity();
}

int wg_streamReaderHasChunk(wg_obj streamReader)
{
	return getPtr(streamReader)->chunkInBuffer();
}

int wg_streamReaderBytes(wg_obj streamReader)
{
	return getPtr(streamReader)->bytesInBuffer();
}
