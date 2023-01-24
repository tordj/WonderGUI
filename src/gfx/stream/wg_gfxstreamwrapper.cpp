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

#include <wg_gfxstreamwrapper.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamWrapper::TYPEINFO = { "GfxStreamWrapper", &Object::TYPEINFO };

	//____ create() ___________________________________________________________

	GfxStreamWrapper_p GfxStreamWrapper::create( const void* pBegin, const void* pEnd )
	{
		return new GfxStreamWrapper( pBegin, pEnd );
	}

	GfxStreamWrapper_p GfxStreamWrapper::create( GfxStream::iterator begin, GfxStream::iterator end )
	{
		return new GfxStreamWrapper( (uint8_t*) begin, (uint8_t*) end );
	}


	//____ constructor _____________________________________________________________

	GfxStreamWrapper::GfxStreamWrapper( const void * pBegin, const void * pEnd ) : 
		output(this),
		m_pBegin((uint8_t*)pBegin),
		m_pEnd((uint8_t*)pEnd),
		m_dataSegment({ (uint8_t*)pBegin,(uint8_t*)pEnd })
	{
	}

	//____ Destructor _________________________________________________________

	GfxStreamWrapper::~GfxStreamWrapper()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamWrapper::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ bytesInBuffer() ____________________________________________________

	int GfxStreamWrapper::bytesInBuffer() const
	{
		return m_dataSegment.pEnd - m_dataSegment.pBegin;
	}


	//____ _hasStreamChunks() _________________________________________________

	bool GfxStreamWrapper::_hasStreamChunks()
	{
		uint64_t beg = (uint64_t) m_dataSegment.pBegin;
		uint64_t end = (uint64_t) m_dataSegment.pEnd;
		uint64_t diff = end -beg;
		
		
		return (m_dataSegment.pBegin != m_dataSegment.pEnd);
	}

	//____ _showStreamChunks() ________________________________________________

	std::tuple<int, const DataSegment*> GfxStreamWrapper::_showStreamChunks()
	{
		uint64_t beg = (uint64_t) m_dataSegment.pBegin;
		uint64_t end = (uint64_t) m_dataSegment.pEnd;
		uint64_t diff = end -beg;

		if (m_dataSegment.pBegin == m_dataSegment.pEnd)
			return std::make_tuple(int(0), nullptr);
		else
			return std::make_tuple(int(1), &m_dataSegment);
	}

	//____ _peekStreamChunk() __________________________________________________________

	GfxChunkId GfxStreamWrapper:: _peekStreamChunk()
	{
		if (m_dataSegment.pBegin == m_dataSegment.pEnd)
			return GfxChunkId::OutOfData;

		return GfxStream::chunkType((uint8_t*)(m_dataSegment.pBegin));
	}

	//____ _discardStreamChunks() _____________________________________________

	void GfxStreamWrapper::_discardStreamChunks(int bytes)
	{
		m_dataSegment.pBegin += bytes;
	}

	//____ _fetchStreamChunks() _______________________________________________

	bool GfxStreamWrapper::_fetchStreamChunks()
	{
		return false;
	}

} // namespace wg
