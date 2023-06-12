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

#include <wg_lineargfxdevice.h>
#include <wg_gfxutil.h>
#include <wg_gfxbase.h>

using namespace std;

namespace wg
{
	using namespace Util;

	const TypeInfo LinearGfxDevice::TYPEINFO = { "LinearGfxDevice", &SoftGfxDevice::TYPEINFO };

//____ create() _______________________________________________________________

LinearGfxDevice_p LinearGfxDevice::create( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
											std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender )
{
	return LinearGfxDevice_p(new LinearGfxDevice(beginCanvasRender, endCanvasRender));
}

//____ constructor _____________________________________________________________

LinearGfxDevice::LinearGfxDevice( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
								   std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender )
	: m_beginCanvasRenderCallback(beginCanvasRender),
	m_endCanvasRenderCallback(endCanvasRender)
{
}

//____ Destructor ______________________________________________________________

LinearGfxDevice::~LinearGfxDevice()
{

}

//____ typeInfo() _________________________________________________________

const TypeInfo& LinearGfxDevice::typeInfo(void) const
{
	return TYPEINFO;
}

//____ defineCanvas() _________________________________________________________

bool LinearGfxDevice::defineCanvas( CanvasRef ref, const SizeSPX size, PixelFormat pixelFormat, int scale )
{
	auto& def = m_canvasDefinitions[int(ref)];
	def.size = size;
	def.scale = scale;
	def.ref = ref;
	def.pSurface = nullptr;
	def.format = pixelFormat;
	return true;
}

//____ canvas() _____________________________________________________

const CanvasInfo LinearGfxDevice::canvas(CanvasRef ref) const
{
	if( m_canvasDefinitions[int(ref)].ref == ref )
		return m_canvasDefinitions[int(ref)];

	return m_dummyCanvas;
}

//____ setSegmentPadding() ____________________________________________________

void LinearGfxDevice::setSegmentPadding( int bytes )
{
	m_segmentPadding = bytes;
}

//____ beginRender() __________________________________________________________

bool LinearGfxDevice::beginRender()
{
	m_nbSurfaceCanvases = 0;
	return SoftGfxDevice::beginRender();
}

//____ endRender() ____________________________________________________________

bool LinearGfxDevice::endRender()
{
	return SoftGfxDevice::endRender();
}

//____ _beginCanvasUpdate() ___________________________________________________

bool LinearGfxDevice::_beginCanvasUpdate(CanvasRef ref, Surface * pCanvas, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
{
	 // For now we assume that there will only be one CanvasRef in the Canvas begin/end-updateStack and that it will be the first.
	
	SoftGfxDevice::_beginCanvasUpdate(ref, pCanvas, nUpdateRects, pUpdateRects, pLayers, startLayer);

	if( pCanvas )
	{
		m_nbSurfaceCanvases++;
	}
	else
	{
		nUpdateRects = m_nClipRects;
		pUpdateRects = m_pClipRects;
		
		m_canvasSegments.resize(nUpdateRects);

		m_canvasPixelBytes = Util::pixelFormatToDescription(m_canvas.format).bits/8;

		int bytesNeeded = 0;
		for( int i = 0 ; i < nUpdateRects ; i++ )
			bytesNeeded += (pUpdateRects[i].w * pUpdateRects[i].h)/(64*64);

		bytesNeeded *= m_canvasPixelBytes;
		bytesNeeded += nUpdateRects*m_segmentPadding;
		
		uint8_t * pCanvasBuffer = (uint8_t*) m_beginCanvasRenderCallback( ref, bytesNeeded );
		
		int ofs = 0;
		for( int i = 0 ; i < nUpdateRects ; i++ )
		{
			RectI updateRect = pUpdateRects[i]/64;
			
			m_canvasSegments[i].rect = updateRect;
			m_canvasSegments[i].pitch = updateRect.w * m_canvasPixelBytes;
			m_canvasSegments[i].pBuffer = pCanvasBuffer + ofs;
			ofs += updateRect.w * updateRect.h * m_canvasPixelBytes + m_segmentPadding;
		}
		
		m_pClipSegments = m_canvasSegments.data();
		m_nClipSegments = m_canvasSegments.size();
	}
	
	return true;
}

//____ endCanvasUpdate() ______________________________________________________

void LinearGfxDevice::endCanvasUpdate()
{
	if( m_nbSurfaceCanvases > 0 )
	{
		m_nbSurfaceCanvases--;
	}
	else
	{
		m_endCanvasRenderCallback( m_canvas.ref, (int) m_canvasSegments.size(), m_canvasSegments.data() );
	}

	SoftGfxDevice::endCanvasUpdate();
}

//____ _canvasWasChanged() ________________________________________________

void LinearGfxDevice::_canvasWasChanged()
{
	if(m_canvas.pSurface)
	{
		auto pSurface = m_canvas.pSurface;
		
		m_layerSurfaces[0] = pSurface;
		m_pRenderLayerSurface = pSurface;
		
		m_canvasPixelBuffer = pSurface->allocPixelBuffer();
		pSurface->pushPixels(m_canvasPixelBuffer);

		m_pCanvasPixels = m_canvasPixelBuffer.pixels;
		m_canvasPitch = m_canvasPixelBuffer.pitch;

		m_canvasPixelBits = pSurface->pixelDescription()->bits;
		m_canvasPixelFormat = pSurface->pixelFormat();
		
		_updateTintSettings();
		_updateBlitFunctions();
	}
	else if(m_canvas.ref != CanvasRef::None)
	{
		m_layerSurfaces[0] = nullptr;
		m_pCanvasPixels = nullptr;
		m_canvasPitch = 0;

		m_canvasPixelFormat 	= m_canvas.format;
		m_canvasPixelBits 		= Util::pixelFormatToDescription(m_canvas.format).bits;

		_updateTintSettings();
		_updateBlitFunctions();
	}
	else
	{
		// Clean up.

		if (m_pRenderLayerSurface)
		{
			m_pRenderLayerSurface->pullPixels(m_canvasPixelBuffer);
			m_pRenderLayerSurface->freePixelBuffer(m_canvasPixelBuffer);
		}

		m_pRenderLayerSurface = nullptr;

		m_pCanvasPixels = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		m_canvasPixelFormat = PixelFormat::Undefined;

		_updateBlitFunctions();		// Good to have dummies in place when we are not allowed to blit.
	}
}

//____ _clipListWasChanged() __________________________________________________

void LinearGfxDevice::_clipListWasChanged()
{
	// We assume that clipRects comes in same order as canvas updateRects.
	
	if( m_pClipRects == m_pCanvasUpdateRects )
	{
		m_pClipSegments = m_canvasSegments.data();
		m_nClipSegments = m_canvasSegments.size();
	}
	else
	{
		m_clipSegments.resize(m_nClipRects);
		
		const Segment * pCanvasSeg = m_canvasSegments.data();
		int j = 0;

		for( int i = 0 ; i < m_nClipRects ; i++ )
		{
			RectI clipRect = m_pClipRects[i]/64;

			m_clipSegments[i].rect = clipRect;
						
			CoordI pos = clipRect.pos();

			while( !pCanvasSeg->rect.contains(pos) )
				pCanvasSeg++;

			m_clipSegments[i].pBuffer = pCanvasSeg->pBuffer + (pos.y - pCanvasSeg->rect.y) * pCanvasSeg->pitch + (pos.x - pCanvasSeg->rect.x) * m_canvasPixelBytes;

			m_clipSegments[i].pitch = pCanvasSeg->pitch;
		}
		
		m_pClipSegments = m_clipSegments.data();
		m_nClipSegments = m_clipSegments.size();
	}
}

//____ _updateBlitFunctions() _________________________________________________

void LinearGfxDevice::_updateBlitFunctions()
{
	SoftGfxDevice::_updateBlitFunctions();

	if( m_pStraightBlitOp == &SoftGfxDevice::_onePassStraightBlit )
		m_pLinearStraightBlitOp = &LinearGfxDevice::_onePassLinearStraightBlit;
	else if( m_pStraightBlitOp == &SoftGfxDevice::_twoPassStraightBlit )
		m_pLinearStraightBlitOp = &LinearGfxDevice::_twoPassLinearStraightBlit;
	else
		m_pLinearStraightBlitOp = &LinearGfxDevice::_dummyLinearStraightBlit;

	if( m_pStraightTileOp == &SoftGfxDevice::_onePassStraightBlit )
		m_pLinearStraightTileOp = &LinearGfxDevice::_onePassLinearStraightBlit;
	else if( m_pStraightTileOp == &SoftGfxDevice::_twoPassStraightBlit )
		m_pLinearStraightTileOp = &LinearGfxDevice::_twoPassLinearStraightBlit;
	else
		m_pLinearStraightTileOp = &LinearGfxDevice::_dummyLinearStraightBlit;

}


//____ fill() _________________________________________________________________

void LinearGfxDevice::fill(const RectSPX& rect, HiColor col)
{
	if( m_canvas.pSurface )
	{
		SoftGfxDevice::fill(rect,col);
		return;
	}
	
	// Clipping

	if( !m_clipBounds.isOverlapping(rect) )
		return;


	// Skip calls that won't affect destination

	if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract ) )
		return;

