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
#include <wg_surface.h>
#include <wg_patches.h>

#include <vector>
#include <cstdint>

using namespace std;

namespace wg
{
	const TypeInfo GfxStreamPump::TYPEINFO = { "GfxStreamPump", &Object::TYPEINFO };



	//____ create() _______________________________________________________________

	GfxStreamPump_p GfxStreamPump::create()
	{
		GfxStreamPump_p p(new GfxStreamPump( GfxStreamSource_p(), GfxStreamSink_p()));
		return p;
	}

	GfxStreamPump_p	GfxStreamPump::create( const GfxStreamSource_p& pInput, const GfxStreamSink_p& pOutput)
	{
		GfxStreamPump_p p(new GfxStreamPump(pInput, pOutput));
		return p;
	}


	//____ constructor _____________________________________________________________

	GfxStreamPump::GfxStreamPump(const GfxStreamSource_p& pInput, const GfxStreamSink_p& pOutput)
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

	void GfxStreamPump::setInput(const GfxStreamSource_p& pStream)
	{
		m_pInput = pStream;
	}

	//____ setOutput() ________________________________________________________

	void GfxStreamPump::setOutput(const GfxStreamSink_p& pStream)
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
			int size = GfxStream::chunkSize( pSegments[0].pBegin );

			m_pOutput->processChunks( pSegments[0].pBegin, pSegments[0].pBegin + size );
			m_pInput->discardChunks(size);

