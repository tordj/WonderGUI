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

#include <wg_gfxstreamloopwrapper.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamLoopWrapper::TYPEINFO = { "GfxStreamLoopWrapper", &Object::TYPEINFO };

	//____ create() ___________________________________________________________

	GfxStreamLoopWrapper_p GfxStreamLoopWrapper::create(const void * pBufferBegin, const void * pBufferEnd,
														std::function<const void *()> getWritePtrFunc,
														std::function<void(const void * pReadPos)> setReadPtrFunc )
	{
		return new GfxStreamLoopWrapper(pBufferBegin, pBufferEnd, getWritePtrFunc, setReadPtrFunc);
	}

	//____ constructor _____________________________________________________________

	GfxStreamLoopWrapper::GfxStreamLoopWrapper(const void * pBufferBegin, const void * pBufferEnd,
											   std::function<const void *()> getWritePtrFunc,
											   std::function<void(const void * pReadPos)> setReadPtrFunc ) : output(this)
	{
		m_pBufferBegin = (const uint8_t*) pBufferBegin;
		m_pBufferEnd = (const uint8_t*) pBufferEnd;
		
		m_pBeginChunks = (const uint8_t*) pBufferBegin;
		m_pEndChunks = (const uint8_t*) pBufferBegin;
		
		m_getWritePtrFunc = getWritePtrFunc;
		m_setReadPtrFunc = setReadPtrFunc;
	}

	//____ Destructor _________________________________________________________

	GfxStreamLoopWrapper::~GfxStreamLoopWrapper()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamLoopWrapper::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _hasStreamChunks() _________________________________________________

	bool GfxStreamLoopWrapper::_hasStreamChunks()
	{
		return m_pBeginChunks != m_pEndChunks;
	}

	//____ _showStreamChunks() ________________________________________________

	std::tuple<int, const DataSegment*> GfxStreamLoopWrapper::_showStreamChunks()
	{
		if( m_pEndChunks == m_pBeginChunks )
			return std::make_tuple(int(0), nullptr);

		m_dataSegments[0].pBegin = (uint8_t *) m_pBeginChunks;

		if( m_pSplitChunkBegin )
		{
			m_dataSegments[0].pEnd = (uint8_t*) m_pSplitChunkBegin;

			m_dataSegments[1].pBegin = m_rejoinedChunk;
			m_dataSegments[1].pEnd = m_rejoinedChunk + m_rejoinedChunkSize;
			
			if( m_pSplitChunkEnd == m_pEndChunks )
				return std::make_tuple(int(2), m_dataSegments);

			m_dataSegments[2].pBegin = (uint8_t*) m_pSplitChunkEnd;
			m_dataSegments[2].pEnd = (uint8_t*) m_pEndChunks;

			return std::make_tuple(int(3), m_dataSegments);
		}
		else if( m_pEndChunks < m_pBeginChunks )
		{
			m_dataSegments[0].pEnd = (uint8_t*) m_pBufferEnd;

			m_dataSegments[1].pBegin = (uint8_t*) m_pBufferBegin;
			m_dataSegments[1].pEnd = (uint8_t*) m_pEndChunks;

			return std::make_tuple(int(2), m_dataSegments);
		}
		else
		{
			m_dataSegments[0].pEnd = (uint8_t*) m_pEndChunks;
			
			return std::make_tuple(int(1), m_dataSegments);
		}
	}

	//____ _peekStreamChunk() __________________________________________________________

	GfxChunkId GfxStreamLoopWrapper:: _peekStreamChunk()
	{
		if( m_pBeginChunks == m_pEndChunks )
			return GfxChunkId::OutOfData;
		else
			return GfxStream::chunkType(m_pBeginChunks);
	}

	//____ _discardStreamChunks() _____________________________________________

	void GfxStreamLoopWrapper::_discardStreamChunks(int bytes)
	{
		const uint8_t * p = m_pBeginChunks;
		p += bytes;
		if( p >= m_pBufferEnd )
		{
			p -= (m_pBufferEnd - m_pBufferBegin);
			
			m_rejoinedChunkSize = 0;
			m_pSplitChunkBegin = nullptr;
			m_pSplitChunkEnd = nullptr;
		}
		
		m_pBeginChunks = p;
		
		m_setReadPtrFunc(p);
	}

	//____ _fetchStreamChunks() _______________________________________________

	bool GfxStreamLoopWrapper::_fetchStreamChunks()
	{
		const uint8_t *  pDataEnd = (const uint8_t*) m_getWritePtrFunc();

		const uint8_t *	pChunk = m_pEndChunks;
		
		if( pDataEnd < pChunk )
		{
			int endBufferBytes = m_pBufferEnd - pChunk;
			
			// Advance pChunk over chunks to the last, incomplete chunk in the buffer
			
			while( endBufferBytes > 2 )
			{
				// Get chunk size
				
				int chunkSize;
				
				uint8_t sizeEtc = pChunk[1] & 0x1F;
				if (sizeEtc <= 30)
					chunkSize = sizeEtc + 2;
				else
				{
					if( endBufferBytes < 4 )
						break;
					
					chunkSize = ((uint16_t*) pChunk)[1] + 4;
				}
				
				//
				
				if( chunkSize < endBufferBytes )
				{
					pChunk += chunkSize;
					endBufferBytes -= chunkSize;
				}
				else
					break;
			}
			
			if( pChunk < m_pBufferEnd )
			{
				// We have an broken chunk at the end
				
				int dataSize = (m_pBufferEnd - pChunk) + (pDataEnd - m_pBufferBegin);

				int chunkSize;
				
				uint8_t sizeEtc = pChunk[1] & 0x1F;
				if (sizeEtc <= 30)
					chunkSize = sizeEtc + 2;
				else
				{
					if( endBufferBytes >= 4 )
						chunkSize = ((uint16_t*) pChunk)[1] + 4;
					else if( dataSize >= 4 )
						chunkSize = ((uint16_t*) m_pBufferBegin)[0] + 4;
					else
						goto out;
				}

				// Leave if chunk is not complete
				
				if( chunkSize > dataSize )
					goto out;
				
				// Rejoin our chunk, set pointer
				
				int part1Size = m_pBufferEnd - pChunk;
				memcpy( m_rejoinedChunk, pChunk, part1Size );
				memcpy( m_rejoinedChunk + part1Size, m_pBufferBegin, chunkSize - part1Size );
				
				m_rejoinedChunkSize = chunkSize;
				m_pSplitChunkBegin = pChunk;
			
				pChunk = m_pBufferBegin + chunkSize - part1Size;
				m_pSplitChunkEnd = pChunk;

			}
			else
			{
				pChunk = m_pBufferBegin;
			}
		}
		
		//
		
		while( pChunk + 2 <= pDataEnd )
		{
			int chunkSize;
			
			uint8_t sizeEtc = pChunk[1] & 0x1F;
			if (sizeEtc <= 30)
				chunkSize = sizeEtc + 2;
			else
			{
				if( pChunk + 4 <= pDataEnd )
					chunkSize = ((uint16_t*) pChunk)[1] + 4;
				else
					break;
			}
			
			if( pChunk + chunkSize <= pDataEnd )
				pChunk += chunkSize;
			else
				break;
		}
out:
		if( pChunk != m_pEndChunks )
		{
			m_pEndChunks = pChunk;
			return true;
		}
		else
			return false;

	}


} // namespace wg