	// Optimize calls

	BlendMode blendMode = m_blendMode;
	if (blendMode == BlendMode::Blend && col.a == 4096 && m_bTintOpaque)
	{
		blendMode = BlendMode::Replace;
	}


	// No subpixel precision, make it quick and easy

	RectI pixelRect = roundToPixels(rect);

	int pixelBytes = m_canvasPixelBits / 8;
	FillOp_p pFunc = nullptr;
	
	auto pKernels = m_pKernels[(int)m_canvasPixelFormat];
	if (pKernels)
		pFunc = pKernels->pFillKernels[(int)m_colTrans.mode][(int)blendMode];

	if (pFunc == nullptr)
	{
		if( m_blendMode == BlendMode::Ignore )
			return;
		
		char errorMsg[1024];
		
		sprintf(errorMsg, "Failed fill operation. SoftGfxDevice is missing fill kernel for TintMode::%s, BlendMode::%s onto surface of PixelFormat:%s.",
			toString(m_colTrans.mode),
			toString(m_blendMode),
			toString(m_canvasPixelFormat));
		
		GfxBase::throwError(ErrorLevel::SilentError, ErrorCode::RenderFailure, errorMsg, this, &TYPEINFO, __func__, __FILE__, __LINE__);
		return;
	}

	for (int i = 0; i < m_nClipSegments; i++)
	{
		Segment& seg = m_pClipSegments[i];
		
		RectI patch =  RectI::overlap(seg.rect, pixelRect);
		if (patch.w == 0 || patch.h == 0)
			continue;

		uint8_t * pDst = seg.pBuffer + (patch.y - seg.rect.y)*seg.pitch + (patch.x - seg.rect.x)*m_canvasPixelBytes;
		
		pFunc(pDst, m_canvasPixelBytes, seg.pitch - patch.w*m_canvasPixelBytes, patch.h, patch.w, col, m_colTrans, patch.pos());
	}
}



