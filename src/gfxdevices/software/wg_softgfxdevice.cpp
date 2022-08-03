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

#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>
#include <wg_util.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <wg_base.h>
#include <wg_context.h>


#include <cassert>

using namespace std;

namespace wg
{
	using namespace Util;

	const TypeInfo SoftGfxDevice::TYPEINFO = { "SoftGfxDevice", &GfxDevice::TYPEINFO };

	SoftGfxDevice::PlotOp_p		SoftGfxDevice::s_plotOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[TintMode_size][BlendMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_lineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::SegmentOp_p	SoftGfxDevice::s_segmentOpTab[2][BlendMode_size][PixelFormat_size];		// bVerticalTint, BlendMode, PixelFormat

	SoftGfxDevice::StraightBlitOp_p		SoftGfxDevice::s_pass2OpTab[TintMode_size][BlendMode_size][PixelFormat_size];
	SoftGfxDevice::StraightBlitOp_p		SoftGfxDevice::s_pass2OpTab_fast8[TintMode_size][BlendMode_size][PixelFormat_size];

	SoftGfxDevice::StraightBlitOp_p		SoftGfxDevice::s_moveTo_internal_OpTab[PixelFormat_size][2];
	SoftGfxDevice::StraightBlitOp_p		SoftGfxDevice::s_moveTo_internal_fast8_OpTab[PixelFormat_size][2];

	SoftGfxDevice::TransformBlitOp_p		SoftGfxDevice::s_transformTo_internal_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::TransformBlitOp_p		SoftGfxDevice::s_transformTo_internal_fast8_OpTab[PixelFormat_size][2][3];

	static_assert(PixelFormat_size == 18,
		"You need to update s_srcFmtToMtxOfsTab, s_dstFmtToMtxOfsTab and number of entries in m_straightBlitKernelMatrix and m_transformBlitKernelMatrix when PixelFormat_size changes!");

	static_assert(BlendMode_size == 11,
		"You need to update s_blendModeToMtxOfsTab and number of entries in m_straightBlitKernelMatrix and m_transformBlitKernelMatrix when BlendMode_size changes!");



	int SoftGfxDevice::s_srcFmtToMtxOfsTab[PixelFormat_size] = { -1, -1, -1, 0, 1, -1, 2, 3, -1, 4, 5, 6, 7, -1, 8, 9, 10, 11 };
	int SoftGfxDevice::s_dstFmtToMtxOfsTab[PixelFormat_size] = { -1, -1, -1, 0, 1, -1, 2, 3, -1, 4, 5, 6, 7, -1, -1, -1, 8, 9 };

	int SoftGfxDevice::s_blendModeToMtxOfsTab[BlendMode_size] = { -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };


	//____ create() _______________________________________________________________

	SoftGfxDevice_p SoftGfxDevice::create()
	{
		return SoftGfxDevice_p(new SoftGfxDevice());
	}

	//____ constructor _____________________________________________________________

	SoftGfxDevice::SoftGfxDevice()
	{
		m_pCanvasPixels = nullptr;
		m_pRenderLayerSurface = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		m_colTrans.morphFactor = 2048;
		_initTables();

	}

	//____ Destructor ______________________________________________________________

	SoftGfxDevice::~SoftGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& SoftGfxDevice::surfaceType( void ) const
	{
		return SoftSurface::TYPEINFO;
	}

	//____ defineCanvas() ____________________________________________________

	bool SoftGfxDevice::defineCanvas( CanvasRef ref, SoftSurface * pSurface )
	{
	
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( pSurface )
				m_definedCanvases.push_back( CanvasInfo( ref, pSurface, pSurface->pixelSize()*64, pSurface->scale() ) );
		}
		else
		{
			if( pSurface )
			{
				it->pSurface = pSurface;
				it->size = pSurface->pixelSize()*64;
				it->scale = pSurface->scale();
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	//____ canvas() _____________________________________________________

	const CanvasInfo SoftGfxDevice::canvas(CanvasRef ref) const
	{
		auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](const CanvasInfo& entry) { return (ref == entry.ref); });

		if (it != m_definedCanvases.end())
			return *it;

		return m_dummyCanvas;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p SoftGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = SoftSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ setTintColor() _____________________________________________________

	void SoftGfxDevice::setTintColor(HiColor color)
	{
		if (color == m_tintColor)
			return;

		GfxDevice::setTintColor(color);
		_updateTintSettings();

/*
		m_colTrans.flatTintColor = color;

		TintMode tintMode;

		if (m_nTintGradients == 0)
		{
			if (color == Color::White)
				tintMode = TintMode::None;
			else
				tintMode = TintMode::Flat;

			bool bOpacityChanged = (color.a != m_tintColor.a) && (color.a == 255 || m_tintColor.a == 255);

			if (bOpacityChanged || tintMode != m_tintModeSingle)
			{
				m_tintModeSingle = tintMode;
				m_tintModeMulti = tintMode;

				_updateBlitFunctions();
			}
		}
*/
	}

	//____ setTintGradient() __________________________________________________

	void SoftGfxDevice::setTintGradient(const RectI& rect, const Gradient& gradient)
	{
		GfxDevice::setTintGradient(rect, gradient);
		m_colTrans.tintRect = Util::align(rect) / 64;
		_updateTintSettings();
	}

	//____ clearTintGradient() _______________________________________________

	void SoftGfxDevice::clearTintGradient()
	{
		GfxDevice::clearTintGradient();
		_updateTintSettings();
	}

	//____ _updateTintSettings() ______________________________________________

	void SoftGfxDevice::_updateTintSettings()
	{
		TintMode tintMode;

		const int16_t* pUnpackTab = Base::activeContext()->gammaCorrection() ? HiColor::unpackSRGBTab : HiColor::unpackLinearTab;

		if (!m_bTintGradient)
		{
			tintMode = (m_tintColor == Color::White) ? TintMode::None : TintMode::Flat;
			m_colTrans.flatTintColor = m_tintColor;
			m_bTintOpaque = (m_tintColor.a == 4096);
		}
		else
		{
			// Update m_gradientData and generate diffMasks for X- and Y-gradients.

			int diffMaskX = 0, diffMaskY = 0;


			uint32_t	flatTintColorR = m_tintColor.r;
			uint32_t	flatTintColorG = m_tintColor.g;
			uint32_t	flatTintColorB = m_tintColor.b;
			uint32_t	flatTintColorA = m_tintColor.a;

			uint32_t	r[4], g[4], b[4], a[4];							// Scale: 0 -> (1 << 18)

			HiColor* pCol = &m_tintGradient.topLeft;
			for (int i = 0; i < 4; i++)
			{
				r[i] = (pCol->r * flatTintColorR) >> 6;
				g[i] = (pCol->g * flatTintColorG) >> 6;
				b[i] = (pCol->b * flatTintColorB) >> 6;
				a[i] = (pCol->a * flatTintColorA) >> 6;
				pCol++;
			}

			m_colTrans.topLeftR = r[0];
			m_colTrans.topLeftG = g[0];
			m_colTrans.topLeftB = b[0];
			m_colTrans.topLeftA = a[0];

			m_colTrans.topRightR = r[1];
			m_colTrans.topRightG = g[1];
			m_colTrans.topRightB = b[1];
			m_colTrans.topRightA = a[1];

			int width = Util::align(m_tintGradientRect.h)/64;

			m_colTrans.leftIncR = int(r[3] - r[0]) / width;
			m_colTrans.leftIncG = int(g[3] - g[0]) / width;
			m_colTrans.leftIncB = int(b[3] - b[0]) / width;
			m_colTrans.leftIncA = int(a[3] - a[0]) / width;

			m_colTrans.rightIncR = int(r[2] - r[1]) / width;
			m_colTrans.rightIncG = int(g[2] - g[1]) / width;
			m_colTrans.rightIncB = int(b[2] - b[1]) / width;
			m_colTrans.rightIncA = int(a[2] - a[1]) / width;

			diffMaskX |= (m_tintGradient.topLeft.argb - m_tintGradient.topRight.argb) | (m_tintGradient.bottomRight.argb - m_tintGradient.bottomLeft.argb);
			diffMaskY |= (m_tintGradient.topLeft.argb - m_tintGradient.bottomLeft.argb) | (m_tintGradient.topRight.argb - m_tintGradient.bottomRight.argb);

			// Calculate tintMode

			if( (diffMaskX | diffMaskY) == 0 )
			{
				if (m_tintGradient.topLeft.isOpaqueWhite() )
					tintMode = TintMode::None;
				else
				{
					m_colTrans.flatTintColor.r = r[0] >> 6;
					m_colTrans.flatTintColor.g = g[0] >> 6;
					m_colTrans.flatTintColor.b = b[0] >> 6;
					m_colTrans.flatTintColor.a = a[0] >> 6;
					tintMode = TintMode::Flat;
				}
			}
			else
			{
				if (diffMaskY == 0)
					tintMode = TintMode::GradientX;
				else if (diffMaskX == 0)
					tintMode = TintMode::GradientY;
				else
					tintMode = TintMode::GradientXY;
			}

			// Update m_bTintOpaque

			int value = int(m_tintColor.a) + int(m_tintGradient.topLeft.a) + int(m_tintGradient.topRight.a) 
						+ int(m_tintGradient.bottomRight.a) + int(m_tintGradient.bottomLeft.a);

			m_bTintOpaque = (value == 4096*5);
		}

		// Update tintMode and blitFunctions.

		if (tintMode != m_colTrans.mode)
		{
			m_colTrans.mode = tintMode;
			_updateBlitFunctions();
		}

	}

	//____ setBlendMode() _____________________________________________________

	bool SoftGfxDevice::setBlendMode(BlendMode blendMode)
	{
		if (blendMode == m_blendMode)
			return true;

		bool retVal = GfxDevice::setBlendMode(blendMode);
		if (retVal)
			_updateBlitFunctions();
		return retVal;
	}

	//____ setMorphFactor() ___________________________________________________

	void SoftGfxDevice::setMorphFactor(float factor)
	{
		limit(factor, 0.f, 1.f);
		m_morphFactor = factor;
		m_colTrans.morphFactor = int(factor * 4096);
	}

	//____ beginRender() _______________________________________________________

	bool SoftGfxDevice::beginRender()
	{
		if( m_bRendering )
			return false;

		setTintColor(Color::White);
		setBlendMode(BlendMode::Blend);

		m_bRendering = true;
		return true;
	}

	//____ endRender() _________________________________________________________

	bool SoftGfxDevice::endRender()
	{
		if( !m_bRendering )
			return false;

		m_bRendering = false;
		return true;
	}

	//____ fill() ______________________________________________________

	void SoftGfxDevice::fill(const RectSPX& rect, HiColor col)
	{
		if (!m_pRenderLayerSurface || !m_pCanvasPixels )
			return;

		// Clipping

		if( !m_clipBounds.intersectsWith(rect) )
			return;

		// Prepare colors


		// Skip calls that won't affect destination

		if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract ) )
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && col.a == 4096 && m_bTintOpaque)
		{
			blendMode = BlendMode::Replace;
		}

