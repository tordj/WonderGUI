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


#include <wg_c_gfxstreambuffer.h>
#include <wg_gfxstreambuffer.h>

using namespace wg;

inline GfxStreamBuffer* getPtr(wg_obj obj) {
	return static_cast<GfxStreamBuffer*>(reinterpret_cast<Object*>(obj));
}

wg_obj wg_createStreamBuffer(int initialCapacity)
{
	auto pObj = GfxStreamBuffer::create(initialCapacity);
	pObj->retain();
	return static_cast<Object*>(pObj.rawPtr());
}

wg_component wg_getStreamBufferOutput(wg_obj streamBuffer)
{
	auto p = getPtr(streamBuffer);
	return { static_cast<Object*>(p), &p->output };
}

wg_component wg_getStreamBufferInput(wg_obj streamBuffer)
{
	auto p = getPtr(streamBuffer);
	return { static_cast<Object*>(p), &p->input };
}

int wg_streamBufferCapacity(wg_obj streamBuffer)
{
	return getPtr(streamBuffer)->capacity();
}

int wg_streamBufferHasChunk(wg_obj streamBuffer)
{
	return getPtr(streamBuffer)->hasChunk();
}

int wg_streamBufferSize(wg_obj streamBuffer)
{
	return getPtr(streamBuffer)->size();
}

void wg_resetStreamBuffer(wg_obj streamBuffer)
{
	getPtr(streamBuffer)->reset();
}

int wg_setStreamBufferCapacity( wg_obj streamBuffer, int capacity )
{
	return getPtr(streamBuffer)->setCapacity(capacity);
}



