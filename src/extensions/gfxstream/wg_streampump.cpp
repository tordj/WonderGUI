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

#include <wg_streampump.h>
#include <wg_surface.h>
#include <wg_patches.h>
#include <wg_gfxbase.h>

#include <vector>
#include <cstdint>
#include <cstring>

using namespace std;

namespace wg
{
	const TypeInfo StreamPump::TYPEINFO = { "StreamPump", &Object::TYPEINFO };



	//____ create() _______________________________________________________________

	StreamPump_p StreamPump::create()
	{
		StreamPump_p p(new StreamPump( StreamSource_p(), StreamSink_p()));
		return p;
	}

	StreamPump_p	StreamPump::create( const StreamSource_p& pInput, const StreamSink_p& pOutput)
	{
		StreamPump_p p(new StreamPump(pInput, pOutput));
		return p;
	}


	//____ constructor _____________________________________________________________

	StreamPump::StreamPump(const StreamSource_p& pInput, const StreamSink_p& pOutput)
	{
		m_pInput = pInput;
		m_pOutput = pOutput;
	}

	//____ Destructor ______________________________________________________________

	StreamPump::~StreamPump()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StreamPump::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setInput() _________________________________________________________

	void StreamPump::setInput(const StreamSource_p& pStream)
	{
		m_pInput = pStream;
	}

	//____ setOutput() ________________________________________________________

	void StreamPump::setOutput(const StreamSink_p& pStream)
	{
		m_pOutput = pStream;
	}

	//____ peekChunk() ________________________________________________________

	GfxStream::ChunkId StreamPump::peekChunk()
	{
		if (!m_pInput)
			return GfxStream::ChunkId::OutOfData;

		return m_pInput->peekChunk();
	}

	//____ pumpChunk() ________________________________________________________

	bool StreamPump::pumpChunk()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const GfxStream::Data* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		if (m_pInput->hasChunks())
		{
			std::tie(nSegments, pSegments) = m_pInput->showChunks();
			int size = GfxStream::chunkSize( pSegments[0].pBegin );

			m_pOutput->processChunks( pSegments[0].pBegin, pSegments[0].pBegin + size );
			m_pInput->discardChunks(size);

			return true;
		}