		//

		if( ((rect.x | rect.y | rect.w | rect.h) & 0x3F) == 0 )
		{
			// No subpixel precision, make it quick and easy

			RectI pixelRect = roundToPixels(rect);

			int pixelBytes = m_canvasPixelBits / 8;
			FillOp_p pFunc = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
			if (pFunc == nullptr)
			{
				if( m_blendMode == BlendMode::Ignore )
					return;
				
				char errorMsg[1024];
				
				sprintf(errorMsg, "Failed fill operation. SoftGfxDevice is missing fill kernel for TintMode::%s, BlendMode::%s onto surface of PixelFormat:%s.",
					toString(m_colTrans.mode),
					toString(m_blendMode),
					toString(m_pRenderLayerSurface->pixelFormat()) );
				
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
				return;
			}

			for (int i = 0; i < m_nClipRects; i++)
			{
				RectI patch =  RectI::getIntersection(m_pClipRects[i]/64, pixelRect);
				if (patch.w == 0 || patch.h == 0)
					continue;

				uint8_t * pDst = m_pCanvasPixels + patch.y *m_canvasPitch + patch.x * pixelBytes;
				pFunc(pDst, pixelBytes, m_canvasPitch - patch.w*pixelBytes, patch.h, patch.w, col, m_colTrans, patch.pos());
			}
		}
		else
		{
					BlendMode	edgeBlendMode = (m_blendMode == BlendMode::Replace) ? BlendMode::Blend : m_blendMode; // Need to blend edges and corners even if fill is replace

			int pixelBytes = m_canvasPixelBits / 8;
			FillOp_p pFillOp = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
			FillOp_p pEdgeOp = s_fillOpTab[(int)m_colTrans.mode][(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];
	//		PlotOp_p pPlotOp = s_plotOpTab[(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];

			if (pFillOp == nullptr || pEdgeOp == nullptr )
			{
				if( m_blendMode == BlendMode::Ignore )
					return;
				
				char errorMsg[1024];
				
				sprintf(errorMsg, "Failed fill operation. SoftGfxDevice is missing fill kernel for TintMode::%s, BlendMode::%s onto surface of PixelFormat:%s.",
					toString(m_colTrans.mode),
					toString(m_blendMode),
					toString(m_pRenderLayerSurface->pixelFormat()) );
				
				Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
				return;
			}

			
			for (int i = 0; i < m_nClipRects; i++)
			{
				HiColor color = col;

				RectI  patch = RectI::getIntersection(rect, m_pClipRects[i]);
				if (patch.w == 0 || patch.h == 0)
					continue;

				// Fill all but anti-aliased edges

				int x1 = ((patch.x + 63) >> 6);
				int y1 = ((patch.y + 63) >> 6);
				int x2 = ((patch.x + patch.w) >> 6);
				int y2 = ((patch.y + patch.h) >> 6);


				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
				pFillOp(pDst, pixelBytes, m_canvasPitch - (x2 - x1)*pixelBytes, y2 - y1, x2 - x1, color, m_colTrans, { x1,y1 });

				// Draw the sides

				int aaLeft = 4096 - (patch.x & 0x3F) * 64;
				int aaTop = 4096 - (patch.y & 0x3F) * 64;
				int aaRight = ((patch.x + patch.w) & 0x3F) * 64;
				int aaBottom = ((patch.y + patch.h) & 0x3F) * 64;

				int aaTopLeft = aaTop * aaLeft / 4096;
				int aaTopRight = aaTop * aaRight / 4096;
				int aaBottomLeft = aaBottom * aaLeft / 4096;
				int aaBottomRight = aaBottom * aaRight / 4096;


				if (m_blendMode != BlendMode::Replace)
				{
					int alpha = color.a;

					aaLeft = aaLeft * alpha >> 12;
					aaTop = aaTop * alpha >> 12;
					aaRight = aaRight * alpha >> 12;
					aaBottom = aaBottom * alpha >> 12;

					aaTopLeft = aaTopLeft * alpha >> 12;
					aaTopRight = aaTopRight * alpha >> 12;
					aaBottomLeft = aaBottomLeft * alpha >> 12;
					aaBottomRight = aaBottomRight * alpha >> 12;
				}

				RectI pixelPatch = patch / 64;

				if (aaTop != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + x1 * pixelBytes;
					int length = x2 - x1;
					color.a = aaTop;
					pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,pixelPatch.y });
				}

				if (aaBottom != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
					int length = x2 - x1;
					color.a = aaBottom;
					pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,y2 });
				}

				if (aaLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + pixelPatch.x * pixelBytes;
					int length = y2 - y1;
					color.a = aaLeft;
					pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { pixelPatch.x, y1 });
				}

				if (aaRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
					int length = y2 - y1;
					color.a = aaRight;
					pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { x2, y1 });
				}

				// Draw corner pieces


				if (aaTopLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + pixelPatch.x * pixelBytes;
					color.a = aaTopLeft;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { pixelPatch.x, pixelPatch.y } );
	//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, (int)patch.y } );
				}

				if (aaTopRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + x2 * pixelBytes;
					color.a = aaTopRight;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, pixelPatch.y });
	//				pPlotOp(pDst, color, m_colTrans, { x2, (int)patch.y });
				}

				if (aaBottomLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + pixelPatch.x * pixelBytes;
					color.a = aaBottomLeft;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { pixelPatch.x, y2 });
	//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, y2 } );
				}

				if (aaBottomRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
					color.a = aaBottomRight;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, y2 });
	//				pPlotOp(pDst, color, m_colTrans, { x2,y2 });
				}
			}

		}


	}
	
	//____ drawLine() ____ [from/to] __________________________________________

	void SoftGfxDevice::drawLine(CoordSPX beg, CoordSPX end, HiColor color, spx thickness)
	{
		//TODO: Proper 26:6 support
		beg = roundToPixels(beg);
		end = roundToPixels(end);

		if (!m_pRenderLayerSurface || !m_pCanvasPixels)
			return;

		HiColor fillColor = color * m_tintColor;

		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		//

		ClipLineOp_p pOp = s_clipLineOpTab[(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		if (pOp == nullptr )
		{
			if( m_blendMode == BlendMode::Ignore )
				return;
			
			char errorMsg[1024];
			
			sprintf(errorMsg, "Failed drawLine operation. SoftGfxDevice is missing clipLine kernel for BlendMode::%s onto surface of PixelFormat:%s.",
				toString(m_blendMode),
				toString(m_pRenderLayerSurface->pixelFormat()) );
			
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		
		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;
		int		clipStart, clipEnd;

		if (std::abs(beg.x - end.x) > std::abs(beg.y - end.y))
		{
			// Prepare mainly horizontal line segment

			if (beg.x > end.x)
				swap(beg, end);

			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness(thickness/64.f, slope);
			pos = (beg.y << 16) - width / 2 + 32768;

			rowInc = m_canvasPixelBits / 8;
			pixelInc = m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;

			// Loop through patches

			for( int i = 0 ; i < m_nClipRects ; i++ )
			{
				// Do clipping

				const RectI clip = m_pClipRects[i]/64;

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.x < clip.x)
				{
					int cut = clip.x - beg.x;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.x > clip.x + clip.w)
					_length -= end.x - (clip.x + clip.w);

				clipStart = clip.y << 16;
				clipEnd = (clip.y + clip.h) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
		else
		{
			// Prepare mainly vertical line segment

			if (beg.y > end.y)
				swap(beg, end);

			length = end.y - beg.y;
			if (length == 0)
				return;											// TODO: Should stil draw the caps!

			// Need multiplication instead of shift as operand might be negative
			slope = ((end.x - beg.x) * 65536) / length;
			width = _scaleLineThickness(thickness/64.f, slope);
			pos = (beg.x << 16) - width / 2 + 32768;

			rowInc = m_canvasPitch;
			pixelInc = m_canvasPixelBits / 8;

			pRow = m_pCanvasPixels + beg.y * rowInc;

			// Loop through patches

			for (int i = 0; i < m_nClipRects; i++)
			{
				// Do clipping

				const RectI clip = m_pClipRects[i]/64;

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.y < clip.y)
				{
					int cut = clip.y - beg.y;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.y > clip.y + clip.h)
					_length -= end.y - (clip.y + clip.h);

				clipStart = clip.x << 16;
				clipEnd = (clip.x + clip.w) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
	}

	//____ drawLine() ____ [start/direction] ________________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::drawLine(CoordSPX _begin, Direction dir, spx _length, HiColor _col, spx _thickness)
	{

		float thickness = _thickness / 64.f;

		//TODO: Proper 26:6 support
		_begin = roundToPixels(_begin);
		_length = roundToPixels(_length);

		//TODO: Optimize!

		if (thickness <= 0.f)
			return;

		BlendMode edgeBlendMode = m_blendMode;
		if (m_blendMode == BlendMode::Replace)
		{
			_col.a = 255;						// Needed since we still blend the edges.
			edgeBlendMode = BlendMode::Blend;
		}

		_col = _col * m_tintColor;

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pCenterOp = s_fillOpTab[(int)TintMode::None][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		FillOp_p	pEdgeOp = s_fillOpTab[(int)TintMode::None][(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		if (pCenterOp == nullptr || pEdgeOp == nullptr )
		{
			if( m_blendMode == BlendMode::Ignore )
				return;
			
			char errorMsg[1024];
			
			sprintf(errorMsg, "Failed drawLine operation. SoftGfxDevice is missing fill kernel for BlendMode::%s onto surface of PixelFormat:%s.",
				toString(m_blendMode),
				toString(m_pRenderLayerSurface->pixelFormat()) );
			
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		
		
		for (int i = 0; i < m_nClipRects; i++)
		{
			const RectI& clip = m_pClipRects[i]/64;

			CoordI begin = _begin;
			int length = _length;

			switch (dir)
			{
			case Direction::Left:
				begin.x -= length;
			case Direction::Right:
			{
				if (begin.x > clip.x + clip.w)
					continue;

				if (begin.x < clip.x)
				{
					length -= clip.x - begin.x;
					if (length <= 0)
						continue;
					begin.x = clip.x;
				}

				if (begin.x + length > clip.x + clip.w)
				{
					length = clip.x + clip.w - begin.x;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.y < clip.y || begin.y >= clip.y + clip.h)
						continue;

					HiColor col = _col;
					col.a = (int16_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, pixelBytes, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					HiColor edgeColor(_col.r, _col.g, _col.b, (int16_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.y + expanse <= clip.y || begin.y - expanse >= clip.y + clip.h)
						continue;

					int beginY = begin.y - expanse;
					int endY = begin.y + expanse + 1;

					if (beginY < clip.y)
						beginY = clip.y - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
						//					_drawStraightLine({ begin.x, beginY }, Axis::X, length, edgeColor);
					}

					if (endY > clip.y + clip.h)
						endY = clip.y + clip.h + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					int bodyThickness = endY - beginY - 2;
					uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
					pCenterOp(pBegin, pixelBytes, m_canvasPitch - length * pixelBytes, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
			case Direction::Up:
				begin.y -= length;
			case Direction::Down:
				if (begin.y > clip.y + clip.h)
					continue;

				if (begin.y < clip.y)
				{
					length -= clip.y - begin.y;
					if (length <= 0)
						continue;
					begin.y = clip.y;
				}

				if (begin.y + length > clip.y + clip.h)
				{
					length = clip.y + clip.h - begin.y;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.x < clip.x || begin.x >= clip.x + clip.w)
						continue;

					HiColor col = _col;
					col.a = (int16_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					HiColor edgeColor(_col.r, _col.g, _col.b, (int16_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.x + expanse <= clip.x || begin.x - expanse >= clip.x + clip.w)
						continue;

					int beginX = begin.x - expanse;
					int endX = begin.x + expanse + 1;

					if (beginX < clip.x)
						beginX = clip.x - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					if (endX > clip.x + clip.w)
						endX = clip.x + clip.w + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}


					int bodyThickness = endX - beginX - 2;
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
					pCenterOp(pBegin, m_canvasPitch, pixelBytes - length * m_canvasPitch, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
		}
	}

	//____ _transformDrawSegments() _________________________________________

	void SoftGfxDevice::_transformDrawSegments(const RectSPX& _destIn, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * _pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int _simpleTransform[2][2])
	{
		//TODO: Proper 26:6 support
		RectI _dest = roundToPixels(_destIn);

		RectI dest = _dest;

		SegmentEdge edges[c_maxSegments - 1];

		int xPitch = m_canvasPixelBits / 8;
		int yPitch = m_canvasPitch;

		// We need to modify our transform since we are moving the destination pointer, not the source pointer, according to the transform.

		int simpleTransform[2][2];

		simpleTransform[0][0] = _simpleTransform[0][0];
		simpleTransform[1][1] = _simpleTransform[1][1];

		if ((_simpleTransform[0][0] | _simpleTransform[1][1]) == 0)
		{
			simpleTransform[0][1] = _simpleTransform[1][0];
			simpleTransform[1][0] = _simpleTransform[0][1];
		}
		else
		{
			simpleTransform[0][1] = _simpleTransform[0][1];
			simpleTransform[1][0] = _simpleTransform[1][0];
		}

		// Calculate pitches

		int colPitch = simpleTransform[0][0] * xPitch + simpleTransform[0][1] * yPitch;
		int rowPitch = simpleTransform[1][0] * xPitch + simpleTransform[1][1] * yPitch;

		// Calculate start coordinate

		CoordI start = dest.pos();

		if (simpleTransform[0][0] + simpleTransform[1][0] < 0)
			start.x += dest.w - 1;

		if (simpleTransform[0][1] + simpleTransform[1][1] < 0)
			start.y += dest.h - 1;

		// Detect if strip columns are lined horizontally or verically

		bool bHorizontalColumns = (abs(colPitch) == xPitch);

		// Limit size of destination rect by number of edgestrips.

		if (bHorizontalColumns)
		{
			if (dest.w > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.x += dest.w - nEdgeStrips - 1;

				dest.w = nEdgeStrips - 1;
			}
		}
		else
		{
			if (dest.h > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.y += dest.h - nEdgeStrips - 1;

				dest.h = nEdgeStrips - 1;
			}
		}

		// Apply tinting

		int16_t	colors[c_maxSegments*4][4];				// RGBA order of elements
		bool	transparentSegments[c_maxSegments];
		bool	opaqueSegments[c_maxSegments];

		SegmentGradient* pGradientsY = nullptr;
		SegmentGradient* pGradientsX = nullptr;
		int		gradientsYBufferSize = 0;
		int		gradientsXBufferSize = 0;

		// Determine combined tint-mode

		bool	bTintFlat = false;			// Set if we tint in any way, flat, x, y, or xy.
		bool	bTintX = false;
		bool	bTintY = false;

		if (tintMode != TintMode::None)
		{
			bTintFlat = !m_tintColor.isOpaqueWhite();

			if (tintMode == TintMode::GradientXY || m_colTrans.mode == TintMode::GradientXY)
			{
				bTintX = bTintY = true;
			}
			else
			{
				if (tintMode == TintMode::GradientY || m_colTrans.mode == TintMode::GradientY)
					bTintY = true;
				if (tintMode == TintMode::GradientX || m_colTrans.mode == TintMode::GradientX)
					bTintX = true;
			}
		}

		// Unpack input colors and fill in transparentSegments

		if (!bTintX && !bTintY)
		{
			// If we just use flat tinting (or no tint at all), we tint our segment colors right away

			for (int i = 0; i < nSegments; i++)
			{
				colors[i][0] = (pSegmentColors[i].r * m_colTrans.flatTintColor.r) >> 12;
				colors[i][1] = (pSegmentColors[i].g * m_colTrans.flatTintColor.g) >> 12;
				colors[i][2] = (pSegmentColors[i].b * m_colTrans.flatTintColor.b) >> 12;
				colors[i][3] = (pSegmentColors[i].a * m_colTrans.flatTintColor.a) >> 12;

				transparentSegments[i] = (colors[i][3] == 0);
				opaqueSegments[i] = (colors[i][3] == 4096);
			}
		}
		else
		{
			int colorsPerSegment = bTintX && bTintY ? 4 : 2;

			for (int i = 0; i < nSegments*colorsPerSegment; i++)
			{
				colors[i][0] = pSegmentColors[i].r;
				colors[i][1] = pSegmentColors[i].g;
				colors[i][2] = pSegmentColors[i].b;
				colors[i][3] = pSegmentColors[i].a;
			}
		}


		// If we instead have gradients we have things to take care of...

		if (bTintX || bTintY)
		{
			// Generate the buffers that we will need

			if (bTintY)
			{
				gradientsYBufferSize = sizeof(SegmentGradient) * nSegments;
				pGradientsY = (SegmentGradient*)Base::memStackAlloc(gradientsYBufferSize);
			}

			if (bTintX)
			{
				// We use two gradients per segment in X, one for top and bottom of rectangle each.

				gradientsXBufferSize = sizeof(SegmentGradient) * nSegments * 2;
				pGradientsX = (SegmentGradient*)Base::memStackAlloc(gradientsXBufferSize);
			}


			// Calculate RGBA values from m_colTrans for our four corners.

			int		baseB[4], baseG[4], baseR[4], baseA[4];
			int		tempB[4], tempG[4], tempR[4], tempA[4];

			_colTransRect(tempB, tempG, tempR, tempA, _dest);

			// Rotate the colors of our four corners if we are flipped

			static const int cornerSwitchMap[2][2][2][4] = 
							{ {{{0,3,2,1},				// [ 0,-1,-1, 0] = Rot90FlipY
								{1,2,3,0}},				// [ 0,-1, 1, 0] = Rot90
								{{3,0,1,2},				// [ 0, 1,-1, 0] = Rot270
								{2,1,0,3}}},			// [ 0, 1, 1, 0] = Rot90FlipX
								{{{2,3,0,1},			// [-1, 0, 0,-1] = Rot180
								{1,0,3,2}},				// [-1, 0, 0, 1] = FlipX
								{{3,2,1,0},				// [ 1, 0, 0,-1] = FlipY
								{0,1,2,3}}} };			// [ 1, 0, 0, 1] = Normal

			int i1, i2, i3;
			if (_simpleTransform[0][0] == 0)
			{
				i1 = 0;
				i2 = _simpleTransform[0][1] == 1 ? 1 : 0;
				i3 = _simpleTransform[1][0] == 1 ? 1 : 0;
			}
			else
			{
				i1 = 1;
				i2 = _simpleTransform[0][0] == 1 ? 1 : 0;
				i3 = _simpleTransform[1][1] == 1 ? 1 : 0;
			}

			const int* pSwitch = cornerSwitchMap[i1][i2][i3];

			for (int i = 0; i < 4; i++)
			{
				int n = pSwitch[i];
				baseB[i] = tempB[n];
				baseG[i] = tempG[n];
				baseR[i] = tempR[n];
				baseA[i] = tempA[n];
			}

			// Lets process each segment

			for (int seg = 0; seg < nSegments; seg++)
			{

				int		segB[4], segG[4], segR[4], segA[4];

				// Calculate RGBA values for our four corners

				if (tintMode == TintMode::Flat)
				{
					for (int i = 0; i < 4; i++)
					{
						segR[i] = baseR[i] * colors[seg][0];
						segG[i] = baseG[i] * colors[seg][1];
						segB[i] = baseB[i] * colors[seg][2];
						segA[i] = baseA[i] * colors[seg][3];
					}
				}
				else
				{
					if (tintMode == TintMode::GradientX)
					{
						segR[0] = baseR[0] * colors[seg*2][0];
						segG[0] = baseG[0] * colors[seg * 2][1];
						segB[0] = baseB[0] * colors[seg * 2][2];
						segA[0] = baseA[0] * colors[seg * 2][3];

						segR[1] = baseR[1] * colors[seg * 2+1][0];
						segG[1] = baseG[1] * colors[seg * 2+1][1];
						segB[1] = baseB[1] * colors[seg * 2+1][2];
						segA[1] = baseA[1] * colors[seg * 2+1][3];

						segR[2] = baseR[2] * colors[seg * 2 + 1][0];
						segG[2] = baseG[2] * colors[seg * 2 + 1][1];
						segB[2] = baseB[2] * colors[seg * 2 + 1][2];
						segA[2] = baseA[2] * colors[seg * 2 + 1][3];

						segR[3] = baseR[3] * colors[seg * 2][0];
						segG[3] = baseG[3] * colors[seg * 2][1];
						segB[3] = baseB[3] * colors[seg * 2][2];
						segA[3] = baseA[3] * colors[seg * 2][3];
					}
					else if (tintMode == TintMode::GradientY)
					{
						segR[0] = baseR[0] * colors[seg * 2][0];
						segG[0] = baseG[0] * colors[seg * 2][1];
						segB[0] = baseB[0] * colors[seg * 2][2];
						segA[0] = baseA[0] * colors[seg * 2][3];

						segR[1] = baseR[1] * colors[seg * 2][0];
						segG[1] = baseG[1] * colors[seg * 2][1];
						segB[1] = baseB[1] * colors[seg * 2][2];
						segA[1] = baseA[1] * colors[seg * 2][3];

						segR[2] = baseR[2] * colors[seg * 2 + 1][0];
						segG[2] = baseG[2] * colors[seg * 2 + 1][1];
						segB[2] = baseB[2] * colors[seg * 2 + 1][2];
						segA[2] = baseA[2] * colors[seg * 2 + 1][3];

						segR[3] = baseR[3] * colors[seg * 2 + 1][0];
						segG[3] = baseG[3] * colors[seg * 2 + 1][1];
						segB[3] = baseB[3] * colors[seg * 2 + 1][2];
						segA[3] = baseA[3] * colors[seg * 2 + 1][3];
					}
					else
					{
						segR[0] = baseR[0] * colors[seg * 4][0];
						segG[0] = baseG[0] * colors[seg * 4][1];
						segB[0] = baseB[0] * colors[seg * 4][2];
						segA[0] = baseA[0] * colors[seg * 4][3];

						segR[1] = baseR[1] * colors[seg * 4 + 1][0];
						segG[1] = baseG[1] * colors[seg * 4 + 1][1];
						segB[1] = baseB[1] * colors[seg * 4 + 1][2];
						segA[1] = baseA[1] * colors[seg * 4 + 1][3];

						segR[2] = baseR[2] * colors[seg * 4 + 2][0];
						segG[2] = baseG[2] * colors[seg * 4 + 2][1];
						segB[2] = baseB[2] * colors[seg * 4 + 2][2];
						segA[2] = baseA[2] * colors[seg * 4 + 2][3];

						segR[3] = baseR[3] * colors[seg * 4 + 3][0];
						segG[3] = baseG[3] * colors[seg * 4 + 3][1];
						segB[3] = baseB[3] * colors[seg * 4 + 3][2];
						segA[3] = baseA[3] * colors[seg * 4 + 3][3];
					}
				}

				// We now have the segments corner colors. Let's save
				// that information in the correct format for future
				// processing depending on tint mode.

				// Filling in the x-gradient if we have one. Two SegmentGradient structs for
				// each segment, one for gradient along top and of dest rectangle and one along the bottom.

				if (bTintX)
				{
					// Fill in top gradient

					auto p = &pGradientsX[seg * 2];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[1] - segB[0]) / dest.w;
					p->incG = (segG[1] - segG[0]) / dest.w;
					p->incR = (segR[1] - segR[0]) / dest.w;
					p->incA = (segA[1] - segA[0]) / dest.w;

					// Fill in bottom gradient

					p++;

					p->begB = segB[3];
					p->begG = segG[3];
					p->begR = segR[3];
					p->begA = segA[3];

					p->incB = (segB[2] - segB[3]) / dest.w;
					p->incG = (segG[2] - segG[3]) / dest.w;
					p->incR = (segR[2] - segR[3]) / dest.w;
					p->incA = (segA[2] - segA[3]) / dest.w;
				}

				// If we don't have any x-gradient we can fill in y-gradient once and for all

				if (bTintY && !bTintX)
				{
					auto p = &pGradientsY[seg];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[3] - segB[0]) / dest.h;
					p->incG = (segG[3] - segG[0]) / dest.h;
					p->incR = (segR[3] - segR[0]) / dest.h;
					p->incA = (segA[3] - segA[0]) / dest.h;
				}

				// Possibly mark this segment as transparent

				transparentSegments[seg] = (segA[0] + segA[1] + segA[2] + segA[3] == 0);
				opaqueSegments[seg] = (segA[0] + segA[1] + segA[2] + segA[3] == 4096*4096*4);
			}
		}

		// Modify opaqueSegments if our state isn't blend
		
		if( m_blendMode != BlendMode::Blend )
		{
			bool val = (m_blendMode == BlendMode::Replace);
			
			for (int seg = 0; seg < nSegments; seg++)
				opaqueSegments[seg] = val;
		}
		
		
		// Set start position and clip dest

		uint8_t* pOrigo = m_pCanvasPixels + start.y * yPitch + start.x * xPitch;

		if (!dest.intersectsWith(m_clipBounds/64))
			return;

		SegmentOp_p	pOp = s_segmentOpTab[(int)bTintY][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		if (pOp == nullptr )
		{
			if( m_blendMode == BlendMode::Ignore )
				return;
			
			char errorMsg[1024];
			
			sprintf(errorMsg, "Failed draw segments operation. SoftGfxDevice is missing segments kernel %s Y-tint for BlendMode::%s onto surface of PixelFormat:%s.",
				bTintY ? "with" : "without",
				toString(m_blendMode),
				toString(m_pRenderLayerSurface->pixelFormat()) );
			
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		// Loop through patches

		for (int patchIdx = 0; patchIdx < m_nClipRects; patchIdx++)
		{
			// Clip patch

			RectI patch = RectI::getIntersection(dest, m_pClipRects[patchIdx]/64);
			if (patch.w == 0 || patch.h == 0)
				continue;

			// Calculate stripstart, clipBeg/clipEnd and first edge for patch

			int columnOfs;
			int rowOfs;

			int columns;
			int rows;

			if (bHorizontalColumns)
			{
				columnOfs = colPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();
				rowOfs = rowPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();

				columns = patch.w;
				rows = patch.h;
			}
			else
			{
				columnOfs = colPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();
				rowOfs = rowPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();

				columns = patch.h;
				rows = patch.w;
			}

			const int* pEdgeStrips = _pEdgeStrips + columnOfs * edgeStripPitch;
			uint8_t* pStripStart = pOrigo + columnOfs * colPitch;

			int clipBeg = rowOfs * 256;
			int clipEnd = clipBeg + (rows * 256);

			for (int x = 0; x < columns; x++)
			{
				int nEdges = 0;
				int skippedSegments = 0;

				for (int y = 0; y < nSegments - 1; y++)
				{
					int beg = pEdgeStrips[y];
					int end = pEdgeStrips[y + edgeStripPitch];

					if (beg > end)
						swap(beg, end);

					if (beg >= clipEnd)
						break;

					if (end > clipBeg)
					{
						int coverageInc = (end == beg) ? 0 : (65536 * 256) / (end - beg);
						int coverage = 0;

						if (beg < clipBeg)
						{
							int cut = clipBeg - beg;
							beg = clipBeg;
							coverage += (coverageInc * cut) >> 8;
						}

						if (end > clipEnd)
							end = clipEnd;


						edges[nEdges].begin = beg;
						edges[nEdges].end = end;
						edges[nEdges].coverage = coverage;
						edges[nEdges].coverageInc = coverageInc;
						nEdges++;
					}
					else
						skippedSegments++;
				}

				// Update tinting if we have X-gradient

				const int16_t * pColors = &colors[skippedSegments][0];

				if (bTintX)
				{
					if (bTintY)
					{
						auto pOut = pGradientsY + skippedSegments;
						auto pIn = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++)
						{
							int beg = pIn[0].begB + pIn[0].incB * columnOfs;
							int inc = (pIn[1].begB + pIn[1].incB * columnOfs - beg) / dest.h;
							pOut[i].begB = beg;
							pOut[i].incB = inc;

							beg = pIn[0].begG + pIn[0].incG * columnOfs;
							inc = (pIn[1].begG + pIn[1].incG * columnOfs - beg) / dest.h;
							pOut[i].begG = beg;
							pOut[i].incG = inc;

							beg = pIn[0].begR + pIn[0].incR * columnOfs;
							inc = (pIn[1].begR + pIn[1].incR * columnOfs - beg) / dest.h;
							pOut[i].begR = beg;
							pOut[i].incR = inc;

							beg = pIn[0].begA + pIn[0].incA * columnOfs;
							inc = (pIn[1].begA + pIn[1].incA * columnOfs - beg) / dest.h;
							pOut[i].begA = beg;
							pOut[i].incA = inc;

							pIn += 2;
						}
					}
					else
					{
						// We just use the color array and fill in correct color values for this column.

						auto pGrad = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++)
						{
							colors[i][0] = (pGrad->begR + pGrad->incR * columnOfs) >> 12;
							colors[i][1] = (pGrad->begG + pGrad->incG * columnOfs) >> 12;
							colors[i][2] = (pGrad->begB + pGrad->incB * columnOfs) >> 12;
							colors[i][3] = (pGrad->begA + pGrad->incA * columnOfs) >> 12;
							pGrad += 2;												// Skipping bottom gradient data since we don't need it.
						}

						pColors = &colors[0][0];
					}
				}

				//
				auto gradientPointer = pGradientsY != nullptr ? pGradientsY + skippedSegments : nullptr;
				pOp(clipBeg, clipEnd, pStripStart, rowPitch, nEdges, edges, pColors, gradientPointer, transparentSegments + skippedSegments, opaqueSegments + skippedSegments, int(m_morphFactor*4096));
				pEdgeStrips += edgeStripPitch;
				pStripStart += colPitch;
				columnOfs++;
			}
		}

		// Free what we have reserved on the memStack.

		if (gradientsXBufferSize > 0)
			Base::memStackRelease(gradientsXBufferSize);

		if (gradientsYBufferSize > 0)
			Base::memStackRelease(gradientsYBufferSize);

	}




	//____ _colTransRect() _____________________________________________________

	void SoftGfxDevice::_colTransRect( int outB[4], int outG[4], int outR[4], int outA[], const RectI& dest)
	{
		// Then we tint with our out tint

		if (m_colTrans.mode == TintMode::None)
		{
			for (int i = 0; i < 4; i++)
			{
				outB[i] = 0x1000;
				outG[i] = 0x1000;
				outR[i] = 0x1000;
				outA[i] = 0x1000;
			}
		}
		else if (m_colTrans.mode == TintMode::Flat)
		{
			int r = m_colTrans.flatTintColor.r;
			int g = m_colTrans.flatTintColor.g;
			int b = m_colTrans.flatTintColor.b;
			int a = m_colTrans.flatTintColor.a;

			for (int i = 0; i < 4; i++)
			{
				outB[i] = b;
				outG[i] = g;
				outR[i] = r;
				outA[i] = a;
			}
		}
		else if (m_colTrans.mode == TintMode::GradientX)
		{
			int ofsX = dest.x - m_colTrans.tintRect.x;
			int xIncB = int(m_colTrans.topRightB - m_colTrans.topLeftB) / m_colTrans.tintRect.w;

			int leftB = (m_colTrans.topLeftB + xIncB * ofsX) >> 6;
			int rightB = (m_colTrans.topLeftB + xIncB * (ofsX + dest.w)) >> 6;

			outB[0] = leftB;
			outB[1] = rightB;
			outB[2] = rightB;
			outB[3] = leftB;

			//

			int xIncG = int(m_colTrans.topRightG - m_colTrans.topLeftG) / m_colTrans.tintRect.w;

			int leftG = (m_colTrans.topLeftG + xIncG * ofsX) >> 6;
			int rightG = (m_colTrans.topLeftG + xIncG * (ofsX + dest.w)) >> 6;

			outG[0] = leftG;
			outG[1] = rightG;
			outG[2] = rightG;
			outG[3] = leftG;

			//

			int xIncR = int(m_colTrans.topRightR - m_colTrans.topLeftR) / m_colTrans.tintRect.w;

			int leftR = (m_colTrans.topLeftR + xIncR * ofsX) >> 6;
			int rightR = (m_colTrans.topLeftR + xIncR * (ofsX + dest.w)) >> 6;

			outR[0] = leftR;
			outR[1] = rightR;
			outR[2] = rightR;
			outR[3] = leftR;

			//

			int xIncA = int(m_colTrans.topRightA - m_colTrans.topLeftA) / m_colTrans.tintRect.w;

			int leftA = (m_colTrans.topLeftA + xIncA * ofsX) >> 6;
			int rightA = (m_colTrans.topLeftA + xIncA * (ofsX + dest.w)) >> 6;

			outA[0] = leftA;
			outA[1] = rightA;
			outA[2] = rightA;
			outA[3] = leftA;

		}
		else if (m_colTrans.mode == TintMode::GradientY)
		{
			int ofsY = dest.y - m_colTrans.tintRect.y;

			int topB = (m_colTrans.topLeftB + m_colTrans.leftIncB * ofsY) >> 6;
			int bottomB = (m_colTrans.topLeftB + m_colTrans.leftIncB * (ofsY + dest.h)) >> 6;

			outB[0] = topB;
			outB[1] = topB;
			outB[2] = bottomB;
			outB[3] = bottomB;

			//

			int topG = (m_colTrans.topLeftG + m_colTrans.leftIncG * ofsY) >> 6;
			int bottomG = (m_colTrans.topLeftG + m_colTrans.leftIncG * (ofsY + dest.h)) >> 6;

			outG[0] = topG;
			outG[1] = topG;
			outG[2] = bottomG;
			outG[3] = bottomG;

			//

			int topR = (m_colTrans.topLeftR + m_colTrans.leftIncR * ofsY) >> 6;
			int bottomR = (m_colTrans.topLeftR + m_colTrans.leftIncR * (ofsY + dest.h)) >> 6;

			outR[0] = topR;
			outR[1] = topR;
			outR[2] = bottomR;
			outR[3] = bottomR;

			//

			int topA = (m_colTrans.topLeftA + m_colTrans.leftIncA * ofsY) >> 6;
			int bottomA = (m_colTrans.topLeftA + m_colTrans.leftIncA * (ofsY + dest.h)) >> 6;

			outA[0] = topA;
			outA[1] = topA;
			outA[2] = bottomA;
			outA[3] = bottomA;
		}
		else if (m_colTrans.mode == TintMode::GradientXY)
		{
			CoordI ofs = dest.pos() - m_colTrans.tintRect.pos();

			int begB = m_colTrans.topLeftB + m_colTrans.leftIncB * ofs.y;
			int xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * ofs.y - begB) / m_colTrans.tintRect.w;

			outB[0] = (begB + xIncB * ofs.x) >> 6;
			outB[1] = (begB + xIncB * (ofs.x + dest.w)) >> 6;

			begB = m_colTrans.topLeftB + m_colTrans.leftIncB * (ofs.y + dest.h);
			xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * (ofs.y + dest.h) - begB) / m_colTrans.tintRect.w;

			outB[2] = (begB + xIncB * (ofs.x + dest.w)) >> 6;
			outB[3] = (begB + xIncB * ofs.x) >> 6;

			//

			int begG = m_colTrans.topLeftG + m_colTrans.leftIncG * ofs.y;
			int xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * ofs.y - begG) / m_colTrans.tintRect.w;

			outG[0] = (begG + xIncG * ofs.x) >> 6;
			outG[1] = (begG + xIncG * (ofs.x + dest.w)) >> 6;

			begG = m_colTrans.topLeftG + m_colTrans.leftIncG * (ofs.y + dest.h);
			xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * (ofs.y + dest.h) - begG) / m_colTrans.tintRect.w;

			outG[2] = (begG + xIncG * (ofs.x + dest.w)) >> 6;
			outG[3] = (begG + xIncG * ofs.x) >> 6;

			//

			int begR = m_colTrans.topLeftR + m_colTrans.leftIncR * ofs.y;
			int xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * ofs.y - begR) / m_colTrans.tintRect.w;

			outR[0] = (begR + xIncR * ofs.x) >> 6;
			outR[1] = (begR + xIncR * (ofs.x + dest.w)) >> 6;

			begR = m_colTrans.topLeftR + m_colTrans.leftIncR * (ofs.y + dest.h);
			xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * (ofs.y + dest.h) - begR) / m_colTrans.tintRect.w;

			outR[2] = (begR + xIncR * (ofs.x + dest.w)) >> 6;
			outR[3] = (begR + xIncR * ofs.x) >> 6;

			//

			int begA = m_colTrans.topLeftA + m_colTrans.leftIncA * ofs.y;
			int xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * ofs.y - begA) / m_colTrans.tintRect.w;

			outA[0] = (begA + xIncA * ofs.x) >> 6;
			outA[1] = (begA + xIncA * (ofs.x + dest.w)) >> 6;

			begA = m_colTrans.topLeftA + m_colTrans.leftIncA * (ofs.y + dest.h);
			xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * (ofs.y + dest.h) - begA) / m_colTrans.tintRect.w;

			outA[2] = (begA + xIncA * (ofs.x + dest.w)) >> 6;
			outA[3] = (begA + xIncA * ofs.x) >> 6;
		}
	}




	//____ _lineToEdges() __________________________________________________________

	void SoftGfxDevice::_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch )
	{
		static int brush[128];
		static float prevThickness = -1.f;

		float thickness = pWave->thickness;
		int brushSteps = (int)(thickness / 2 + 0.99f);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);

			brush[0] = scaledThickness;
			for (int i = 1; i < brushSteps; i++)
			{
				brush[i] = (scaledThickness * s_pCurveTab[c_nCurveTabEntries - (i*c_nCurveTabEntries) / brushSteps - 1]) >> 16;
				//				printf( "%d - %d - %d\n", i, brush[i], m_pCurveTab[0][(c_nCurveTabEntries - 1) - (i * c_nCurveTabEntries) / brushSteps]);
			}
			prevThickness = thickness;
		}

		int nTracePoints = max(0, min(nPoints, pWave->length - offset));
		int nFillPoints = nPoints - nTracePoints;

		// Trace...

		int * pSrc = pWave->pWave + offset;
		SegmentEdge * pWrite = pDest;
		for (int i = 0; i < nTracePoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i - j] - brush[j];
				int bottomCover = pSrc[i - j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints - i, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i + j] - brush[j];
				int bottomCover = pSrc[i + j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			pWrite[0].begin = top;
			pWrite[1].begin = bottom;


			pWrite += pitch;

		}

		// Fill out the edge specifications if we didn't have enough wave data

		if (nFillPoints)
		{
			int top = pWave->hold - brush[0];
			int bottom = pWave->hold + brush[0];

			for (int i = 0; i < nFillPoints; i++)
			{
				pWrite[0].begin = top;
				pWrite[0].end = top;
				pWrite[0].coverage = 0;
				pWrite[0].coverageInc = 0;

				pWrite[1].begin = bottom;
				pWrite[1].end = bottom;
				pWrite[1].coverage = 0;
				pWrite[1].coverageInc = 0;

				pWrite += pitch;
			}
		}

		// Calculate rest of the edge data for our traced points (not needed for the filled points)

		int nEdges = nFillPoints > 0 ? nTracePoints : nTracePoints-1;

		pWrite = pDest;
		for (int i = 0; i < nEdges; i++)
		{
			{
				int edgeBegin = pWrite[0].begin;
				int edgeEnd = pWrite[pitch].begin;
				int coverageStart = 0;

				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);


				pWrite[0].begin = edgeBegin;
				pWrite[0].end = edgeEnd;
				pWrite[0].coverage = coverageStart;
				pWrite[0].coverageInc = coverageInc;
			}

			{
				int edgeBegin = pWrite[1].begin;
				int edgeEnd = pWrite[1+pitch].begin;
				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);

				int coverageStart = 0;

				pWrite[1].begin = edgeBegin;
				pWrite[1].end = edgeEnd;
				pWrite[1].coverage = coverageStart;
				pWrite[1].coverageInc = coverageInc;
			}

			pWrite += pitch;
		}
	}

	//____ plotPixels() _________________________________________________

	void SoftGfxDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
	{
		const int pitch = m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits / 8;

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		if (pOp == nullptr )
		{
			if( m_blendMode == BlendMode::Ignore )
				return;
			
			char errorMsg[1024];
			
			sprintf(errorMsg, "Failed plotPixels operation. SoftGfxDevice is missing plotList kernel for BlendMode::%s onto surface of PixelFormat:%s.",
				toString(m_blendMode),
				toString(m_pRenderLayerSurface->pixelFormat()) );
			
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		for (int i = 0; i < m_nClipRects; i++)
			pOp(m_pClipRects[i], nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch, m_colTrans);
	}

	//____ setBlitSource() ____________________________________________________

	bool SoftGfxDevice::setBlitSource(Surface * pSource)
	{
		if (pSource == m_pBlitSource)
			return true;

		if (!pSource || !pSource->isInstanceOf(SoftSurface::TYPEINFO))
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		SoftSurface * pSrcSurf = (SoftSurface*)pSource;

		if (!pSrcSurf->m_pData)
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		m_pBlitSource = pSrcSurf;
		GfxDevice::m_pBlitSource = pSource;

		_updateBlitFunctions();
		return true;
	}

	//____ transformBlit() _______________________________________________________

	void SoftGfxDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
		if (m_pBlitSource->isTiling())
		{
			m_bTileSource = true;
			_transformBlit(dest, srcSPX, transform);
			m_bTileSource = false;
		}
		else
		{
			m_bClipSource = true;
			_transformBlit(dest, srcSPX, transform);
			m_bClipSource = false;
		}
	}



	//____ _rotScaleBlit() ____________________________________________________

	void SoftGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter )
	{
		if (m_pBlitSource->isTiling())
		{
			m_bTileSource = true;
			GfxDevice::rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter);
			m_bTileSource = false;
		}
		else
		{
			m_bClipSource = true;
			GfxDevice::rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter);
			m_bClipSource = false;
		}
	}

	//____ tile() _____________________________________________________________

	void SoftGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
	{
		m_bTileSource = true;
		GfxDevice::tile(dest, shift);
		m_bTileSource = false;
	}

	//____ flipTile() _________________________________________________________

	void SoftGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
	{
		m_bTileSource = true;
		GfxDevice::flipTile(dest, flip, shift);
		m_bTileSource = false;
	}

	//____ scaleTile() ________________________________________________________

	void SoftGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
	{
		m_bTileSource = true;
		GfxDevice::scaleTile(dest, scale, shift);
		m_bTileSource = false;
	}

	//____ scaleFlipTile() ____________________________________________________

	void SoftGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
	{
		m_bTileSource = true;
		GfxDevice::scaleFlipTile(dest, scale, flip, shift);
		m_bTileSource = false;
	}

	//____ _canvasWasChanged() ________________________________________________

	void SoftGfxDevice::_canvasWasChanged()
	{
		Surface * pCanvasSurface = nullptr;
		
		if(m_canvas.pSurface)
			pCanvasSurface = m_canvas.pSurface;
		else if(m_canvas.ref != CanvasRef::None)
		{
			CanvasRef ref = m_canvas.ref;
			
			auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );
			
			if( it != m_definedCanvases.end() )
				pCanvasSurface = it->pSurface;
		}
		
		
		if (pCanvasSurface)
		{
            m_layerSurfaces[0] = pCanvasSurface;
			_renderLayerWasChanged();
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

			_updateBlitFunctions();		// Good to have dummies in place when we are not allowed to blit.
		}
	}

	//____ _renderLayerWasChanged() ___________________________________________

	void SoftGfxDevice::_renderLayerWasChanged()
	{
		// Finish up previous render layer surface

		if (m_pRenderLayerSurface)
		{
			m_pRenderLayerSurface->pullPixels(m_canvasPixelBuffer);
			m_pRenderLayerSurface->freePixelBuffer(m_canvasPixelBuffer);
		}

		// Create our layer surface if needed

		bool	bClear = false;
		if (m_renderLayer > 0 && m_layerSurfaces[m_renderLayer] == nullptr)
		{
			m_layerSurfaces[m_renderLayer] = SoftSurface::create(m_canvas.size/64, m_pCanvasLayers->layerFormat(m_renderLayer));
			bClear = true;
		}

		// 

		m_pRenderLayerSurface = m_layerSurfaces[m_renderLayer];

		m_canvasPixelBuffer = m_pRenderLayerSurface->allocPixelBuffer();
		m_pRenderLayerSurface->pushPixels(m_canvasPixelBuffer);

		m_pCanvasPixels = m_canvasPixelBuffer.pPixels;
		m_canvasPitch = m_canvasPixelBuffer.pitch;

		m_canvasPixelBits = m_pRenderLayerSurface->pixelDescription()->bits;

		_updateTintSettings();
		_updateBlitFunctions();

		if (bClear)
			_clearRenderLayer();
	}

	//____ _transformBlit() [straight] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectSPX& _dest, CoordSPX _src, const int simpleTransform[2][2])
	{
		// For this method, source and dest should be pixel aligned.

		// Clip and render the patches

		if (!_dest.intersectsWith(m_clipBounds))
			return;


		//TODO: Proper 26:6 support
		RectI dest = roundToPixels(_dest);

		const RectI& clip = dest;

		// Step forward _src by half a pixel, so we start from correct pixel.

		_src.x = _src.x + (simpleTransform[0][0] + simpleTransform[1][0])*32;
		_src.y = _src.y + (simpleTransform[0][1] + simpleTransform[1][1])*32;

		//

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i]/64;

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

			if( m_bTileSource )
				(this->*m_pStraightTileOp)(patch, src, simpleTransform, patch.pos(), m_pStraightTileFirstPassOp);
			else
				(this->*m_pStraightBlitOp)(patch, src, simpleTransform, patch.pos(), m_pStraightBlitFirstPassOp);
		}
	}

	//____ _transformBlit() [transform] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectSPX& _dest, CoordF _src, const float complexTransform[2][2])
	{
		// Clip and render the patches

		if (!_dest.intersectsWith(m_clipBounds))
			return;

		//TODO: Proper 26:6 support
		RectI dest = roundToPixels(_dest);

		const RectI& clip = dest;

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i]/64;

			CoordF src = CoordF(_src)/64;

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

			src.x += patchOfs.x * complexTransform[0][0] + patchOfs.y * complexTransform[1][0];
			src.y += patchOfs.x * complexTransform[0][1] + patchOfs.y * complexTransform[1][1];

			// See if we can skip the expensive source clipping....

			bool bClipSource = false;
			if (m_bClipSource)
			{
				SizeF clipMax = static_cast<SizeF>(m_pBlitSource->m_size);

				if (m_pBlitSource->m_sampleMethod == SampleMethod::Bilinear)
					clipMax -= SizeF(1.f, 1.f);

				CoordF src2 = src;		// Source pixel to read for top-right destination corner.
				CoordF src3 = src;		// Source pixel to read for bottom-right destination corner.
				CoordF src4 = src;		// Source pixel to read for bottom-left destination corner.

				src2.x += (patch.w - 1) * complexTransform[0][0];
				src2.y += (patch.w - 1) * complexTransform[0][1];

				src3.x += (patch.w - 1) * complexTransform[0][0] + (patch.h - 1) * complexTransform[1][0];
				src3.y += (patch.w - 1) * complexTransform[0][1] + (patch.h - 1) * complexTransform[1][1];

				src4.x += (patch.h - 1) * complexTransform[1][0];
				src4.y += (patch.h - 1) * complexTransform[1][1];

				if (src.x < 0 || src.y < 0 || src.x >= clipMax.w || src.y >= clipMax.h ||
					src2.x < 0 || src2.y < 0 || src2.x > clipMax.w || src2.y > clipMax.h ||
					src3.x < 0 || src3.y < 0 || src3.x > clipMax.w || src3.y > clipMax.h ||
					src4.x < 0 || src4.y < 0 || src4.x > clipMax.w || src4.y > clipMax.h )
				{
					bClipSource = true;
				}
			}

			//

			if( m_bTileSource )
				(this->*m_pTransformTileOp)(patch, src, complexTransform, patch.pos(), m_pTransformTileFirstPassOp);
			else if( bClipSource )
				(this->*m_pTransformClipBlitOp)(patch, src, complexTransform, patch.pos(), m_pTransformClipBlitFirstPassOp);
			else
				(this->*m_pTransformBlitOp)(patch, src, complexTransform,patch.pos(), m_pTransformBlitFirstPassOp);
		}
	}

	//____ _onePassStraightBlit() _____________________________________________

	void SoftGfxDevice::_onePassStraightBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
	{
		const SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitches;

		pitches.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitches.dstX = dstPixelBytes;
		pitches.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitches.srcX*dest.w;
		pitches.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;
		uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + src.x * srcPixelBytes;

		pPassOneOp(pSrc, pDst, pSource, pitches, dest.h, dest.w, m_colTrans, patchPos, simpleTransform);
	}

	//____ _twoPassStraightBlit() _____________________________________________

	void SoftGfxDevice::_twoPassStraightBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
	{
		SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass1, pitchesPass2;

		pitchesPass1.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitchesPass1.dstX = 8;
		pitchesPass1.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitchesPass1.srcX*dest.w;
		pitchesPass1.dstY = 0;

		pitchesPass2.srcX = 8;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w*8;

		uint8_t * pChunkBuffer = (uint8_t*) Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y+line) * m_canvasPitch + dest.x * dstPixelBytes;
			uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + line*(srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1]) + src.x * srcPixelBytes;
