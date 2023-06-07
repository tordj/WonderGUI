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
}

//____ canvas() _____________________________________________________

const CanvasInfo LinearGfxDevice::canvas(CanvasRef ref) const
{
	if( m_canvasDefinitions[int(ref)].ref == ref )
		return m_canvasDefinitions[int(ref)];

	return m_dummyCanvas;
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
		
		uint8_t * pCanvasBuffer = (uint8_t*) m_beginCanvasRenderCallback( ref, bytesNeeded );
		
		int ofs = 0;
		for( int i = 0 ; i < nUpdateRects ; i++ )
		{
			RectI updateRect = pUpdateRects[i]/64;
			
			m_canvasSegments[i].rect = updateRect;
			m_canvasSegments[i].pitch = updateRect.w * m_canvasPixelBytes;
			m_canvasSegments[i].pBuffer = pCanvasBuffer + ofs;
			ofs += updateRect.w * updateRect.h * m_canvasPixelBytes;
		}
		
		m_pClipSegments = m_canvasSegments.data();
		m_nClipSegments = m_canvasSegments.size();
	}
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

} //namespace wg
