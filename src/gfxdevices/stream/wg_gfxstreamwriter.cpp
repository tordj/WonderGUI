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

#include <cstring>

#include <wg_gfxstreamwriter.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamWriter::TYPEINFO = { "GfxStreamWriter", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamWriter_p GfxStreamWriter::create(std::function<void(int nBytes, const void * pData)> dispatcher )
	{
		return new GfxStreamWriter(dispatcher);
	}

	//____ constructor _____________________________________________________________

	GfxStreamWriter::GfxStreamWriter(std::function<void(int nBytes, const void * pData)> dispatcher) : stream(this)
	{
		m_dispatcher = dispatcher;
	}

	//____ Destructor _________________________________________________________

	GfxStreamWriter::~GfxStreamWriter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamWriter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _processStreamChunks() _____________________________________________

	void GfxStreamWriter::_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		m_dispatcher(pEnd - pBegin, pBegin);
	}


	//____ _object() __________________________________________________________

	Object * GfxStreamWriter::_object()
	{
		return this;
	}

	const Object * GfxStreamWriter::_object() const
	{
		return this;
	}


} // namespace wg
