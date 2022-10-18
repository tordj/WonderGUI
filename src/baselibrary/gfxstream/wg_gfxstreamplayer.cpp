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

#include <wg_gfxstreamplayer.h>
#include <wg_base.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamPlayer::TYPEINFO = { "GfxStreamPlayer", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamPlayer_p GfxStreamPlayer::create(GfxDevice * pDevice, SurfaceFactory * pFactory)
	{
		return new GfxStreamPlayer(pDevice, pFactory);
	}

	//____ constructor _____________________________________________________________

	GfxStreamPlayer::GfxStreamPlayer(GfxDevice * pDevice, SurfaceFactory * pFactory) : input(this)
	{
		m_pDevice = pDevice;
		m_pSurfaceFactory = pFactory;

		m_pDecoder = GfxStreamDecoder::create();

		m_clipListBuffer.pRects = new RectI[c_clipListBufferSize];
		m_clipListBuffer.nRects = 0;
		m_clipListBuffer.capacity = c_clipListBufferSize;
	}

	//____ Destructor _________________________________________________________

	GfxStreamPlayer::~GfxStreamPlayer()
	{
		delete[] m_clipListBuffer.pRects;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamPlayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setStoreDirtyRects() _______________________________________________

	void GfxStreamPlayer::setStoreDirtyRects(bool bStore)
	{
		if (bStore == m_bStoreDirtyRects)
			return;

		m_bStoreDirtyRects = bStore;
		if (!bStore)
			clearDirtyRects();
	}

	//____ setMaxDirtyRects() ____________________________________________________

	void GfxStreamPlayer::setMaxDirtyRects(int max)
	{
		m_maxDirtyRects = max;
	}

	//____ dirtyRects() __________________________________________________________

	std::tuple<int, const RectSPX*> GfxStreamPlayer::dirtyRects(CanvasRef canvas)
	{
		const PatchesSPX& patches = m_dirtyRects[(int)canvas];

		int size = patches.size();
		const RectSPX* pRects = patches.begin();
		return std::tie(size, pRects);
	}

	//____ clearDirtyRects() _____________________________________________________

	void GfxStreamPlayer::clearDirtyRects()
	{
		for (int i = 0; i < CanvasRef_size; i++)
			m_dirtyRects[i].clear();
	}

	//____ setCanvasInfoCallback() ________________________________________________

	void GfxStreamPlayer::setCanvasInfoCallback( std::function<void(const CanvasInfo * pBegin, const CanvasInfo * pEnd)>& callback )
	{
		m_canvasInfoCallback = callback;
	}

	//____ _processStreamChunks() _____________________________________________

	void GfxStreamPlayer::_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		m_pDecoder->setInput(pBegin, pEnd);

		while (_playChunk() == true);
	}


	//____ _playChunk() ____________________________________________________________

	bool GfxStreamPlayer::_playChunk()
	{
		GfxStream::Header header;

		*m_pDecoder >> header;

		switch (header.type)
		{

		case GfxChunkId::OutOfData:
			return false;

		case GfxChunkId::ProtocolVersion:
		{
			uint16_t	version;
			*m_pDecoder >> version;
			
			//TODO: Something if version isn't supported.

			break;
		}

		case GfxChunkId::CanvasList:
		{
			uint16_t nbCanvases;
			
			*m_pDecoder >> nbCanvases;
			
			CanvasInfo	canvas[CanvasRef_size];
			
			for( int i = 0 ; i < nbCanvases ; i++ )
			{
				uint16_t 	ref;
				SizeSPX		size;
				uint16_t	scale;

				*m_pDecoder >> ref;
				*m_pDecoder >> size;
				*m_pDecoder >> scale;
				
				canvas[i].ref = (CanvasRef) ref;
				canvas[i].size = size;
				canvas[i].scale = scale;
			}

			if( m_canvasInfoCallback )
				m_canvasInfoCallback(canvas, canvas+nbCanvases);
		}

        case GfxChunkId::BeginRender:
            m_pDevice->beginRender();
            break;

        case GfxChunkId::EndRender:
			assert(m_clipListBufferStack.empty() && m_clipListInfoStack.empty() && m_clipListBuffer.nRects == 0);
            m_pDevice->endRender();
            break;
                
        case GfxChunkId::Flush:
            m_pDevice->flush();
            break;
             
		case GfxChunkId::BeginCanvasUpdate:
		{
			CanvasRef	canvasRef;
			uint16_t	surfaceId;
			int			nUpdateRects;
				
 			*m_pDecoder >> canvasRef;
			*m_pDecoder >> surfaceId;
			*m_pDecoder >> nUpdateRects;

			auto pRects = _pushClipListCanvas(nUpdateRects);

			if( surfaceId > 0 )
				m_pDevice->beginCanvasUpdate(m_vSurfaces[surfaceId], nUpdateRects, pRects );
			else
				m_pDevice->beginCanvasUpdate(canvasRef, nUpdateRects, pRects );

			break;
		}
				
		case GfxChunkId::EndCanvasUpdate:
		{

			const CanvasInfo& canvasInfo = m_pDevice->canvas();

			if (m_bStoreDirtyRects && canvasInfo.ref != CanvasRef::None )
			{
				m_pDevice->resetClipList();								// Returns cliplist to that of canvas.

				int nDirtyRects = m_pDevice->clipListSize();
				const RectSPX* pDirtyRects = m_pDevice->clipList();

				PatchesSPX& patches = m_dirtyRects[(int)canvasInfo.ref];
				if (patches.isEmpty())
				{
					if (nDirtyRects > m_maxDirtyRects)
					{
						patches.clear();
						patches.push(m_pDevice->clipBounds());
					}
					else
					{
						for (int i = 0; i < nDirtyRects; i++)
							patches.push(pDirtyRects[i]);
					}
				}
				else
				{
					for (int i = 0; i < nDirtyRects; i++)
						patches.add(pDirtyRects[i]);

					if (patches.size() > m_maxDirtyRects)
					{
						RectSPX u = patches.getUnion();
						patches.clear();
						patches.push(u);
					}
				}
			}

			m_pDevice->endCanvasUpdate();
			_popClipListCanvas();
			break;
		}

		case GfxChunkId::SetClipList:
		{
			int nRects = header.size / 16;

			auto pRects = _setClipList(nRects);

			m_pDevice->setClipList(nRects, pRects);
			break;
		}

		case GfxChunkId::ResetClipList:
			m_pDevice->resetClipList();
			break;

		case GfxChunkId::PushClipList:
		{
			int nRects = header.size / 16;

			auto pRects = _pushClipList(nRects);
			m_pDevice->pushClipList(nRects, pRects);
			break;
		}
			
		case GfxChunkId::PopClipList:
			_popClipList();
			m_pDevice->popClipList();
			break;

		case GfxChunkId::SetTintColor:
		{
			HiColor	col;
			*m_pDecoder >> col;
			m_pDevice->setTintColor(col);
			break;
		}

		case GfxChunkId::SetTintGradient:
		{
			RectI rect;
			Gradient gradient;
			
			*m_pDecoder >> rect;
			*m_pDecoder >> gradient;
			
			m_pDevice->setTintGradient(rect, gradient );
		   break;
		}

		case GfxChunkId::ClearTintGradient:
			m_pDevice->clearTintGradient();
			break;

		case GfxChunkId::SetBlendMode:
		{
			BlendMode	blendMode;
			*m_pDecoder >> blendMode;
			m_pDevice->setBlendMode(blendMode);
			break;
		}

		case GfxChunkId::SetBlitSource:
		{
			uint16_t	surfaceId;
			*m_pDecoder >> surfaceId;

			m_pDevice->setBlitSource(m_vSurfaces[surfaceId]);
			break;
		}

		case GfxChunkId::SetMorphFactor:
		{
			float	factor;

			*m_pDecoder >> factor;
			
			m_pDevice->setRenderLayer(factor);
			break;
		}

		case GfxChunkId::SetRenderLayer:
		{
			uint16_t	layer;

			*m_pDecoder >> layer;
			
			m_pDevice->setRenderLayer(layer);
			break;
		}

		case GfxChunkId::Fill:
		{
			HiColor	col;

			*m_pDecoder >> col;

			m_pDevice->fill(col);
			break;
		}
			
		case GfxChunkId::FillRect:
		{
			RectSPX	rect;
			HiColor	col;

			*m_pDecoder >> rect;
			*m_pDecoder >> col;

			m_pDevice->fill(rect, col);
			break;
		}

		case GfxChunkId::PlotPixels:
		{
			int nPixels = header.size / 16;

			int bufferSize = header.size;
			char * pBuffer = reinterpret_cast<char*>(Base::memStackAlloc(bufferSize));

			// Load all data to buffer

			*m_pDecoder >> GfxStream::DataChunk{ header.size, pBuffer };

			//

			m_pDevice->plotPixels(nPixels, (CoordI*)pBuffer, (HiColor*)(pBuffer + header.size / 2));

			Base::memStackRelease(bufferSize);
			break;
		}

		case GfxChunkId::DrawLineFromTo:
		{
			CoordSPX	begin;
			CoordSPX	end;
			HiColor		color;
			spx			thickness;

			*m_pDecoder >> begin;
			*m_pDecoder >> end;
			*m_pDecoder >> color;
			*m_pDecoder >> thickness;

			m_pDevice->drawLine(begin, end, color, thickness);
			break;
		}

		case GfxChunkId::DrawLineStraight:
		{
			CoordSPX	begin;
			Direction	dir;
			spx			length;
			HiColor		color;
			spx			thickness;

			*m_pDecoder >> begin;
			*m_pDecoder >> dir;
			*m_pDecoder >> length;
			*m_pDecoder >> color;
			*m_pDecoder >> thickness;

			m_pDevice->drawLine(begin, dir, length, color, thickness);
			break;
		}

		case GfxChunkId::Blit:
		{
			CoordSPX	dest;

			*m_pDecoder >> dest;

			m_pDevice->blit(dest);
			break;
		}

		case GfxChunkId::BlitRect:
		{
			CoordSPX	dest;
			RectSPX		source;

			*m_pDecoder >> dest;
			*m_pDecoder >> source;

			m_pDevice->blit(dest,source);
			break;
		}

		case GfxChunkId::FlipBlit:
		{
			CoordSPX	dest;
			GfxFlip		flip;

			*m_pDecoder >> dest;
			*m_pDecoder >> flip;

			m_pDevice->flipBlit(dest,flip);
			break;
		}

		case GfxChunkId::FlipBlitRect:
		{
			CoordSPX	dest;
			RectSPX		source;
			GfxFlip		flip;

			*m_pDecoder >> dest;
			*m_pDecoder >> source;
			*m_pDecoder >> flip;

			m_pDevice->flipBlit(dest,source,flip);
			break;
		}

		case GfxChunkId::StretchBlit:
		{
			RectSPX		dest;

			*m_pDecoder >> dest;

			m_pDevice->stretchBlit(dest);
			break;
		}

		case GfxChunkId::StretchBlitRect:
		{
			RectSPX		dest;
			RectSPX		source;

			*m_pDecoder >> dest;
			*m_pDecoder >> source;

			m_pDevice->stretchBlit(dest, source);
			break;
		}
			
		case GfxChunkId::StretchFlipBlit:
		{
			RectSPX		dest;
			GfxFlip		flip;

			*m_pDecoder >> dest;
			*m_pDecoder >> flip;

			m_pDevice->stretchFlipBlit(dest, flip);
			break;
		}

		case GfxChunkId::StretchFlipBlitRect:
		{
			RectSPX		dest;
			RectSPX		source;
			GfxFlip		flip;

			*m_pDecoder >> dest;
			*m_pDecoder >> source;
			*m_pDecoder >> flip;

			m_pDevice->stretchFlipBlit(dest, source, flip);
			break;
		}

		case GfxChunkId::PrecisionBlit:
		{
			RectSPX		dest;
			RectF		source;

			*m_pDecoder >> dest;
			*m_pDecoder >> source;

			m_pDevice->precisionBlit(dest, source);
			break;
		}
				
		case GfxChunkId::TransformBlit:
		{
			RectSPX		dest;
			CoordF		source;
			float		transform[2][2];

			*m_pDecoder >> dest;
			*m_pDecoder >> source;
			*m_pDecoder >> transform[0][0];
			*m_pDecoder >> transform[0][1];
			*m_pDecoder >> transform[1][0];
			*m_pDecoder >> transform[1][1];

			m_pDevice->transformBlit(dest, source, transform);
			break;
		}

		case GfxChunkId::RotScaleBlit:
		{
			RectSPX		dest;
			float		rotationDegrees;
			float		scale;
			CoordF		srcCenter;
			CoordF		destCenter;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> rotationDegrees;
			*m_pDecoder >> scale;
			*m_pDecoder >> srcCenter;
			*m_pDecoder >> destCenter;

			m_pDevice->rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter );
			break;
		}

		case GfxChunkId::Tile:
		{
			RectSPX		dest;
			CoordSPX	shift;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> shift;

			m_pDevice->tile(dest, shift);
			break;
		}

		case GfxChunkId::FlipTile:
		{
			RectSPX		dest;
			GfxFlip		flip;
			CoordSPX	shift;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> flip;
			*m_pDecoder >> shift;

			m_pDevice->flipTile(dest, flip, shift);
			break;
		}
			
		case GfxChunkId::ScaleTile:
		{
			RectSPX		dest;
			float		scale;
			CoordSPX	shift;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> scale;
			*m_pDecoder >> shift;

			m_pDevice->scaleTile(dest, scale, shift);
			break;
		}

		case GfxChunkId::ScaleFlipTile:
		{
			RectSPX		dest;
			float		scale;
			GfxFlip		flip;
			CoordSPX	shift;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> scale;
			*m_pDecoder >> flip;
			*m_pDecoder >> shift;

			m_pDevice->scaleFlipTile(dest, scale, flip, shift);
			break;
		}

		case GfxChunkId::DrawWave:
		case GfxChunkId::FlipDrawWave:
		{
			m_drawTypeInProgress = header.type;

			*m_pDecoder >> m_wave.dest;
			*m_pDecoder >> m_wave.topBorder.length;
			*m_pDecoder >> m_wave.topBorder.thickness;
			*m_pDecoder >> m_wave.topBorder.color;
			*m_pDecoder >> m_wave.topBorder.hold;

			*m_pDecoder >> m_wave.bottomBorder.length;
			*m_pDecoder >> m_wave.bottomBorder.thickness;
			*m_pDecoder >> m_wave.bottomBorder.color;
			*m_pDecoder >> m_wave.bottomBorder.hold;

			*m_pDecoder >> m_wave.frontFill;
			*m_pDecoder >> m_wave.backFill;

			if (header.type == GfxChunkId::FlipDrawWave)
				*m_pDecoder >> m_wave.flip;

			int nTotalSamples = m_wave.topBorder.length + m_wave.bottomBorder.length;

			int bufferSize = sizeof(int) * nTotalSamples;

			m_pTempBuffer = new char[bufferSize];
			m_bytesLoaded = 0;
			m_bufferSize = bufferSize;

			m_wave.topBorder.pWave = (int*) m_pTempBuffer;
			m_wave.bottomBorder.pWave = reinterpret_cast<int*>(m_pTempBuffer) + m_wave.topBorder.length;
			break;
		}
			
		case GfxChunkId::DrawElipse:
		{
			RectSPX	canvas;
			spx 	thickness;
			HiColor	color;
			spx		outlineThickness;
			HiColor	outlineColor;

			*m_pDecoder >> canvas;
			*m_pDecoder >> thickness;
			*m_pDecoder >> color;
			*m_pDecoder >> outlineThickness;
			*m_pDecoder >> outlineColor;

			
			m_pDevice->drawElipse(canvas, thickness, color, outlineThickness, outlineColor);
			break;
		}
				
		case GfxChunkId::DrawPieChart:
		{
			float sliceSizes[32];
			HiColor sliceColors[32];
			
			RectSPX	canvas;
			float 	start;
			int 	nSlices;
			float 	hubSize;
			HiColor hubColor;
			HiColor backColor;
			bool	bRectangular;
			
			*m_pDecoder >> canvas;
			*m_pDecoder >> start;
			*m_pDecoder >> nSlices;
			*m_pDecoder >> hubSize;
			*m_pDecoder >> hubColor;
			*m_pDecoder >> backColor;
			*m_pDecoder >> bRectangular;

			assert(nSlices <= 32);
			
			*m_pDecoder >> GfxStream::DataChunk{ nSlices*4, sliceSizes };
			*m_pDecoder >> GfxStream::DataChunk{ nSlices*8, sliceColors };

			m_pDevice->drawPieChart(canvas, start, nSlices, sliceSizes, sliceColors, hubSize, hubColor, backColor, bRectangular );
			break;
		}

		case GfxChunkId::DrawSegments:
		case GfxChunkId::FlipDrawSegments:
		{
			m_drawTypeInProgress = header.type;
			
			RectI		dest;
			uint16_t	nSegments;
			uint16_t	nEdgeStrips;
			GfxFlip		flip = GfxFlip::Normal;
			TintMode	tintMode;
			
			*m_pDecoder >> dest;
			*m_pDecoder >> nSegments;
			*m_pDecoder >> nEdgeStrips;
			if (header.type == GfxChunkId::FlipDrawSegments)
				*m_pDecoder >> flip;
			*m_pDecoder >> tintMode;

			int nTotalSamples = (nSegments - 1)*nEdgeStrips;

			int nColors;
			switch( tintMode )
			{
				case TintMode::None:
				case TintMode::Flat:
					nColors = nSegments;
					break;
				case TintMode::GradientX:
				case TintMode::GradientY:
					nColors = nSegments*2;
					break;
				case TintMode::GradientXY:
					nColors = nSegments*4;
					break;
			}

			int bufferSize = sizeof(HiColor)*nColors+sizeof(int)*nTotalSamples;
			
			m_pTempBuffer = new char[bufferSize];
			m_bytesLoaded = sizeof(HiColor)*nColors;
			m_bufferSize = bufferSize;

			m_seg.dest = dest;
			m_seg.nSegments = nSegments;
			m_seg.pSegmentColors = (HiColor*) m_pTempBuffer;
			m_seg.nEdgeStrips = nEdgeStrips;
			m_seg.pEdgeStrips = (int*) &m_pTempBuffer[sizeof(HiColor)*nColors];
			m_seg.edgeStripPitch = nSegments-1;
			m_seg.flip = flip;
			m_seg.tintMode = tintMode;
						
			for (int i = 0; i < nColors; i++)
				*m_pDecoder >> m_seg.pSegmentColors[i];

			break;
		}


		case GfxChunkId::EdgeSamples:
		{
			int nBytes = header.size;

			assert(m_pTempBuffer != nullptr && nBytes <= m_bufferSize - m_bytesLoaded);

			// Stream the compressed samples to end of destination and unpack them.

			*m_pDecoder >> GfxStream::DataChunk{ nBytes, &m_pTempBuffer[m_bytesLoaded] };

			// Increase counter and possibly render the segment

			m_bytesLoaded += nBytes;
			if (m_bytesLoaded == m_bufferSize)
			{
				switch (m_drawTypeInProgress)
				{
				case GfxChunkId::DrawSegments:
					m_pDevice->drawSegments(m_seg.dest, m_seg.nSegments, m_seg.pSegmentColors, m_seg.nEdgeStrips, m_seg.pEdgeStrips, m_seg.edgeStripPitch, m_seg.tintMode);
					break;
				case GfxChunkId::FlipDrawSegments:
					m_pDevice->flipDrawSegments(m_seg.dest, m_seg.nSegments, m_seg.pSegmentColors, m_seg.nEdgeStrips, m_seg.pEdgeStrips, m_seg.edgeStripPitch, m_seg.flip, m_seg.tintMode);
					break;
				case GfxChunkId::DrawWave:
					m_pDevice->drawWave(m_wave.dest, &m_wave.topBorder, &m_wave.bottomBorder, m_wave.frontFill, m_wave.backFill);
					break;
				case GfxChunkId::FlipDrawWave:
					m_pDevice->flipDrawWave(m_wave.dest, &m_wave.topBorder, &m_wave.bottomBorder, m_wave.frontFill, m_wave.backFill, m_wave.flip);
					break;
				default:
					assert(false);
				}

				delete[] m_pTempBuffer;
				m_pTempBuffer = nullptr;
				m_drawTypeInProgress = GfxChunkId::OutOfData;
			}
			break;
		}


		case GfxChunkId::BlitNinePatch:
		{
			RectSPX		dstRect;
			BorderSPX 	dstFrame;
			NinePatch 	patch;
			int			scale;
				
			*m_pDecoder >> dstRect;
			*m_pDecoder >> dstFrame;

			*m_pDecoder >> patch.block;
			*m_pDecoder >> patch.frame;

			*m_pDecoder >> patch.rigidPartXOfs;
			*m_pDecoder >> patch.rigidPartXLength;
			*m_pDecoder >> patch.rigidPartXSections;

			*m_pDecoder >> patch.rigidPartYOfs;
			*m_pDecoder >> patch.rigidPartYLength;
			*m_pDecoder >> patch.rigidPartYSections;

			*m_pDecoder >> scale;

			m_pDevice->blitNinePatch(dstRect, dstFrame, patch, scale);
			break;
		}

				
		case GfxChunkId::CreateSurface:
		{
			uint16_t	surfaceId;
			Surface::Blueprint	bp;

			*m_pDecoder >> surfaceId;
			*m_pDecoder >> bp.canvas;
			*m_pDecoder >> bp.dynamic;
			*m_pDecoder >> bp.format;
			*m_pDecoder >> bp.identity;
			*m_pDecoder >> bp.mipmap;
			*m_pDecoder >> bp.sampleMethod;
			*m_pDecoder >> bp.scale;
			*m_pDecoder >> bp.size;
			*m_pDecoder >> bp.tiling;

			bp.buffered = false;
			bp.clut = nullptr;

			if (header.size > 1024)
			{
				bp.clut = (Color8*) Base::memStackAlloc(1024);
				*m_pDecoder >> GfxStream::DataChunk{ 1024, bp.clut };
			}

			if (m_vSurfaces.size() <= surfaceId)
				m_vSurfaces.resize(surfaceId + 16, nullptr);

			m_vSurfaces[surfaceId] = m_pSurfaceFactory->createSurface(bp);

			if (bp.clut)
				Base::memStackRelease(1024);

			break;
		}

		case GfxChunkId::BeginSurfaceUpdate:
		{
			uint16_t	surfaceId;
			RectI		rect;

			*m_pDecoder >> surfaceId;
			*m_pDecoder >> rect;

			m_pUpdatingSurface = m_vSurfaces[surfaceId];
			m_pixelBuffer = m_pUpdatingSurface->allocPixelBuffer(rect);

			m_pWritePixels = m_pixelBuffer.pPixels;
			
			m_surfaceBytesLeft = rect.w * rect.h * m_pUpdatingSurface->pixelBytes();
			break;
		}

		case GfxChunkId::SurfacePixels:
		{
            int line = (m_pWritePixels - m_pixelBuffer.pPixels) / m_pixelBuffer.pitch;
            int ofs = (m_pWritePixels - m_pixelBuffer.pPixels) % m_pixelBuffer.pitch;

            int bytesPerLine = m_pixelBuffer.rect.w * m_pUpdatingSurface->pixelBytes();
            
            int chunkBytesLeft = header.size;
			
			if( chunkBytesLeft > m_surfaceBytesLeft )
				chunkBytesLeft = m_surfaceBytesLeft;		// Last chunk was padded with an extra byte.
			
			m_surfaceBytesLeft -= chunkBytesLeft;
			
            
            while( chunkBytesLeft > 0 )
            {
                int toRead = std::min(chunkBytesLeft, bytesPerLine - ofs);
                *m_pDecoder >> GfxStream::DataChunk{ toRead, m_pWritePixels };
                chunkBytesLeft -= toRead;

                if(toRead + ofs == bytesPerLine)
                {
                    m_pWritePixels += m_pixelBuffer.pitch - ofs;
                    ofs = 0;
                }
                else
                {
                    m_pWritePixels += toRead;
                    ofs += toRead;
                }

            }
			m_pDecoder->align();
			break;
		}

		case GfxChunkId::EndSurfaceUpdate:
		{
			assert(m_surfaceBytesLeft == 0);
			
			m_pUpdatingSurface->pullPixels(m_pixelBuffer);
			m_pUpdatingSurface->freePixelBuffer(m_pixelBuffer);
			m_pUpdatingSurface = nullptr;
			break;
		}
			
		case GfxChunkId::FillSurface:
		{
			uint16_t	surfaceId;
			RectI		region;
			HiColor		col;

			*m_pDecoder >> surfaceId;
			*m_pDecoder >> region;
			*m_pDecoder >> col;

			m_vSurfaces[surfaceId]->fill(region, col);
			break;
		}
/*
		case GfxChunkId::CopySurface:
		{
			uint16_t	destSurfaceId;
			uint16_t	sourceSurfaceId;
			RectI		sourceRect;
			CoordI		dest;

			*m_pDecoder >> destSurfaceId;
			*m_pDecoder >> sourceSurfaceId;
			*m_pDecoder >> sourceRect;
			*m_pDecoder >> dest;

			Surface * pDest	  = m_vSurfaces[destSurfaceId];
			Surface * pSource = m_vSurfaces[sourceSurfaceId];

			pDest->copyFrom(pSource, sourceRect, dest);
			break;
		}
*/
		case GfxChunkId::DeleteSurface:
		{
			uint16_t	surfaceId;

			*m_pDecoder >> surfaceId;

			m_vSurfaces[surfaceId] = nullptr;
			break;
		}

		default:
			// We don't know how to handle this, so let's just skip it

			m_pDecoder->skip(header.size);
			break;
		}

		return true;
	}

	//____ _pushClipListCanvas() ____________________________________________________

	RectI * GfxStreamPlayer::_pushClipListCanvas(int nRects)
	{
		m_clipListInfoStack.push_back(m_clipList);
		RectI * pRects = _setClipList(nRects, true);
		m_clipList.bCanvas = true;
		return pRects;
	}

	//____ _popClipListCanvas() _____________________________________________________

	void GfxStreamPlayer::_popClipListCanvas()
	{
		while( !m_clipList.bCanvas )
			_popClipList();
		
		_popClipList();
	}

	//____ _pushClipList() ____________________________________________________

	RectI * GfxStreamPlayer::_pushClipList(int nRects)
	{
		m_clipListInfoStack.push_back(m_clipList);
		return _setClipList(nRects, true);
	}

	//____ _setClipList() _____________________________________________________

	RectI * GfxStreamPlayer::_setClipList(int nRects, bool bPush)
	{
		assert(nRects <= c_clipListBufferSize);

		// We overwrite previous clipList unless we start a new canvas or it is the
		// cliplist of the canvas.

		if( m_clipList.bCanvas )
			m_clipListInfoStack.push_back(m_clipList);		// Force an extra push so ClipListInfo for our canvas is preserved.

		if( !bPush && !m_clipList.bCanvas )
			m_clipListBuffer.nRects -= m_clipList.nRects;

		// Reserve a new buffer if needed

		if (m_clipListBuffer.capacity < m_clipListBuffer.nRects + nRects)
		{
			m_clipListBufferStack.push_back(m_clipListBuffer);
			
			m_clipListBuffer.pRects = new RectI[c_clipListBufferSize];
			m_clipListBuffer.nRects = 0;
			m_clipListBuffer.capacity = c_clipListBufferSize;
		}

		// Puts values in clipList-buffer

		auto pRects = m_clipListBuffer.pRects + m_clipListBuffer.nRects;

		*m_pDecoder >> GfxStream::DataChunk{ nRects * 16, pRects };

		m_clipListBuffer.nRects += nRects;
		m_clipList.nRects = nRects;
		m_clipList.bCanvas = false;
		return pRects;
	}

	//____ _popClipList() _____________________________________________________

	void GfxStreamPlayer::_popClipList()
	{
		assert(m_clipListBuffer.nRects >= m_clipList.nRects);
		
		if (m_clipListBuffer.nRects == m_clipList.nRects && !m_clipListBufferStack.empty() )
		{
			delete[] m_clipListBuffer.pRects;
			m_clipListBuffer = m_clipListBufferStack.back();
			m_clipListBufferStack.pop_back();
		}
		else
		{
			m_clipListBuffer.nRects -= m_clipList.nRects;
		}

		m_clipList = m_clipListInfoStack.back();
		m_clipListInfoStack.pop_back();
	}


} //namespace wg
