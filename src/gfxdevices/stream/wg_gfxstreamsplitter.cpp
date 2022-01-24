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

#include <wg_gfxstreamsplitter.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamSplitter::TYPEINFO = { "GfxStreamSplitter", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamSplitter_p GfxStreamSplitter::create(const std::initializer_list<CGfxOutStream_p>& outputs)
	{
		return new GfxStreamSplitter(outputs);
	}

	//____ constructor _____________________________________________________________

	GfxStreamSplitter::GfxStreamSplitter(const std::initializer_list<CGfxOutStream_p>& outputs) : input(this)
	{
		m_pOutputs = outputs;
	}

	//____ Destructor _________________________________________________________

	GfxStreamSplitter::~GfxStreamSplitter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamSplitter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _processStreamChunks() _____________________________________________

	void GfxStreamSplitter::_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		for (auto& pOutput : m_pOutputs)
			pOutput->processChunks(pBegin, pEnd);
	}

	//____ _object() __________________________________________________________

	Object * GfxStreamSplitter::_object()
	{
		return this;
	}

	const Object * GfxStreamSplitter::_object() const
	{
		return this;
	}


} // namespace wg