		return false;
	}

	//____ pumpUntilFrame() ________________________________________________________

	bool StreamPump::pumpUntilFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		return _pumpUntilChunk(GfxStream::ChunkId::BeginRender, false);
	}

	//____ pumpFrame() ________________________________________________________

	bool StreamPump::pumpFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		if ( _fetchUntilChunk(GfxStream::ChunkId::EndRender) )
			return _pumpUntilChunk(GfxStream::ChunkId::EndRender, true);

		return false;
	}

	//____ _pumpUntilChunk() ___________________________________________________

	bool StreamPump::_pumpUntilChunk(GfxStream::ChunkId id, bool bInclusive)
	{
		int	nSegments;
		const GfxStream::Data* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		while (m_pInput->hasChunks())
		{
			std::tie(nSegments, pSegments) = m_pInput->showChunks();

			int	bytesProcessed = 0;
			for (int i = 0; i < nSegments; i++)
			{
				const uint8_t* pBegin = pSegments[i].pBegin;
				const uint8_t* pEnd = _findChunk(id, pSegments[i].pBegin, pSegments[i].pEnd);

				m_pOutput->processChunks(pBegin, pEnd);
				bytesProcessed += pEnd - pBegin;

				if (pEnd != pSegments[i].pEnd)
				{
					if (bInclusive)
					{
						bytesProcessed += GfxStream::chunkSize(pEnd);
						m_pOutput->processChunks(pEnd, pEnd + GfxStream::chunkSize(pEnd) );
					}

					m_pInput->discardChunks(bytesProcessed);
					return true;
				}
			}

			m_pInput->discardChunks(bytesProcessed);
			m_pInput->fetchChunks();
		}

		return false;
	}

	//____ _fetchUntilChunk() _________________________________________________

	bool StreamPump::_fetchUntilChunk(GfxStream::ChunkId id)
	{
		int	nSegments;
		const GfxStream::Data* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		while (m_pInput->hasChunks())
		{
			std::tie(nSegments, pSegments) = m_pInput->showChunks();

			for (int i = 0; i < nSegments; i++)
			{
				if (_findChunk(id, pSegments[i].pBegin, pSegments[i].pEnd) != pSegments[i].pEnd)
					return true;
			}
			
			if( !m_pInput->fetchChunks() )
				break;
		}

		return false;
	}

	//____ pumpAllFramesOptimizeClipping() ____________________________________________________

	bool StreamPump::pumpAllFramesOptimizeClipping( int optimizationDepth )
{
		// Fetch all data

		while (m_pInput->fetchChunks());

		// Show all chunks

		int	nSegments;
		const DataSegment* pSegments;
		std::tie(nSegments, pSegments) = m_pInput->showChunks();

		// Find end of last complete frame, that will be the scope of our work.

		const uint8_t* pLastFoundEndRender = nullptr;
		int nFrames = 0;
		int nFullSegments = 0;

		for (int i = 0; i < nSegments; i++)
		{
			auto p = _findChunk(GfxChunkId::EndRender, pSegments[i].pBegin, pSegments[i].pEnd );
			while (p != pSegments[i].pEnd)
			{
				nFullSegments = i;
				nFrames++;
				pLastFoundEndRender = p;
				p = _findChunk(GfxChunkId::EndRender, p + GfxStream::chunkSize(p), pSegments[i].pEnd);
			}
		}

		// Early out if we have one frame or less.

		if (nFrames == 0)
			return false;

		if (nFrames == 1)
			return _pumpUntilChunk(GfxChunkId::EndRender, true);

		// Collect our sessions and update rectangles







	}


	//____ pumpAll() __________________________________________________________

	bool StreamPump::pumpAll()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const GfxStream::Data* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		while (m_pInput->hasChunks())
		{
			std::tie(nSegments, pSegments) = m_pInput->showChunks();

			int	bytesProcessed = 0;
			for (int i = 0; i < nSegments; i++)
			{
				m_pOutput->processChunks(pSegments[i].pBegin, pSegments[i].pEnd);
				bytesProcessed += pSegments[i].pEnd - pSegments[i].pBegin;
			}

			m_pInput->discardChunks(bytesProcessed);

			m_pInput->fetchChunks();
		}

		return true;
	}

	//____ pumpBytes() __________________________________________________________

	int StreamPump::pumpBytes( int maxBytes )
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int bytesLeft = maxBytes;
		bool done = false;
		
		while (!done)
		{
			if (!m_pInput->hasChunks())
			{
				m_pInput->fetchChunks();
				if( !m_pInput->hasChunks())
					return maxBytes - bytesLeft;				// Out of data!
			}

			int	nSegments;
			const GfxStream::Data* pSegments;
			std::tie(nSegments, pSegments) = m_pInput->showChunks();

			int	bytesProcessed = 0;
			for (int i = 0; i < nSegments && !done; i++)
			{
				const uint8_t* pBegin = pSegments[i].pBegin;
				const uint8_t* pEnd = pSegments[i].pEnd;
				const uint8_t* p = pBegin;

				if( bytesLeft >= (pEnd - pBegin) )
					p = pEnd;
				else
				{
					int chunkSize = GfxStream::chunkSize(p);

					while( p + chunkSize - pBegin <= bytesLeft )
					{
						p += chunkSize;
						chunkSize = GfxStream::chunkSize(p);
					}

					done = true;
				}

				if( p > pBegin )
				{
					m_pOutput->processChunks(pBegin, p);
					bytesProcessed += p - pBegin;
					bytesLeft -= p - pBegin;
				}
				
			}

			m_pInput->discardChunks(bytesProcessed);
		}

		return maxBytes - bytesLeft;
	}


	//____ _findChunk() _______________________________________________________

	const uint8_t* StreamPump::_findChunk(GfxStream::ChunkId id, const uint8_t * pBegin, const uint8_t * pEnd)
	{
		const uint8_t* p = pBegin;

		while (p != pEnd)
		{
			GfxStream::ChunkId chunkId = GfxStream::chunkType(p);
			if (chunkId == id)
				break;
			p += GfxStream::chunkSize(p);
		}

		return p;
	}

} // namespace