void LinearGfxDevice::plotPixels( int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
{
}

void LinearGfxDevice::drawLine( CoordSPX begin, CoordSPX end, HiColor color, spx thickness )
{
}

void LinearGfxDevice::drawLine( CoordSPX begin, Direction dir, spx length, HiColor color, spx thickness )
{
}

void LinearGfxDevice::stretchBlit(const RectSPX& dest)
{
}

void LinearGfxDevice::stretchBlit(const RectSPX& dest, const RectSPX& src)
{
}

void LinearGfxDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
{
}

void LinearGfxDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& src, GfxFlip flip)
{
}

void LinearGfxDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
{
}

void LinearGfxDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
{
}

void LinearGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
{
}

void LinearGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
{
}

void LinearGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
{
}

void LinearGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
{
}

void LinearGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
{
}

void LinearGfxDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
{
}

void LinearGfxDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
{
}

void LinearGfxDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor)
{
}

void LinearGfxDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
{
}

void LinearGfxDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
{
}

void LinearGfxDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
{
}

void LinearGfxDevice::drawWaveform(CoordSPX dest, Waveform * pWaveform )
{
}

void LinearGfxDevice::flipDrawWaveform(CoordSPX dest, Waveform * pWaveform, GfxFlip flip)
{
}

void LinearGfxDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
{
}