			return true;
		}

		return false;
	}

	//____ pumpUntilFrame() ________________________________________________________

	bool GfxStreamPump::pumpUntilFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		int	nSegments;
		const DataSegment* pSegments;

		return _pumpUntilChunk(GfxChunkId::BeginRender, false);
	}

	//____ pumpFrame() ________________________________________________________

	bool GfxStreamPump::pumpFrame()
	{
		if (!m_pInput || !m_pOutput)
			return false;

		if ( _fetchUntilChunk(GfxChunkId::EndRender) )
			return _pumpUntilChunk(GfxChunkId::EndRender, true);

		return false;
	}

	//____ _pumpUntilChunk() ___________________________________________________

	bool GfxStreamPump::_pumpUntilChunk(GfxChunkId id, bool bInclusive)
	{
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

	bool GfxStreamPump::_fetchUntilChunk(GfxChunkId id)
	{
		int	nSegments;
		const DataSegment* pSegments;

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

	bool GfxStreamPump::pumpAllFramesOptimizeClipping()
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

		// Build up our canvas info

		struct CanvasFrame
		{
			const uint8_t*	pBegin;
			int				ofsClipRects;
			int				nClipRects;
		};

		struct CanvasData
		{
			CanvasData(int _id) : id(_id), framesPlayed(0) {}

			int id;				// surfaceId + canvasRef << 16;
			std::vector<CanvasFrame>	frames;
			int framesPlayed;
		};


		std::vector<CanvasData>	canvases;
		int totalRects = 0;

		for (int seg = 0; seg <= nFullSegments; seg++)
		{
			const uint8_t* pEnd = (seg == nFullSegments) ? pLastFoundEndRender : pSegments[seg].pEnd;
			auto p = _findChunk(GfxChunkId::BeginCanvasUpdate, pSegments[seg].pBegin, pEnd);
			while (p != pEnd)
			{
				// Get canvasRef and surfaceId from chunk, create our ID

				const uint8_t* pChunkData = p + GfxStream::headerSize(p);

				int chunkSize = GfxStream::chunkSize(p);

				uint16_t	surfaceId	= *(const uint16_t*)pChunkData;
				CanvasRef	canvasRef	= (CanvasRef) pChunkData[2];
				totalRects += pChunkData[3];

				int id = surfaceId + (int(canvasRef) << 16);

				// See if we already have this canvas, otherwise add it to our vector.

				int idx = 0;
				for( ; idx < canvases.size() ; idx++ )
				{
					if (canvases[idx].id == id)
						break;
				}

				if( idx == canvases.size() )
					canvases.push_back( id );

				// Add our frame to the list

				canvases[idx].frames.push_back({ p, 0, 0 } );

				// 

				p = _findChunk(GfxChunkId::BeginCanvasUpdate, p + GfxStream::chunkSize(p), pEnd);
			}
		}

		// Copy our rectangles from the stream, possibly mask clipping rectangles

		std::vector<RectSPX>	clipRects;

		for (auto& canvas : canvases)
		{
			int maskBegin = clipRects.size();				// Where canvases we mask against begin

			for (int i = canvas.frames.size() - 1; i >= 0; i--)
			{
				// Get data from header

				const uint8_t* pChunk = canvas.frames[i].pBegin;

				const uint8_t* pData = pChunk + GfxStream::headerSize(pChunk);

				int nRects = pData[3];
				RectSPX* pRects = (RectSPX*)(pData + 4);

				// Check if this frame completely redraws the canvas.

				if (nRects == 0)
				{
					canvas.frames[i].ofsClipRects = -1;							// Signal that this is a complete redraw.
					canvas.frames[i].nClipRects = 0;

					// All earlier frames can be skipped 

					i--;
					while (i >= 0)
					{
						canvas.frames[i].ofsClipRects = clipRects.size();
						canvas.frames[i].nClipRects = 0;
						i--;
					}

					break;
				}



				// Mask and copy rectangles for this frame

				int startOfs = clipRects.size();
		
				// For last frame we just copy the rects

				if (i == canvas.frames.size() - 1)
				{
					for (int r = 0; r < nRects; r++)
					{
						const RectI& rect = *pRects++;
						if( rect.w > 0 && rect.h > 0 )
							clipRects.push_back(rect);
					}
				}
				else
				{
					// For all other frames we mask against all existing rectangles

					for (int r = 0; r < nRects; r++)
					{
						const RectI& rect = *pRects++;
						if (rect.w > 0 && rect.h > 0)
							_maskAddRect(clipRects, maskBegin, rect);
					}
				}

				canvas.frames[i].ofsClipRects = startOfs;
				canvas.frames[i].nClipRects = clipRects.size() - startOfs;
			}
		}

		// Play stream, replacing BeginRenderCanvas with our opimized one and
		// filtering set/push clip-rect against canvas cliplist.


		uint8_t	tempChunk[16 + 16 * 256];		// Maximum of 256 rectangles... 

		RectSPX*	pActiveUpdateRects = nullptr;
		int			nActiveUpdateRects = 0;

		int			bytesToDiscard = 0;

		for (int seg = 0; seg <= nFullSegments; seg++)
		{
			const uint8_t* pBegin = pSegments[seg].pBegin;
			const uint8_t* pEnd = (seg == nFullSegments) ? pLastFoundEndRender + GfxStream::chunkSize(pLastFoundEndRender) : pSegments[seg].pEnd;

			bytesToDiscard += pEnd - pBegin;

			while (true)
			{
				auto p = pBegin;

				GfxChunkId chunkType = GfxStream::chunkType(p);
				while (p != pEnd )
				{
					if (chunkType == GfxChunkId::BeginCanvasUpdate || chunkType == GfxChunkId::SetClipList || chunkType == GfxChunkId::PushClipList)
						break;
					p += GfxStream::chunkSize(p);
					chunkType = GfxStream::chunkType(p);
				}

				if (p != pBegin)
					m_pOutput->processChunks(pBegin, p);

				if (p == pEnd)
					break;

				if (chunkType == GfxChunkId::BeginCanvasUpdate)
				{
					// Find the optimized rects for our canvas

					auto pChunkData = p + GfxStream::headerSize(p);

					uint16_t	surfaceId = *(uint16_t*) pChunkData;
					CanvasRef	canvasRef = (CanvasRef) pChunkData[2];

					int id = surfaceId + (int(canvasRef) << 16);

					CanvasData* pCanvasData = canvases.data();
					while ( pCanvasData->id != id)
						pCanvasData++;

					CanvasFrame* pFrameData = &pCanvasData->frames[pCanvasData->framesPlayed++];

					// Take care of special cases: Full canvas redraw and no canvas update.

					if (pFrameData->nClipRects == 0)
					{
						if (pFrameData->ofsClipRects == -1)
						{
							m_pOutput->processChunks(p, p + GfxStream::chunkSize(p) );
							pBegin = p + GfxStream::chunkSize(p);
							continue;
						}
						else
						{
							//TODO: Skip whole surface update, unless it contains another surface update.
						}

					}


					nActiveUpdateRects = pFrameData->nClipRects;

					if( nActiveUpdateRects == 0 )
					{
						// No updates for this canvas, just skip it
						
						int nCanvasUpdate = 1;
						p += GfxStream::chunkSize(p);
						
						while( nCanvasUpdate > 0 )
						{
							GfxChunkId type = GfxStream::chunkType(p);
							if( type == GfxChunkId::BeginCanvasUpdate )
								nCanvasUpdate++;
							else if( type == GfxChunkId::EndCanvasUpdate )
								nCanvasUpdate--;
							
							p += GfxStream::chunkSize(p);
						}
					}
					else
					{
						pActiveUpdateRects = clipRects.data() + pFrameData->ofsClipRects;

						
						
						// Create our own BeginCanvasUpdate

						int dataSize = nActiveUpdateRects * sizeof(RectSPX) + 4;

						tempChunk[0] = (uint8_t) GfxChunkId::BeginCanvasUpdate;
						tempChunk[1] = 31;		// Force long header.
						*(uint16_t*)(tempChunk + 2) = dataSize;
						*(uint16_t*)(tempChunk + 4) = surfaceId;
						tempChunk[6] = (uint8_t) canvasRef;
						tempChunk[7] = nActiveUpdateRects;

						std::memcpy(tempChunk + 8, pActiveUpdateRects, nActiveUpdateRects * sizeof(RectSPX));
						
						// Process our created chunk

						m_pOutput->processChunks(tempChunk, tempChunk + 4 + dataSize);
					}

				}
				else // PushClipList or SetClipList
				{
					// Get info from the chunk

					int nSrcRect = GfxStream::dataSize(p) / sizeof(RectSPX);
					const RectSPX* pSrcRect = (const RectSPX*)(p + GfxStream::headerSize(p));

					// Create our own PushClipList/SetClipList, start by filtering and pushing rectangles.

					RectSPX* pDst = (RectSPX*)(tempChunk + 4);

					for (int i = 0; i < nSrcRect; i++)
					{
						for (int j = 0; j < nActiveUpdateRects; j++)
						{
							if (pSrcRect[i].intersectsWith(pActiveUpdateRects[j]) )
								*pDst++ = RectSPX::getIntersection(pSrcRect[i], pActiveUpdateRects[j]);
						}
					}

					int nDstRect = pDst - (RectSPX*)(tempChunk + 4);

					// Create the header

					tempChunk[0] = (uint8_t) chunkType;
					tempChunk[1] = 31;		// Force long header.
					*((uint16_t*)(tempChunk + 2)) = nDstRect*sizeof(RectSPX);

					// Process our created chunk

					m_pOutput->processChunks( tempChunk, (uint8_t*) pDst );

				}

				pBegin = p + GfxStream::chunkSize(p);
			}
		}

		// Discard our processed chunks

		m_pInput->discardChunks(bytesToDiscard);
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

	const uint8_t* GfxStreamPump::_findChunk(GfxChunkId id, const uint8_t * pBegin, const uint8_t * pEnd)
	{
		const uint8_t* p = pBegin;

		while (p != pEnd)
		{
			GfxChunkId chunkId = GfxStream::chunkType(p);
			if (chunkId == id)
				break;
			p += GfxStream::chunkSize(p);
		}

		return p;
	}


	//____ _maskAddRect() __________________________________________________________________

	void GfxStreamPump::_maskAddRect(std::vector<RectI>& vRects, int startOffset, const RectI& rect)
	{

		for (int i = startOffset; i < vRects.size(); i++)
		{
			RectI* pR = vRects.data() + i;

			// Bail out early if no intersection at all.

			if ((rect.x >= pR->x + pR->w) || (rect.x + rect.w <= pR->x) ||
				(rect.y >= pR->y + pR->h) || (rect.y + rect.h <= pR->y) )
				continue;															// No intersection.

			// Check for total coverage

			if ((rect.x >= pR->x) && (rect.x + rect.w <= pR->x + pR->w) &&
				(rect.y >= pR->y) && (rect.y + rect.h <= pR->y + pR->h) )
				return;  															// rect totally covered by pR


			// Clip newR against pR.

			RectI newR = rect;

			RectI mask = *pR;

			// Cut off upper part

			if (newR.y < mask.y)
			{
				RectI xR(newR.x, newR.y, newR.w, mask.y - newR.y);
				_maskAddRect(vRects, i + 1, xR );

				newR.h -= xR.h;
				newR.y += xR.h;
			}

			// Cut off lower part

			if (newR.y + newR.h > mask.y + mask.h )
			{
				RectI	xR;
				xR.x = newR.x;
				xR.y = mask.y + mask.h;
				xR.w = newR.w;
				xR.h = (newR.y + newR.h) - (mask.y + mask.h);
				_maskAddRect(vRects, i + 1, xR);

				newR.h -= xR.h;
			}

			if (newR.h > 0)
			{
				// Cut off left part

				if (newR.x < mask.x)
				{
					RectI	xR;
					xR.x = newR.x;
					xR.y = newR.y;
					xR.w = mask.x - newR.x;
					xR.h = newR.h;

					_maskAddRect(vRects, i + 1, xR);
				}

				// Cut off right part

				if (newR.x + newR.w > mask.x + mask.w)
				{
					RectI	xR;
					xR.x = mask.x + mask.w;
					xR.y = newR.y;
					xR.w = (newR.x + newR.w) - (mask.x + mask.w);
					xR.h = newR.h;

					_maskAddRect(vRects, i + 1, xR);
				}
			}

			// We have split our rectangle into visible pieces and masked them one by one.
			// There is nothing left of original rectangle.

			return;	

		}

		// If we haven't returned yet we have a patch left to add.

		vRects.push_back(rect);
	}


} // namespace
