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
#include <algorithm>

#include <wg_gfxstreamsplitter.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamSplitter::TYPEINFO = { "GfxStreamSplitter", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamSplitter_p GfxStreamSplitter::create()
	{
		return new GfxStreamSplitter();
	}

	GfxStreamSplitter_p GfxStreamSplitter::create(const std::initializer_list<CGfxStreamSink_p>& outputs)
	{
		return new GfxStreamSplitter(outputs);
	}

	//____ constructor _____________________________________________________________

	GfxStreamSplitter::GfxStreamSplitter() : input(this)
	{
	}

	GfxStreamSplitter::GfxStreamSplitter(const std::initializer_list<CGfxStreamSink_p>& outputs) : input(this)
	{
		m_outputs = outputs;
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

	//____ clearOutputs() ________________________________________________________

	void GfxStreamSplitter::clearOutputs()
	{
		m_outputs.clear();
	}

	//____ addOutput() ___________________________________________________________

	void GfxStreamSplitter::addOutput( CGfxStreamSink * pOutput )
	{
		m_outputs.push_back(pOutput);
	}

	//____ removeOutput() ________________________________________________________

	void GfxStreamSplitter::removeOutput( CGfxStreamSink * pOutput )
	{
		auto newEnd = std::remove( m_outputs.begin(), m_outputs.end(), pOutput );
		m_outputs.erase(newEnd, m_outputs.end());
	}

	//____ _processStreamChunks() _____________________________________________

	void GfxStreamSplitter::_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		for (auto& pOutput : m_outputs)
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