void LinearGfxDevice::_transformBlitSimple(const RectSPX& _dest, CoordSPX _src, const int simpleTransform[2][2])
{

	if( m_canvas.pSurface )
	{
		SoftGfxDevice::_transformBlitSimple(_dest,_src,simpleTransform);
		return;
	}
	
	// For this method, source and dest should be pixel aligned.

	// Clip and render the patches

	if (!_dest.isOverlapping(m_clipBounds))
		return;


	//TODO: Proper 26:6 support
	RectI dest = roundToPixels(_dest);

	const RectI& clip = dest;

	// Step forward _src by half a pixel, so we start from correct pixel.

	_src.x = _src.x + (simpleTransform[0][0] + simpleTransform[1][0])*32;
	_src.y = _src.y + (simpleTransform[0][1] + simpleTransform[1][1])*32;

	//

	for (int i = 0; i < m_nClipSegments; i++)
	{
		RectI  patch = m_pClipSegments[i].rect;

		CoordI src = _src/64;

		CoordI	patchOfs = patch.pos() - dest.pos();


		if ((clip.x > patch.x) || (clip.x + clip.w < patch.x + patch.w) ||
			(clip.y > patch.y) || (clip.y + clip.h < patch.y + patch.h))
		{

			if ((clip.x > patch.x + patch.w) || (clip.x + clip.w < patch.x) ||
				(clip.y > patch.y + patch.h) || (clip.y + clip.h < patch.y))
				continue;																					// Totally outside clip-rect.

			if (patch.x < clip.x)
			{
				int xDiff = clip.x - patch.x;
				patch.w -= xDiff;
				patch.x = clip.x;
				patchOfs.x += xDiff;
			}

			if (patch.y < clip.y)
			{
				int yDiff = clip.y - patch.y;
				patch.h -= yDiff;
				patch.y = clip.y;
				patchOfs.y += yDiff;
			}

			if (patch.x + patch.w > clip.x + clip.w)
				patch.w = (clip.x + clip.w) - patch.x;

			if (patch.y + patch.h > clip.y + clip.h)
				patch.h = (clip.y + clip.h) - patch.y;
		}

		//

		src.x += patchOfs.x * simpleTransform[0][0] + patchOfs.y * simpleTransform[1][0];
		src.y += patchOfs.x * simpleTransform[0][1] + patchOfs.y * simpleTransform[1][1];

		uint8_t * pDst = m_pClipSegments[i].pBuffer + (patch.y-m_pClipSegments[i].rect.y) * m_pClipSegments[i].pitch + (patch.x - m_pClipSegments[i].rect.x) * m_canvasPixelBytes;
		
		
		if( m_bTileSource )
			(this->*m_pLinearStraightTileOp)(pDst, m_pClipSegments[i].pitch, patch.w, patch.h, src, simpleTransform, patch.pos(), m_pStraightTileFirstPassOp);
		else
			(this->*m_pLinearStraightBlitOp)(pDst, m_pClipSegments[i].pitch, patch.w, patch.h, src, simpleTransform, patch.pos(), m_pStraightBlitFirstPassOp);
	}
}

void LinearGfxDevice::_transformBlitComplex(const RectSPX& dest, BinalCoord src, const binalInt complexTransform[2][2])
{
	
}


//____ _onePassLinearStraightBlit() _____________________________________________

void LinearGfxDevice::_onePassLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI src, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
{
	const SoftSurface * pSource = m_pBlitSource;

	int srcPixelBytes = pSource->m_pPixelDescription->bits / 8;
	int dstPixelBytes = m_canvasPixelBytes;

	Pitches pitches;

	pitches.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
	pitches.dstX = dstPixelBytes;
	pitches.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitches.srcX*width;
	pitches.dstY = destPitch - width * dstPixelBytes;

	uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + src.x * srcPixelBytes;

	pPassOneOp(pSrc, pDst, pSource, pitches, height, width, m_colTrans, patchPos, simpleTransform);
}

//____ _twoPassLinearStraightBlit() _____________________________________________

void LinearGfxDevice::_twoPassLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI src, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
{
	SoftSurface * pSource = m_pBlitSource;

	int srcPixelBytes = pSource->m_pPixelDescription->bits / 8;
	int dstPixelBytes = m_canvasPixelBits / 8;

	Pitches pitchesPass1, pitchesPass2;

	pitchesPass1.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
	pitchesPass1.dstX = 8;
	pitchesPass1.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitchesPass1.srcX*width;
	pitchesPass1.dstY = 0;

	pitchesPass2.srcX = 8;
	pitchesPass2.dstX = dstPixelBytes;
	pitchesPass2.srcY = 0;
	pitchesPass2.dstY = destPitch - width * dstPixelBytes;

	int chunkLines;

	if (width>= 2048)
		chunkLines = 1;
	else if (width*height <= 2048)
		chunkLines = height;
	else
		chunkLines = 2048 / width;

	int memBufferSize = chunkLines * width*8;

	uint8_t * pChunkBuffer = (uint8_t*) GfxBase::memStackAlloc(memBufferSize);

	int line = 0;

	while (line < height)
	{
		int thisChunkLines = min(height - line, chunkLines);

		uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + line*(srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1]) + src.x * srcPixelBytes;
//			uint8_t * pSrc = pSource->m_pData + (src.y+line) * pSource->m_pitch + src.x * srcPixelBytes;

		pPassOneOp(pSrc, pChunkBuffer, pSource, pitchesPass1, thisChunkLines, width, m_colTrans, { 0,0 }, simpleTransform);
		m_pBlitSecondPassOp(pChunkBuffer, pDst, pSource, pitchesPass2, thisChunkLines, width, m_colTrans, patchPos, nullptr);

		patchPos.y += thisChunkLines;
		line += thisChunkLines;
		
		pDst += width*dstPixelBytes;
	}

	GfxBase::memStackFree(memBufferSize);
}

//____ _dummyLinearStraightBlit() _____________________________________________

void LinearGfxDevice::_dummyLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI pos, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
{
}




} //namespace wg