//			uint8_t * pSrc = pSource->m_pData + (src.y+line) * pSource->m_pitch + src.x * srcPixelBytes;

			pPassOneOp(pSrc, pChunkBuffer, pSource, pitchesPass1, thisChunkLines, dest.w, m_colTrans, { 0,0 }, simpleTransform);
			m_pBlitSecondPassOp(pChunkBuffer, pDst, pSource, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos, nullptr);

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _onePassTransforBlit() ____________________________________________

	void SoftGfxDevice::_onePassTransformBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp)
	{
		const SoftSurface * pSource = m_pBlitSource;

		int dstPixelBytes = m_canvasPixelBits / 8;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;

		pPassOneOp(pSource, pos, transformMatrix, pDst, dstPixelBytes, m_canvasPitch - dstPixelBytes * dest.w, dest.h, dest.w, m_colTrans, patchPos);
	}


	//____ _twoPassTransformBlit() ____________________________________________

	void SoftGfxDevice::_twoPassTransformBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp)
	{
		const SoftSurface * pSource = m_pBlitSource;

		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass2;

		pitchesPass2.srcX = 8;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w * 8;

		uint8_t * pChunkBuffer = (uint8_t*)Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y + line) * m_canvasPitch + dest.x * dstPixelBytes;

			pPassOneOp(pSource, pos, transformMatrix, pChunkBuffer, 8, 0, thisChunkLines, dest.w, m_colTrans, { 0,0 });
			m_pBlitSecondPassOp(pChunkBuffer, pDst, pSource, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos, nullptr);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _dummyStraightBlit() _________________________________________________

	void SoftGfxDevice::_dummyStraightBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp)
	{
		if( m_blendMode == BlendMode::Ignore )
			return;
			
		char errorMsg[1024];
		
		sprintf(errorMsg, "Failed blit operation. SoftGfxDevice is missing straight blit kernel for:\n source format = %s\n tile = %s\n tint mode = %s\n blend mode = %s\n, dest format = %s\n", toString(m_pBlitSource->pixelFormat()),
			m_bTileSource ? "true" : "false",
			toString(m_colTrans.mode),
			toString(m_blendMode),
			toString(m_pRenderLayerSurface->pixelFormat()) );
		
		Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
	}

	//____ _dummyTransformBlit() ________________________________________________

	void SoftGfxDevice::_dummyTransformBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp)
	{
		if( m_blendMode == BlendMode::Ignore )
			return;

		char errorMsg[1024];
		
		sprintf(errorMsg, "Failed blit operation. SoftGfxDevice is missing transform blit kernel for:\n source format = %s\n sample method = %s\n tile = %s\n clip = %s\n tint mode = %s\n blend mode = %s\n, dest format = %s\n", toString(m_pBlitSource->pixelFormat()),
			toString(m_pBlitSource->sampleMethod()),
			m_bTileSource ? "true" : "false",
			m_bClipSource ? "true" : "false",
			toString(m_colTrans.mode),
			toString(m_blendMode),
			toString(m_pRenderLayerSurface->pixelFormat()) );
		
		Base::handleError(ErrorSeverity::SilentFail, ErrorCode::RenderFailure, errorMsg, this, TYPEINFO, __func__, __FILE__, __LINE__);
	}


	//____ _updateBlitFunctions() _____________________________________________

	void SoftGfxDevice::_updateBlitFunctions()
	{
		// Sanity checking...

		if (!m_pRenderLayerSurface || !m_pBlitSource || !m_pCanvasPixels || !m_pBlitSource->m_pData)
		{
			m_pStraightBlitOp = &SoftGfxDevice::_dummyStraightBlit;
			m_pStraightTileOp = &SoftGfxDevice::_dummyStraightBlit;

			m_pTransformBlitOp = &SoftGfxDevice::_dummyTransformBlit;
			m_pTransformClipBlitOp = &SoftGfxDevice::_dummyTransformBlit;
			m_pTransformTileOp = &SoftGfxDevice::_dummyTransformBlit;
			return;
		}

		//

		SampleMethod	sampleMethod = m_pBlitSource->sampleMethod();
		PixelFormat		srcFormat = m_pBlitSource->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pRenderLayerSurface->pixelFormat();

		// Add fallback back to two-pass rendering.

		if (m_pRenderLayerSurface->pixelDescription()->bLinear && m_pBlitSource->pixelDescription()->bLinear)
		{
			m_pStraightBlitFirstPassOp = s_moveTo_internal_fast8_OpTab[(int)srcFormat][0];
			m_pStraightTileFirstPassOp = s_moveTo_internal_fast8_OpTab[(int)srcFormat][1];
			m_pTransformBlitFirstPassOp = s_transformTo_internal_fast8_OpTab[(int)srcFormat][(int)sampleMethod][0];
			m_pTransformClipBlitFirstPassOp = s_transformTo_internal_fast8_OpTab[(int)srcFormat][(int)sampleMethod][1];
			m_pTransformTileFirstPassOp = s_transformTo_internal_fast8_OpTab[(int)srcFormat][(int)sampleMethod][2];
			m_pBlitSecondPassOp = s_pass2OpTab_fast8[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		}
		else
		{
			m_pStraightBlitFirstPassOp = s_moveTo_internal_OpTab[(int)srcFormat][0];
			m_pStraightTileFirstPassOp = s_moveTo_internal_OpTab[(int)srcFormat][1];
			m_pTransformBlitFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][0];
			m_pTransformClipBlitFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][1];
			m_pTransformTileFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][2];
			m_pBlitSecondPassOp = s_pass2OpTab[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		}


		// Try to find a suitable one-pass operation


		int srcIndex = s_srcFmtToMtxOfsTab[(int)srcFormat];
		int dstIndex = s_dstFmtToMtxOfsTab[(int)dstFormat];
		int blendModeIndex = s_blendModeToMtxOfsTab[(int)m_blendMode];



		int straightBlitIndex = m_straightBlitKernelMatrix[srcIndex][0][(int)m_colTrans.mode][blendModeIndex][dstIndex];
		int straightTileIndex = m_straightBlitKernelMatrix[srcIndex][1][(int)m_colTrans.mode][blendModeIndex][dstIndex];
		int transformBlitIndex = m_transformBlitKernelMatrix[srcIndex][(int)sampleMethod][0][(int)m_colTrans.mode][blendModeIndex][dstIndex];
		int transformClipBlitIndex = m_transformBlitKernelMatrix[srcIndex][(int)sampleMethod][1][(int)m_colTrans.mode][blendModeIndex][dstIndex];
		int transformTileIndex = m_transformBlitKernelMatrix[srcIndex][(int)sampleMethod][2][(int)m_colTrans.mode][blendModeIndex][dstIndex];

		// Decide on Proxy function depending on what blit operations we got.


		if (straightBlitIndex >= 1)
		{
			m_pStraightBlitOp = &SoftGfxDevice::_onePassStraightBlit;
			m_pStraightBlitFirstPassOp = m_pStraightBlitKernels[straightBlitIndex - 1].pKernel;
		}
		else if (srcFormat == PixelFormat::BGRA_8_linear && m_pRenderLayerSurface->pixelDescription()->bLinear &&
			s_pass2OpTab_fast8[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()] )
		{
			m_pStraightBlitOp = &SoftGfxDevice::_onePassStraightBlit;
			m_pStraightBlitFirstPassOp = s_pass2OpTab_fast8[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		}
		else if (m_pStraightBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pStraightBlitOp = &SoftGfxDevice::_twoPassStraightBlit;
		else
			m_pStraightBlitOp = &SoftGfxDevice::_dummyStraightBlit;


		if (straightTileIndex >= 1)
		{
			m_pStraightTileOp = &SoftGfxDevice::_onePassStraightBlit;
			m_pStraightTileFirstPassOp = m_pStraightBlitKernels[straightTileIndex - 1].pKernel;
		}
		else if (m_pStraightTileFirstPassOp && m_pBlitSecondPassOp)
			m_pStraightTileOp = &SoftGfxDevice::_twoPassStraightBlit;
		else
			m_pStraightTileOp = &SoftGfxDevice::_dummyStraightBlit;


		if (transformBlitIndex >= 1)
		{
			m_pTransformBlitOp = &SoftGfxDevice::_onePassTransformBlit;
			m_pTransformBlitFirstPassOp = m_pTransformBlitKernels[transformBlitIndex - 1].pKernel;
		}
		else if (m_pTransformBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pTransformBlitOp = &SoftGfxDevice::_twoPassTransformBlit;
		else
			m_pTransformBlitOp = &SoftGfxDevice::_dummyTransformBlit;


		if (transformClipBlitIndex >= 1)
		{
			m_pTransformClipBlitOp = &SoftGfxDevice::_onePassTransformBlit;
			m_pTransformClipBlitFirstPassOp = m_pTransformBlitKernels[transformClipBlitIndex - 1].pKernel;
		}
		else if (m_pTransformClipBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pTransformClipBlitOp = &SoftGfxDevice::_twoPassTransformBlit;
		else
			m_pTransformClipBlitOp = &SoftGfxDevice::_dummyTransformBlit;


		if (transformTileIndex >= 1)
		{
			m_pTransformTileOp = &SoftGfxDevice::_onePassTransformBlit;
			m_pTransformTileFirstPassOp = m_pTransformBlitKernels[transformTileIndex - 1].pKernel;
		}
		else if (m_pTransformTileFirstPassOp && m_pBlitSecondPassOp)
			m_pTransformTileOp = &SoftGfxDevice::_twoPassTransformBlit;
		else
			m_pTransformTileOp = &SoftGfxDevice::_dummyTransformBlit;
	}

	//____ _initTables() ___________________________________________________________

	void SoftGfxDevice::_initTables()
	{
		// Init lineThicknessTable

		for( int i = 0 ; i < 17 ; i++ )
		{
			double b = i/16.0;
			m_lineThicknessTable[i] = (int) (Util::squareRoot( 1.0 + b*b ) * 65536);
		}

		// Clear kernel tables

		for (int i = 0; i < BlendMode_size; i++)
		{
			for (int j = 0; j < PixelFormat_size; j++)
			{
				for (int k = 0; k < TintMode_size; k++)
				{
					s_fillOpTab[k][i][j] = nullptr;
					s_pass2OpTab[k][i][j] = nullptr;
					s_pass2OpTab_fast8[k][i][j] = nullptr;

				}

				s_plotOpTab[i][j] = nullptr;
				s_lineOpTab[i][j] = nullptr;
				s_clipLineOpTab[i][j] = nullptr;
				s_plotListOpTab[i][j] = nullptr;
				s_segmentOpTab[0][i][j] = nullptr;
				s_segmentOpTab[1][i][j] = nullptr;
			}
		}

		for (int i = 0; i < PixelFormat_size; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				s_moveTo_internal_OpTab[i][j] = nullptr;
				s_moveTo_internal_fast8_OpTab[i][j] = nullptr;
			}

			for (int j = 0; j < 3; j++)
			{
				s_transformTo_internal_OpTab[i][0][j] = nullptr;
				s_transformTo_internal_OpTab[i][1][j] = nullptr;

				s_transformTo_internal_fast8_OpTab[i][0][j] = nullptr;
				s_transformTo_internal_fast8_OpTab[i][1][j] = nullptr;
			}
		}

		memset(m_straightBlitKernelMatrix, 0, sizeof(m_straightBlitKernelMatrix));
		memset(m_transformBlitKernelMatrix, 0, sizeof(m_transformBlitKernelMatrix));

		// Populate kernel tables

		_populateOpTab();

		_populateOptimizedBlits();
	}
		
	//____ _populateOptimizedBlits() __________________________________________

	void SoftGfxDevice::_populateOptimizedBlits()
	{
		m_pStraightBlitKernels = _getStraightBlitKernels();

		for (int i = 0; m_pStraightBlitKernels[i].pKernel != nullptr; i++)
		{
			const StraightBlitKernelEntry& entry = m_pStraightBlitKernels[i];

			int srcIndex = s_srcFmtToMtxOfsTab[(int)entry.srcFormat];
			int dstIndex = s_dstFmtToMtxOfsTab[(int)entry.destFormat];
			int blendIndex = s_blendModeToMtxOfsTab[(int)entry.blendMode];

			m_straightBlitKernelMatrix[srcIndex][entry.bTile][(int)entry.tintMode][blendIndex][dstIndex] = i+1;
		}

		//

		m_pTransformBlitKernels = _getTransformBlitKernels();

		for (int i = 0; m_pTransformBlitKernels[i].pKernel != nullptr ; i++)
		{
			const TransformBlitKernelEntry& entry = m_pTransformBlitKernels[i];

			int srcIndex = s_srcFmtToMtxOfsTab[(int)entry.srcFormat];
			int dstIndex = s_dstFmtToMtxOfsTab[(int)entry.destFormat];
			int blendIndex = s_blendModeToMtxOfsTab[(int)entry.blendMode];

			m_transformBlitKernelMatrix[srcIndex][(int)entry.sampleMethod][entry.normTileClip][(int)entry.tintMode][blendIndex][dstIndex] = i+1;
		}

	}

	//____ _scaleLineThickness() ___________________________________________________

	int SoftGfxDevice::_scaleLineThickness( float thickness, int slope )
	{
		slope = std::abs(slope);

		int scale = m_lineThicknessTable[slope>>12];

		if( slope < (1 << 16) )
		{
			int scale2 = m_lineThicknessTable[(slope>>12)+1];
			scale += ((scale2-scale)*(slope & 0xFFF)) >> 12;
		}

		return (int) (thickness * scale);
	}


} // namespace wg
