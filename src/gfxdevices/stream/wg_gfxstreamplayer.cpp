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

	GfxStreamPlayer_p GfxStreamPlayer::create(CGfxInStream& in, GfxDevice * pDevice, SurfaceFactory * pFactory)
	{
		return new GfxStreamPlayer(in, pDevice, pFactory);
	}

	//____ constructor _____________________________________________________________

	GfxStreamPlayer::GfxStreamPlayer(CGfxInStream& in, GfxDevice * pDevice, SurfaceFactory * pFactory)
	{
		m_pStream = in.ptr();
		m_pDevice = pDevice;
		m_pSurfaceFactory = pFactory;
	}

	//____ Destructor _________________________________________________________

	GfxStreamPlayer::~GfxStreamPlayer()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamPlayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isEmpty() __________________________________________________________

	bool GfxStreamPlayer::isEmpty() const
	{
		return m_pStream->isEmpty();
	}

	//____ peekChunk() ________________________________________________________

	GfxStream::Header GfxStreamPlayer::peekChunk() const
	{
		return m_pStream->peek();
	}

	//____ playAll() ___________________________________________________________

	void GfxStreamPlayer::playAll()
	{
		while (playChunk() == true);
	}

	//____ playChunk() ____________________________________________________________

	bool GfxStreamPlayer::playChunk()
	{
		GfxStream::Header header;

		*m_pStream >> header;

		switch (header.type)
		{

		case GfxChunkId::OutOfData:
			return false;

        case GfxChunkId::BeginRender:
            m_pDevice->beginRender();
            break;

        case GfxChunkId::EndRender:
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
				
 			*m_pStream >> canvasRef;
			*m_pStream >> surfaceId;
			*m_pStream >> nUpdateRects;

			RectI * pRects = (RectI*)Base::memStackAlloc(nUpdateRects*16);
			
			*m_pStream >> GfxStream::DataChunk{ nUpdateRects*16, pRects };

			if( surfaceId > 0 )
				m_pDevice->beginCanvasUpdate(m_vSurfaces[surfaceId], nUpdateRects, pRects );
			else
				m_pDevice->beginCanvasUpdate(canvasRef, nUpdateRects, pRects );

			Base::memStackRelease(nUpdateRects*16);
			break;
		}
				
		case GfxChunkId::EndCanvasUpdate:
			m_pDevice->endCanvasUpdate();
			break;

		case GfxChunkId::SetClipList:
		{
			int nRects = header.size / 16;
			RectI * pRects = (RectI*) Base::memStackAlloc(header.size);
			
			*m_pStream >> GfxStream::DataChunk{ header.size, pRects };

			m_pDevice->setClipList(nRects, pRects);
			Base::memStackRelease(header.size);
			break;
		}

		case GfxChunkId::ClearClipList:
			m_pDevice->clearClipList();
			break;

		case GfxChunkId::PushClipList:
		{
			int nRects = header.size / 16;
			RectI * pRects = (RectI*) Base::memStackAlloc(header.size);
			
			*m_pStream >> GfxStream::DataChunk{ header.size, pRects };

			m_pDevice->pushClipList(nRects, pRects);
			Base::memStackRelease(header.size);
			break;
		}
			
		case GfxChunkId::PopClipList:
			m_pDevice->popClipList();
			break;

		case GfxChunkId::SetTintColor:
		{
			HiColor	col;
			*m_pStream >> col;
			m_pDevice->setTintColor(col);
			break;
		}

		case GfxChunkId::SetTintGradient:
		{
			RectI rect;
			Gradient gradient;
			
			*m_pStream >> rect;
			*m_pStream >> gradient.topLeft;
			*m_pStream >> gradient.topRight;
			*m_pStream >> gradient.bottomRight;
			*m_pStream >> gradient.bottomLeft;
			
			m_pDevice->setTintGradient(rect, gradient );
		   break;
		}

		case GfxChunkId::ClearTintGradient:
			m_pDevice->clearTintGradient();
			break;

		case GfxChunkId::SetBlendMode:
		{
			BlendMode	blendMode;
			*m_pStream >> blendMode;
			m_pDevice->setBlendMode(blendMode);
			break;
		}

		case GfxChunkId::SetBlitSource:
		{
			uint16_t	surfaceId;
			*m_pStream >> surfaceId;

			m_pDevice->setBlitSource(m_vSurfaces[surfaceId]);
			break;
		}

		case GfxChunkId::SetMorphFactor:
		{
			float	factor;

			*m_pStream >> factor;
			
			m_pDevice->setRenderLayer(factor);
			break;
		}

		case GfxChunkId::SetRenderLayer:
		{
			uint16_t	layer;

			*m_pStream >> layer;
			
			m_pDevice->setRenderLayer(layer);
			break;
		}

		case GfxChunkId::Fill:
		{
			HiColor	col;

			*m_pStream >> col;

			m_pDevice->fill(col);
			break;
		}
			
		case GfxChunkId::FillRectI:
		{
			RectI	rect;
			HiColor	col;

			*m_pStream >> rect;
			*m_pStream >> col;

			m_pDevice->fill(rect, col);
			break;
		}

		case GfxChunkId::FillRectF:
		{
			RectF	rect;
			HiColor	col;

			*m_pStream >> rect;
			*m_pStream >> col;

			m_pDevice->fill(rect, col);
			break;
		}

		case GfxChunkId::PlotPixels:
		{
			int nPixels = header.size / 16;

			int bufferSize = header.size;
			char * pBuffer = reinterpret_cast<char*>(Base::memStackAlloc(bufferSize));

			// Load all data to buffer

			*m_pStream >> GfxStream::DataChunk{ header.size, pBuffer };

			//

			m_pDevice->plotPixels(nPixels, (CoordI*)pBuffer, (HiColor*)(pBuffer + header.size / 2));

			Base::memStackRelease(bufferSize);
			break;
		}

		case GfxChunkId::DrawLineFromTo:
		{
			CoordI	begin;
			CoordI	end;
			HiColor	color;
			float	thickness;

			*m_pStream >> begin;
			*m_pStream >> end;
			*m_pStream >> color;
			*m_pStream >> thickness;

			m_pDevice->drawLine(begin, end, color, thickness);
			break;
		}

		case GfxChunkId::DrawLineStraight:
		{
			CoordI		begin;
			Direction	dir;
			uint16_t	length;
			HiColor		color;
			float		thickness;

			*m_pStream >> begin;
			*m_pStream >> dir;
			*m_pStream >> length;
			*m_pStream >> color;
			*m_pStream >> thickness;

			m_pDevice->drawLine(begin, dir, length, color, thickness);
			break;
		}

		case GfxChunkId::Blit:
		{
			CoordI		dest;

			*m_pStream >> dest;

			m_pDevice->blit(dest);
			break;
		}

		case GfxChunkId::BlitRectI:
		{
			CoordI		dest;
			RectI		source;

			*m_pStream >> dest;
			*m_pStream >> source;

			m_pDevice->blit(dest,source);
			break;
		}

		case GfxChunkId::FlipBlit:
		{
			CoordI		dest;
			GfxFlip		flip;

			*m_pStream >> dest;
			*m_pStream >> flip;

			m_pDevice->flipBlit(dest,flip);
			break;
		}

		case GfxChunkId::FlipBlitRectI:
		{
			CoordI		dest;
			RectI		source;
			GfxFlip		flip;

			*m_pStream >> dest;
			*m_pStream >> source;
			*m_pStream >> flip;

			m_pDevice->flipBlit(dest,source,flip);
			break;
		}

		case GfxChunkId::StretchBlit:
		{
			RectI		dest;

			*m_pStream >> dest;

			m_pDevice->stretchBlit(dest);
			break;
		}

		case GfxChunkId::StretchBlitRectI:
		{
			RectI		dest;
			RectI		source;

			*m_pStream >> dest;
			*m_pStream >> source;

			m_pDevice->stretchBlit(dest, source);
			break;
		}
			
		case GfxChunkId::StretchBlitRectF:
		{
			RectI		dest;
			RectF		source;

			*m_pStream >> dest;
			*m_pStream >> source;

			m_pDevice->stretchBlit(dest, source);
			break;
		}

		case GfxChunkId::StretchFlipBlit:
		{
			RectI		dest;
			GfxFlip		flip;

			*m_pStream >> dest;
			*m_pStream >> flip;

			m_pDevice->stretchFlipBlit(dest, flip);
			break;
		}

		case GfxChunkId::StretchFlipBlitRectI:
		{
			RectI		dest;
			RectI		source;
			GfxFlip		flip;

			*m_pStream >> dest;
			*m_pStream >> source;
			*m_pStream >> flip;

			m_pDevice->stretchFlipBlit(dest, source, flip);
			break;
		}
			
		case GfxChunkId::StretchFlipBlitRectF:
		{
			RectI		dest;
			RectF		source;
			GfxFlip		flip;

			*m_pStream >> dest;
			*m_pStream >> source;
			*m_pStream >> flip;

			m_pDevice->stretchFlipBlit(dest, source, flip);
			break;
		}

		case GfxChunkId::RotScaleBlit:
		{
			RectI		dest;
			float		rotationDegrees;
			float		scale;
			CoordF		srcCenter;
			CoordF		destCenter;
			
			*m_pStream >> dest;
			*m_pStream >> rotationDegrees;
			*m_pStream >> scale;
			*m_pStream >> srcCenter;
			*m_pStream >> destCenter;

			m_pDevice->rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter );
			break;
		}

		case GfxChunkId::Tile:
		{
			RectI		dest;
			CoordI		shift;
			
			*m_pStream >> dest;
			*m_pStream >> shift;

			m_pDevice->tile(dest, shift);
			break;
		}

		case GfxChunkId::FlipTile:
		{
			RectI		dest;
			GfxFlip		flip;
			CoordI		shift;
			
			*m_pStream >> dest;
			*m_pStream >> flip;
			*m_pStream >> shift;

			m_pDevice->flipTile(dest, flip, shift);
			break;
		}
			
		case GfxChunkId::ScaleTile:
		{
			RectI		dest;
			float		scale;
			CoordI		shift;
			
			*m_pStream >> dest;
			*m_pStream >> scale;
			*m_pStream >> shift;

			m_pDevice->scaleTile(dest, scale, shift);
			break;
		}

		case GfxChunkId::ScaleFlipTile:
		{
			RectI		dest;
			float		scale;
			GfxFlip		flip;
			CoordI		shift;
			
			*m_pStream >> dest;
			*m_pStream >> scale;
			*m_pStream >> flip;
			*m_pStream >> shift;

			m_pDevice->scaleFlipTile(dest, scale, flip, shift);
			break;
		}

		case GfxChunkId::DrawWave:
			//TODO: Implement!!!
			break;
			
		case GfxChunkId::FlipDrawWave:
			//TODO: Implement!!!
			break;
			
		case GfxChunkId::DrawElipse:
		{
			RectF 	canvas;
			float 	thickness;
			HiColor	color;
			float	outlineThickness;
			HiColor	outlineColor;

			*m_pStream >> canvas;
			*m_pStream >> thickness;
			*m_pStream >> color;
			*m_pStream >> outlineThickness;
			*m_pStream >> outlineColor;

			
			m_pDevice->drawElipse(canvas, thickness, color, outlineThickness, outlineColor);
			break;
		}
				
		case GfxChunkId::DrawPieChart:
		{
			float sliceSizes[32];
			HiColor sliceColors[32];
			
			RectI 	canvas;
			float 	start;
			int 	nSlices;
			float 	hubSize;
			HiColor hubColor;
			HiColor backColor;
			bool	bRectangular;
			
			*m_pStream >> canvas;
			*m_pStream >> start;
			*m_pStream >> nSlices;
			*m_pStream >> hubSize;
			*m_pStream >> hubColor;
			*m_pStream >> backColor;
			*m_pStream >> bRectangular;

			assert(nSlices <= 32);
			
			*m_pStream >> GfxStream::DataChunk{ nSlices*4, sliceSizes };
			*m_pStream >> GfxStream::DataChunk{ nSlices*8, sliceColors };

			m_pDevice->drawPieChart(canvas, start, nSlices, sliceSizes, sliceColors, hubSize, hubColor, backColor, bRectangular );
			break;
		}

		case GfxChunkId::DrawSegments:
		{
			m_drawTypeInProgress = GfxChunkId::DrawSegments;
			
			RectI		dest;
			uint16_t	nSegments;
			uint16_t	nEdgeStrips;
			TintMode	tintMode;
			
			*m_pStream >> dest;
			*m_pStream >> nSegments;
			*m_pStream >> nEdgeStrips;
			*m_pStream >> tintMode;

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
			m_seg.edgeStripPitch = nEdgeStrips;
						
			for (int i = 0; i < nSegments; i++)
				* m_pStream >> m_seg.pSegmentColors[i];

			break;
		}

		case GfxChunkId::FlipDrawSegments:
			//TODO: Implement!!!
			break;

		case GfxChunkId::BlitNinePatch:
		{
			RectI 		dstRect;
			BorderI 	dstFrame;
			NinePatch 	patch;
				
			*m_pStream >> dstRect;
			*m_pStream >> dstFrame;

			*m_pStream >> patch.block;
			*m_pStream >> patch.frame;

			*m_pStream >> patch.rigidPartXOfs;
			*m_pStream >> patch.rigidPartXLength;
			*m_pStream >> patch.rigidPartXSections;

			*m_pStream >> patch.rigidPartYOfs;
			*m_pStream >> patch.rigidPartYLength;
			*m_pStream >> patch.rigidPartYSections;

			m_pDevice->blitNinePatch(dstRect, dstFrame, patch);
			break;
		}
		case GfxChunkId::EdgeSamples:
		{
			int nBytes = header.size;

			assert(m_pTempBuffer != nullptr && nBytes <= m_bufferSize - m_bytesLoaded);

			// Stream the compressed samples to end of destination and unpack them.

			*m_pStream >> GfxStream::DataChunk{ nBytes, &m_pTempBuffer[m_bytesLoaded] };
			
			// Increase counter and possibly render the segment

			m_bytesLoaded += nBytes;
			if (m_bytesLoaded == m_bufferSize)
			{
				switch( m_drawTypeInProgress )
				{
					case GfxChunkId::DrawSegments:
						m_pDevice->drawSegments(m_seg.dest, m_seg.nSegments, m_seg.pSegmentColors, m_seg.nEdgeStrips, m_seg.pEdgeStrips, m_seg.edgeStripPitch, m_seg.tintMode);
						break;
					case GfxChunkId::FlipDrawSegments:
						delete [] m_seg.pSegmentColors;
						break;
					case GfxChunkId::DrawWave:
						break;
					case GfxChunkId::FlipDrawWave:
						break;
					default:
						assert(false);
				}
								
				delete [] m_pTempBuffer;
				m_pTempBuffer = nullptr;
				m_drawTypeInProgress = GfxChunkId::OutOfData;
			}
			break;
		}

				
		case GfxChunkId::CreateSurface:
		{
			uint16_t	surfaceId;
			PixelFormat	format;
			SizeI		size;
			uint16_t	flags;

			*m_pStream >> surfaceId;
			*m_pStream >> format;
			*m_pStream >> size;
			*m_pStream >> flags;


			Color8 * pClut = nullptr;

			if (header.size > 4096)
			{
				pClut = (Color8*) Base::memStackAlloc(4096);
				*m_pStream >> GfxStream::DataChunk{ 4096, pClut };
			}

			if (m_vSurfaces.size() <= surfaceId)
				m_vSurfaces.resize(surfaceId + 16, nullptr);

			m_vSurfaces[surfaceId] = m_pSurfaceFactory->createSurface(size, format, flags & ~SurfaceFlag::Buffered, pClut);

			if (pClut)
				Base::memStackRelease(4096);

			break;
		}
			
		case GfxChunkId::SetSurfaceScaleMode:
		{
			uint16_t	surfaceId;
			ScaleMode	scaleMode;

			*m_pStream >> surfaceId;
			*m_pStream >> scaleMode;

			m_vSurfaces[surfaceId]->setScaleMode(scaleMode);
			break;
		}
			
		case GfxChunkId::SetSurfaceTiling:
		{
			uint16_t	surfaceId;
			bool		bTiling;

			*m_pStream >> surfaceId;
			*m_pStream >> bTiling;

			m_vSurfaces[surfaceId]->setTiling(bTiling);
			break;
		}
			
		case GfxChunkId::BeginSurfaceUpdate:
		{
			uint16_t	surfaceId;
			RectI		rect;

			*m_pStream >> surfaceId;
			*m_pStream >> rect;

			m_pUpdatingSurface = m_vSurfaces[surfaceId];
			m_pixelBuffer = m_pUpdatingSurface->allocPixelBuffer(rect);

			m_pWritePixels = m_pixelBuffer.pPixels;
			break;
		}

		case GfxChunkId::SurfacePixels:
		{
            int line = (m_pWritePixels - m_pixelBuffer.pPixels) / m_pixelBuffer.pitch;
            int ofs = (m_pWritePixels - m_pixelBuffer.pPixels) % m_pixelBuffer.pitch;

            int bytesPerLine = m_pixelBuffer.rect.w * m_pUpdatingSurface->pixelBytes();
            
            int bytesLeft = header.size;
            
            while( bytesLeft > 0 )
            {
                int toRead = min(bytesLeft, bytesPerLine - ofs);
                *m_pStream >> GfxStream::DataChunk{ toRead, m_pWritePixels };
                bytesLeft -= toRead;

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
                        
			break;
		}

		case GfxChunkId::EndSurfaceUpdate:
		{
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

			*m_pStream >> surfaceId;
			*m_pStream >> region;
			*m_pStream >> col;

			m_vSurfaces[surfaceId]->fill(col, region);
			break;
		}
/*
		case GfxChunkId::CopySurface:
		{
			uint16_t	destSurfaceId;
			uint16_t	sourceSurfaceId;
			RectI		sourceRect;
			CoordI		dest;

			*m_pStream >> destSurfaceId;
			*m_pStream >> sourceSurfaceId;
			*m_pStream >> sourceRect;
			*m_pStream >> dest;

			Surface * pDest	  = m_vSurfaces[destSurfaceId];
			Surface * pSource = m_vSurfaces[sourceSurfaceId];

			pDest->copyFrom(pSource, sourceRect, dest);
			break;
		}
*/
		case GfxChunkId::DeleteSurface:
		{
			uint16_t	surfaceId;

			*m_pStream >> surfaceId;

			m_vSurfaces[surfaceId] = nullptr;
			break;
		}

		default:
			// We don't know how to handle this, so let's just skip it

			m_pStream->skip(header.size);
			break;
		}

		return true;
	}

	//____ playFrame() _________________________________________________________

	bool GfxStreamPlayer::playFrame()
	{
		GfxStream::Header	header = peekChunk();

		while (header.type != GfxChunkId::OutOfData)
		{
			playChunk();
			if (header.type == GfxChunkId::EndRender)
				return true;
			header = peekChunk();
		}
		return false;
	}


} //namespace wg
