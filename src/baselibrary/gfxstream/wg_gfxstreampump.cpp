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

#include <wg_gfxstreampump.h>
#include <wg_patches.h>

using namespace std;

namespace wg
{
	const TypeInfo GfxStreamPump::TYPEINFO = { "GfxStreamPump", &Object::TYPEINFO };



	//____ create() _______________________________________________________________

	GfxStreamPump_p GfxStreamPump::create()
	{
		GfxStreamPump_p p(new GfxStreamPump(nullptr,nullptr));
		return p;
	}

	GfxStreamPump_p	GfxStreamPump::create(CGfxStreamSource* pInput, CGfxStreamSink* pOutput)
	{
		GfxStreamPump_p p(new GfxStreamPump(pInput, pOutput));
		return p;
	}


	//____ constructor _____________________________________________________________

	GfxStreamPump::GfxStreamPump(CGfxStreamSource* pInput, CGfxStreamSink* pOutput)
	{
		m_pInput = pInput;
		m_pOutput = pOutput;
	}

	//____ Destructor ______________________________________________________________

	GfxStreamPump::~GfxStreamPump()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamPump::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setInput() _________________________________________________________

	void GfxStreamPump::setInput(CGfxStreamSource* pStream)
	{
		m_pInput = pStream;
	}

	//____ setOutput() ________________________________________________________

	void GfxStreamPump::setOutput(CGfxStreamSink* pStream)
	{
		m_pOutput = pStream;
	}

	//____ peekChunk() ________________________________________________________

	GfxChunkId GfxStreamPump::peekChunk()
	{
		if (!m_pInput)
			return GfxChunkId::OutOfData;

		return m_pInput->peekChunk();
	}

	//____ pumpChunk() ________________________________________________________

	bool GfxStreamPump::pumpChunk()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const DataSegment* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		if (m_pInput->hasChunks())
		{
			std::tie(nSegments, pSegments) = m_pInput->showChunks();
			auto header = _decodeHeader( pSegments[0].pBegin );

			m_pOutput->processChunks( pSegments[0].pBegin, pSegments[0].pBegin + header.size + 4 );
			m_pInput->discardChunks(header.size + 4);

			return true;
		}

		return false;
	}

	//____ pumpUntilFrame() ________________________________________________________

	bool GfxStreamPump::pumpUntilFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		return _pumpUntilChunk(GfxChunkId::BeginRender, false);
	}

	//____ pumpFrame() ________________________________________________________

	bool GfxStreamPump::pumpFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		return _pumpUntilChunk(GfxChunkId::EndRender, true);
	}

	//____ _pumpUntilChunk() ___________________________________________________

	bool GfxStreamPump::_pumpUntilChunk(GfxChunkId id, bool bInclusive)
	{
		int	nSegments;
		const DataSegment* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		std::tie(nSegments, pSegments) = m_pInput->showChunks();
		const uint8_t* pEndRenderPos = _findChunk(id, nSegments, pSegments);

		while ( pEndRenderPos == nullptr)
		{
			if (!m_pInput->fetchChunks())
				return false;

			std::tie(nSegments, pSegments) = m_pInput->showChunks();
			pEndRenderPos = _findChunk(id, nSegments, pSegments);
		}

		//

		auto header = _decodeHeader(pEndRenderPos);

		const uint8_t* pEnd = pEndRenderPos + (bInclusive ? header.size + 4 : 0);

		int	bytesProcessed = 0;
		for (int i = 0; i < nSegments; i++)
		{
			if (pEnd >= pSegments[i].pBegin && pEnd <= pSegments[i].pEnd)
			{
				m_pOutput->processChunks(pSegments[i].pBegin, pEnd);
				bytesProcessed += pEnd - pSegments[i].pBegin;
				break;
			}
			else
			{
				m_pOutput->processChunks(pSegments[i].pBegin, pSegments[i].pEnd);
				bytesProcessed += pSegments[i].pEnd - pSegments[i].pBegin;
			}
		}

		m_pInput->discardChunks(bytesProcessed);
		return true;
	}

	//____ pumpAllFramesWithOptimization() ____________________________________________________
/*
	bool GfxStreamPump::pumpAllFramesWithOptimization()
	{
		// Fetch all data

		while (m_pInput->fetchChunks());

		// Show all chunks

		int	nSegments;
		const DataSegment* pSegments;
		std::tie(nSegments, pSegments) = m_pInput->showChunks();

		// Make a list of all complete frames.

		struct BeginCanvasInfo
		{
			const uint8_t*	pOrgChunk;
			int				orgChunkLength;

			uint8_t*		pFixedChunk;


			Patches			dirtyRects;

		};



	}
*/



	//____ pumpAll() __________________________________________________________

	bool GfxStreamPump::pumpAll()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const DataSegment* pSegments;

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

	//____ _findChunk() _______________________________________________________

	const uint8_t* GfxStreamPump::_findChunk(GfxChunkId id, int nSegments, const DataSegment* pSegments)
	{
		for (int seg = 0; seg < nSegments; seg++)
		{
			const uint8_t* p = pSegments[seg].pBegin;

			while (p != pSegments[seg].pEnd)
			{
				auto header = _decodeHeader(p);
				if (header.type == id)
					return p;
				p += header.size + 4;
			}
		}

		return nullptr;
	}


} // namespace
