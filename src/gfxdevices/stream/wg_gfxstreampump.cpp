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

	GfxStreamPump_p	GfxStreamPump::create(CGfxInStream* pInput, CGfxOutStream* pOutput)
	{
		GfxStreamPump_p p(new GfxStreamPump(pInput, pOutput));
		return p;
	}


	//____ constructor _____________________________________________________________

	GfxStreamPump::GfxStreamPump(CGfxInStream* pInput, CGfxOutStream* pOutput)
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

	//____ pumpFrame() ________________________________________________________

	int frameCounter = 0;

	bool GfxStreamPump::pumpFrame()
	{
		frameCounter++;
		if (frameCounter == 275)
		{
			int dummy = 0;
		}

		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const DataSegment* pSegments;

		if (!m_pInput->hasChunks())
			m_pInput->fetchChunks();

		std::tie(nSegments, pSegments) = m_pInput->showChunks();
		const uint8_t* pEndRenderPos = _findChunk(GfxChunkId::EndRender, nSegments, pSegments);

		while ( pEndRenderPos == nullptr)
		{
			if (!m_pInput->fetchChunks())
				return false;

			std::tie(nSegments, pSegments) = m_pInput->showChunks();
			pEndRenderPos = _findChunk(GfxChunkId::EndRender, nSegments, pSegments);
		}

		//

		auto header = _decodeHeader(pEndRenderPos);

		const uint8_t* pEnd = pEndRenderPos + header.size + 4;

		int	bytesProcessed = 0;
		for (int i = 0; i < nSegments; i++)
		{
			if (pEnd > pSegments[i].pBegin && pEnd <= pSegments[i].pEnd)
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
