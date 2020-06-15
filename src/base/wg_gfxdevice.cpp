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

#include <algorithm>
#include <cmath>

#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>

using namespace std;

namespace wg
{

	const TypeInfo GfxDevice::TYPEINFO = { "GfxDevice", &Object::TYPEINFO };

	int GfxDevice::s_gfxDeviceCount = 0;
	int *	GfxDevice::s_pCurveTab = nullptr;

	// Transforms for flipping movement over SOURCE when blitting

	static const int blitFlipTransforms[GfxFlip_size][2][2] = { { 1,0,0,1 },			// Normal
																{ -1,0,0,1 },			// FlipX
																{ 1,0,0,-1 },			// FlipY
																{ 0,-1,1,0 },			// Rot90
																{ 0,1,1,0 },			// Rot90FlipX
																{ 0,-1,-1,0 },			// Rot90FlipY
																{ -1,0,0,-1 },			// Rot180
																{ 1,0,0,-1 },			// Rot180FlipX
																{ -1,0,0,1 },			// Rot180FlipY
																{ 0,1,-1,0 },			// Rot270
																{ 0,-1,-1,0 },			// Rot270FlipX
																{ 0,1,1,0 } };			// Rot270FlipY

	static const int blitFlipOffsets[GfxFlip_size][2] = {	{ 0,0 },				// Normal
															{ 1,0 },				// FlipX
															{ 0,1 },				// FlipY
															{ 0,1 },				// Rot90
															{ 0,0 },				// Rot90FlipX
															{ 1,1 },				// Rot90FlipY
															{ 1,1 },				// Rot180
															{ 0,1 },				// Rot180FlipX
															{ 1,0 },				// Rot180FlipY
															{ 1,0 },				// Rot270
															{ 1,1 },				// Rot270FlipX
															{ 0,0 } };				// Rot270FlipY

	// Transforms for flipping movement over CANVAS when drawing
/*
	static const int drawFlipTransforms[GfxFlip_size][2][2] = { { 1,0,0,1 },		// Normal
															{ -1,0,0,1 },			// FlipX
															{ 1,0,0,-1 },			// FlipY
															{ 0,1,-1,0 },			// Rot90
															{ 0,1,1,0 },			// Rot90FlipX
															{ 0,-1,-1,0 },			// Rot90FlipY
															{ -1,0,0,-1 },			// Rot180
															{ 1,0,0,-1 },			// Rot180FlipX
															{ -1,0,0,1 },			// Rot180FlipY
															{ 0,-1,1,0 },			// Rot270
															{ 0,-1,-1,0 },			// Rot270FlipX
															{ 0,1,1,0 } };			// Rot270FlipY
*/






	//____ constructor _____________________________________________________________

	GfxDevice::GfxDevice( SizeI canvasSize )
	{
		m_tintColor 		= Color(255,255,255);
		m_blendMode 		= BlendMode::Blend;
		m_canvasSize		= canvasSize;
		m_clipCanvas		= canvasSize;
		m_clipBounds		= canvasSize;
		m_pClipRects		= &m_clipCanvas;
		m_nClipRects		= 1;

		if (s_gfxDeviceCount == 0)
		{
			_genCurveTab();
		}
		s_gfxDeviceCount++;
	}

	//____ Destructor _________________________________________________________

	GfxDevice::~GfxDevice()
	{
		s_gfxDeviceCount--;
		if (s_gfxDeviceCount == 0)
		{
			delete [] s_pCurveTab;
		}
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setClipList() __________________________________________________________

	bool GfxDevice::setClipList(int nRectangles, const RectI * pRectangles)
	{
		if (nRectangles == 0)
		{
			m_clipCanvas = m_canvasSize;
			m_clipBounds = {0,0,0,0};
			m_pClipRects = &m_clipCanvas;
			m_nClipRects = 0;
			return true;
		}

		RectI bounds = *pRectangles;
		for (int i = 1; i < nRectangles; i++)
			bounds.growToContain(pRectangles[i]);

		if (bounds.x < 0 || bounds.y < 0 || bounds.w > m_canvasSize.w || bounds.h > m_canvasSize.h)
			return false;

		m_pClipRects = pRectangles;
		m_nClipRects = nRectangles;
		m_clipBounds = bounds;
		return true;
	}

	//____ clearClipList() __________________________________________________________

	void GfxDevice::clearClipList()
	{
		m_clipBounds = m_canvasSize;
		m_clipCanvas = m_canvasSize;
		m_pClipRects = &m_clipCanvas;
		m_nClipRects = 1;
	}

	//____ setTintColor() __________________________________________________________

	void GfxDevice::setTintColor( Color color )
	{
		m_tintColor = color;
	}

	//____ setTintGradient() __________________________________________________

	void GfxDevice::setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft)
	{
		m_tintGradientRect = rect;
		m_tintGradient[0] = topLeft;
		m_tintGradient[1] = topRight;
		m_tintGradient[2] = bottomRight;
		m_tintGradient[3] = bottomLeft;
		m_bTintGradient = true;
	}

	//____ clearTintGradient() ________________________________________________

	void GfxDevice::clearTintGradient()
	{
		m_bTintGradient = false;
	}

	//____ setBlendMode() __________________________________________________________

	bool GfxDevice::setBlendMode( BlendMode blendMode )
	{
		if (blendMode == BlendMode::Undefined)
			m_blendMode = BlendMode::Blend;
		else
			m_blendMode = blendMode;

		return true;
	}

	//____ setBlitSource() ____________________________________________________

	bool GfxDevice::setBlitSource(Surface * pSource)
	{
		m_pBlitSource = pSource;
		return true;
	}

	//____ setMorphFactor() ____________________________________________________

	void GfxDevice::setMorphFactor(float factor)
	{
		limit(factor, 0.f, 1.f);
		m_morphFactor = factor;
	}

	//____ beginRender() ___________________________________________________________

	bool GfxDevice::beginRender()
	{
		if( m_bRendering )
			return false;

		m_bRendering = true;
		return true;	// Assumed to be ok if device doesn't have its own method.
	}

	//____ endRender() ________________________________________________________

	bool GfxDevice::endRender()
	{
		if( !m_bRendering )
			return false;

		m_bRendering = false;
		return true;	// Assumed to be ok if device doesn't have its own method.
	}

	//____ isRendering() _______________________________________________________
	/**
	 * @brief Check if GfxDevice is in rendering state.
	 *
	 * Check if GfxDevice is in rendering state, e.g. beginRender() has been called
	 * but not yet followed by endRender().
	 *
	 * Please note that GfxDevice might remain busy for an undefined time drawing
	 * graphics after endRender() has been called if rendering is done
	 * asynchronously. To determine if all graphics has been rendered to the
	 * framebuffer you should call isIdle() instead.
	 *
	 * @return True if between beginRender()/endRender() calls.
	 */

	bool GfxDevice::isRendering()
	{
		return m_bRendering;
	};

	//____ isIdle() ____________________________________________________________
	/**
	 * @brief Check if all rendering has completed.
	 *
	 * Check if GfxDevice has fully completed rendering all issued graphics commands
	 * and the canvas content is up-to-date.
	 *
	 * This method will start returning false once beginRender() has been called
	 * and return true again some time after endRender() has been called.
	 *
	 * @return True if all rendering has completed.
	 */

	bool GfxDevice::isIdle()
	{
		return !m_bRendering;
	}

	//____ fill() ______________________________________________________
	/**
	 * @brief Fill the canvas with specified color.
	 *
	 * Fills the entire canvas (or whatever areas are allowed by the
	 * clip list) with the specified color.
	 *
	 * @return Nothing.
	 */

	void GfxDevice::fill(const Color& col)
	{
		fill(m_canvasSize, col);
	}

	//____ drawLine() __________________________________________________

	void GfxDevice::drawLine(CoordI begin, Direction dir, int length, Color color, float thickness)
	{
		CoordI end;

		switch (dir)
		{
		case Direction::Up:
			end.x = begin.x;
			end.y = begin.y - length;
			break;
		case Direction::Down:
			end.x = begin.x;
			end.y = begin.y + length;
			break;
		case Direction::Left:
			end.x = begin.x - length;
			end.y = begin.y;
			break;
		case Direction::Right:
			end.x = begin.x + length;
			end.y = begin.y;
			break;
		}

		drawLine(begin, end, color, thickness);
	}


	//____ blit() __________________________________________________________________

	void GfxDevice::blit(CoordI dest)
	{
		assert(m_pBlitSource != nullptr);

		_transformBlit({ dest, m_pBlitSource->size() }, { 0,0 }, blitFlipTransforms[0]);
	}

	void GfxDevice::blit(CoordI dest, const RectI& src)
	{
		assert(m_pBlitSource != nullptr);

		_transformBlit({ dest, src.size() }, src.pos(), blitFlipTransforms[0]);
	}

	//____ flipBlit() _________________________________________________________

	void GfxDevice::flipBlit(CoordI dest, GfxFlip flip)
	{
		assert(m_pBlitSource != nullptr);

		SizeI srcSize  = m_pBlitSource->size();

		int ofsX = srcSize.w * blitFlipOffsets[(int)flip][0];
		int ofsY = srcSize.h * blitFlipOffsets[(int)flip][1];

		SizeI dstSize = srcSize;
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);

		_transformBlit({ dest, dstSize }, { ofsX, ofsY }, blitFlipTransforms[(int)flip]);
	}

	void GfxDevice::flipBlit(CoordI dest, const RectI& src, GfxFlip flip)
	{
		assert(m_pBlitSource != nullptr);

		int ofsX = src.x + src.w * blitFlipOffsets[(int)flip][0];
		int ofsY = src.y + src.h * blitFlipOffsets[(int)flip][1];

		SizeI dstSize = src.size();
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);


		_transformBlit({ dest, dstSize }, src.pos() + CoordI(ofsX, ofsY), blitFlipTransforms[(int)flip]);
	}



	//____ stretchBlit() ___________________________________________________________

	void GfxDevice::stretchBlit(const RectI& dest )
	{
		assert(m_pBlitSource != nullptr);

		stretchBlit(dest, RectI(0, 0, m_pBlitSource->size()) );
	}

	void GfxDevice::stretchBlit(const RectI& dest, const RectI& _src )
	{
		assert(m_pBlitSource != nullptr);

		if (m_pBlitSource == nullptr || dest.w == 0 || dest.h == 0 || _src.w == 0 || _src.h == 0)
			return;

		if (dest.w == _src.w && dest.h == _src.h)
		{
			// This is a 1:1 blit, let's use the fast alternative.

			_transformBlit( dest, _src.pos(), blitFlipTransforms[0]);
		}
		else
		{
			RectF src{ (float)_src.x, (float)_src.y, (float)_src.w, (float)_src.h };

			float	mtx[2][2];

			if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
			{
	//			src.x += 0.5f;
	//			src.y += 0.5f;

				if (dest.w == 1)
					mtx[0][0] = 0;
				else
					mtx[0][0] = (src.w-1) / (dest.w-1);

				mtx[0][1] = 0;
				mtx[1][0] = 0;

				if( dest.h == 1)
					mtx[1][1] = 0;
				else
					mtx[1][1] = (src.h-1) / (dest.h-1);
			}
			else
			{
				// We want last src sample to be taken as close to the end of the source
				// rectangle as possible in order to get a more balanced representation.

				mtx[0][0] = src.w / (dest.w-0.99f);
				mtx[0][1] = 0;
				mtx[1][0] = 0;
				mtx[1][1] = src.h / (dest.h-0.99f);
			}

			_transformBlit(dest, { src.x, src.y }, mtx);
		}
	}

	void GfxDevice::stretchBlit(const RectI& dest, const RectF& src)
	{
		assert(m_pBlitSource != nullptr);

		float	mtx[2][2];

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			mtx[0][0] = src.w / dest.w;
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / dest.h;
		}
		else
		{
			mtx[0][0] = src.w / dest.w;
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / dest.h;
		}

		_transformBlit(dest, { src.x,src.y }, mtx );
	}

	//____ stretchFlipBlit() _____________________________________________________

	void GfxDevice::stretchFlipBlit(const RectI& dest, GfxFlip flip)
	{
		assert(m_pBlitSource != nullptr);

		stretchFlipBlit(dest, RectI(0, 0, m_pBlitSource->size()), flip);
	}

	void GfxDevice::stretchFlipBlit(const RectI& dest, const RectI& src, GfxFlip flip)
	{
		assert(m_pBlitSource != nullptr);

		float scaleX, scaleY;
		float ofsX, ofsY;

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			float srcW = (float)(src.w - 1);
			float srcH = (float)(src.h - 1);

			scaleX = srcW / (dest.w-1);
			scaleY = srcH / (dest.h-1);

			ofsX = src.x + srcW * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + srcH * blitFlipOffsets[(int)flip][1];
		}
		else
		{
			float srcW = (float)src.w;
			float srcH = (float)src.h;

			scaleX = srcW / (dest.w - 0.99f);
			scaleY = srcH / (dest.h - 0.99f);

			ofsX = src.x + (srcW - scaleX) * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + (srcH - scaleY) * blitFlipOffsets[(int)flip][1];
		}

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		_transformBlit(dest, { ofsX, ofsY }, mtx);
	}

	void GfxDevice::stretchFlipBlit(const RectI& dest, const RectF& src, GfxFlip flip)
	{
		assert(m_pBlitSource != nullptr);

		float	scaleX = src.w / dest.w;
		float	scaleY = src.h / dest.h;

		float ofsX = src.x + (src.w-scaleX) * blitFlipOffsets[(int)flip][0];
		float ofsY = src.y + (src.h-scaleY) * blitFlipOffsets[(int)flip][1];

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		_transformBlit(dest, { ofsX, ofsY }, mtx);
	}

	//____ rotScaleBlit() _____________________________________________________

	void GfxDevice::rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
	{
		assert(m_pBlitSource != nullptr);

		if (scale <= 0.f || m_pBlitSource->m_size.w * scale < 1.f || m_pBlitSource->m_size.h * scale < 1.f )			// Values very close to zero gives overflow in calculations.
			return;

		CoordF	src;
		float	mtx[2][2];

		float	sz = (float)sin(-rotationDegrees*3.14159265/180);
		float	cz = (float)cos(-rotationDegrees*3.14159265 / 180);

		scale = 1.f / scale;

		mtx[0][0] = cz * scale;
		mtx[0][1] = sz * scale;

		mtx[1][0] = -sz * scale;
		mtx[1][1] = cz * scale;

		src = { srcCenter.x * m_pBlitSource->m_size.w, srcCenter.y * m_pBlitSource->m_size.h };
		 
//		src.x -= dest.w / 2.f * mtx[0][0] + dest.h / 2.f * mtx[1][0];
//		src.y -= dest.w / 2.f * mtx[0][1] + dest.h / 2.f * mtx[1][1];

		src.x -= dest.w * destCenter.x * mtx[0][0] + dest.h * destCenter.y * mtx[1][0];
		src.y -= dest.w * destCenter.x * mtx[0][1] + dest.h * destCenter.y * mtx[1][1];

		_transformBlit(dest, { src.x,src.y }, mtx);
	}


	//____ blitNinePatch() ________________________________________________

	void GfxDevice::blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const RectI& srcRect, const BorderI& srcFrame)
	{
		assert(m_pBlitSource != nullptr);

		if (srcRect.w == dstRect.w && srcRect.h == dstRect.h && srcFrame == dstFrame )
		{
			blit(dstRect.pos(), srcRect);
			return;
		}

		if (srcFrame.isEmpty() || dstFrame.isEmpty())
		{
			stretchBlit(dstRect, srcRect);
			return;
		}

		//TODO: Optimize! Call transformBlit directly instead of going through stretchBlit(), reuse transforms where possible.
		//TODO: Optimize! Use blit instead of stretchBlit on opportunity,fill if center is only 1 pixel and just blit corners if not stretched.

		SizeI srcMidSize(srcRect.w - srcFrame.left - srcFrame.right, srcRect.h - srcFrame.top - srcFrame.bottom);
		SizeI dstMidSize(dstRect.w - dstFrame.left - dstFrame.right, dstRect.h - dstFrame.top - dstFrame.bottom);

		if (srcFrame.top + dstFrame.top > 0)
		{
			RectI	srcNW(srcRect.x, srcRect.y, srcFrame.left, srcFrame.top);
			RectI	srcN(srcRect.x + srcFrame.left, srcRect.y, srcMidSize.w, srcFrame.top);
			RectI	srcNE(srcRect.x + srcRect.w-srcFrame.right, srcRect.y, srcFrame.right, srcFrame.top);

			RectI	dstNW(dstRect.x, dstRect.y, dstFrame.left, dstFrame.top);
			RectI	dstN(dstRect.x + dstFrame.left, dstRect.y, dstMidSize.w, dstFrame.top);
			RectI	dstNE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y, dstFrame.right, dstFrame.top);

			if (srcNW.w + dstNW.w > 0)
				stretchBlit(dstNW, srcNW);

			if (srcN.w + dstN.w > 0)
				stretchBlit(dstN, srcN);

			if (srcNE.w + dstNE.w > 0)
				stretchBlit(dstNE, srcNE);
		}


		if (srcMidSize.h > 0 && dstMidSize.h > 0)
		{
			RectI	srcW(srcRect.x, srcRect.y + srcFrame.top, srcFrame.left, srcMidSize.h);
			RectI	srcC(srcRect.x + srcFrame.left, srcRect.y + srcFrame.top, srcMidSize.w, srcMidSize.h);
			RectI	srcE(srcRect.x + srcRect.w - srcFrame.right, srcRect.y + srcFrame.top, srcFrame.right, srcMidSize.h);

			RectI	dstW(dstRect.x, dstRect.y + dstFrame.top, dstFrame.left, dstMidSize.h);
			RectI	dstC(dstRect.x + dstFrame.left, dstRect.y + dstFrame.top, dstMidSize.w, dstMidSize.h);
			RectI	dstE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y + dstFrame.top, dstFrame.right, dstMidSize.h);

			if (srcW.w + dstW.w > 0)
				stretchBlit(dstW, srcW);

			if (srcC.w + dstC.w > 0)
				stretchBlit(dstC, srcC);

			if (srcE.w + dstE.w > 0)
				stretchBlit(dstE, srcE);
		}

		if (srcFrame.bottom + dstFrame.bottom > 0)
		{
			RectI	srcSW(srcRect.x, srcRect.y + srcRect.h - srcFrame.bottom, srcFrame.left, srcFrame.bottom);
			RectI	srcS(srcRect.x + srcFrame.left, srcRect.y + srcRect.h - srcFrame.bottom, srcMidSize.w, srcFrame.bottom);
			RectI	srcSE(srcRect.x + srcRect.w - srcFrame.right, srcRect.y + srcRect.h - srcFrame.bottom, srcFrame.right, srcFrame.bottom);

			RectI	dstSW(dstRect.x, dstRect.y + dstRect.h - dstFrame.bottom, dstFrame.left, dstFrame.bottom);
			RectI	dstS(dstRect.x + dstFrame.left, dstRect.y + dstRect.h - dstFrame.bottom, dstMidSize.w, dstFrame.bottom);
			RectI	dstSE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y + dstRect.h - dstFrame.bottom, dstFrame.right, dstFrame.bottom);

			if (srcSW.w + dstSW.w > 0)
				stretchBlit(dstSW, srcSW);

			if (srcS.w + dstS.w > 0)
				stretchBlit(dstS, srcS);

			if (srcSE.w + dstSE.w > 0)
				stretchBlit(dstSE, srcSE);
		}

	}

	//____ drawWave() ______________________________________________________

	void GfxDevice::drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill )
	{
		_transformDrawWave(dest, pTopBorder, pBottomBorder, frontFill, backFill, blitFlipTransforms[(int)GfxFlip::Normal] );
	}

	//____ flipDrawWave() ______________________________________________________

	void GfxDevice::flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip )
	{
		_transformDrawWave(dest, pTopBorder, pBottomBorder, frontFill, backFill, blitFlipTransforms[(int)flip] );
	}

	//____ _transformDrawWave() ______________________________________________________

	void GfxDevice::_transformDrawWave(const RectI& _dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, const int simpleTransform[2][2] )
	{
		//TODO: If borders have different colors and cross, colors are not swapped.

		// Make a bounding box around all clip rectangles

		RectI bounds( m_clipBounds, _dest );

		if (bounds.w == 0 || bounds.h == 0)
			return;

		// Generate new destination rect and get waveLength, traceLength and clipping info.

		RectI	dest;
		int		clipBeg, clipEnd;
		int		length;
		int		traceLength;

		if (simpleTransform[0][0] != 0)								// Wave is horizontal
		{
			traceLength = _dest.w;
			dest = { bounds.x, _dest.y, bounds.w, _dest.h };
			clipBeg = bounds.x - _dest.x;
			clipEnd = (_dest.x + _dest.w) - (bounds.x + bounds.w);

			if (simpleTransform[0][0] < 0)							// Wave goes from right to left
				swap(clipBeg, clipEnd);

			length = bounds.w;
		}
		else														// Wave is vertical
		{
			traceLength = _dest.h;
			dest = { _dest.x, bounds.y, _dest.w, bounds.h };
			clipBeg = bounds.y - _dest.y;
			clipEnd = (_dest.y + _dest.h) - (bounds.y + bounds.h);

			if (simpleTransform[1][0] < 0)							// Wave goes from bottom to top
				swap(clipBeg, clipEnd);

			length = bounds.h;
		}

		// Do clipping of our trace

		int traceOfs = 0;
		int startColumn = clipBeg;										// Column of traced lines we start to generate edges from.

		int margin = (int)(max(pTopBorder->thickness, pBottomBorder->thickness) / 2 + 0.99);

		if (clipBeg > margin)
		{
			traceOfs = clipBeg - margin;
			traceLength -= traceOfs;
			startColumn -= traceOfs;
		}

		if (clipEnd > margin)
			traceLength -= clipEnd - margin;

		// Generate line traces

		int	lineBufferSize = (traceLength + 1) * 2 * sizeof(int) * 2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pBuffer = Base::memStackAlloc(lineBufferSize);
		int * pTopBorderTrace = (int*)pBuffer;
		int * pBottomBorderTrace = (int*)(pBuffer + lineBufferSize / 2);

		_traceLine(pTopBorderTrace, traceLength + 1, pTopBorder, traceOfs);
		_traceLine(pBottomBorderTrace, traceLength + 1, pBottomBorder, traceOfs);




		// Generate edges

		int edgeBufferSize;

		if (frontFill == backFill)
		{
			// Generate edges

			edgeBufferSize = (length + 1) * 4 * sizeof(int);
			int *	pEdgeBuffer = (int*)Base::memStackAlloc(edgeBufferSize);
			int *	pEdges = pEdgeBuffer;

			for (int i = startColumn; i <= length + startColumn; i++)
			{
				if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
					swap(pTopBorderTrace, pBottomBorderTrace);

				pEdges[0] = pTopBorderTrace[i * 2];
				pEdges[1] = pTopBorderTrace[i * 2 + 1];

				pEdges[2] = pBottomBorderTrace[i * 2];
				pEdges[3] = pBottomBorderTrace[i * 2 + 1];

				if (pEdges[2] < pEdges[1])
				{
					pEdges[2] = pEdges[1];
					if (pEdges[3] < pEdges[2])
						pEdges[3] = pEdges[2];
				}

				pEdges += 4;
			}

			// Render the segments

			Color	col[5];

			col[0] = Color::Transparent;
			col[1] = pTopBorder->color;
			col[2] = frontFill;
			col[3] = pBottomBorder->color;
			col[4] = Color::Transparent;

			_transformDrawSegments(dest, 5, col, length + 1, pEdgeBuffer, 4, TintMode::Flat, simpleTransform );

		}
		else
		{
			edgeBufferSize = (length + 1) * 5 * sizeof(int);
			int *	pEdgeBuffer = (int*)Base::memStackAlloc(edgeBufferSize);
			int *	pEdges = pEdgeBuffer;
			int		midEdgeFollows = 3;

			for (int i = startColumn; i <= length + startColumn; i++)
			{
				pEdges[0] = pTopBorderTrace[i * 2];
				pEdges[1] = pTopBorderTrace[i * 2 + 1];

				pEdges[3] = pBottomBorderTrace[i * 2];
				pEdges[4] = pBottomBorderTrace[i * 2 + 1];

				if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
				{
					swap(pTopBorderTrace, pBottomBorderTrace);
					midEdgeFollows ^= 0x2;												// Swap between 1 and 3.
				}

				if (pEdges[3] < pEdges[1])
				{
					pEdges[3] = pEdges[1];
					if (pEdges[4] < pEdges[3])
						pEdges[4] = pEdges[3];
				}

				pEdges[2] = pEdges[midEdgeFollows];
				pEdges += 5;
			}

			// Render the segments

			Color	col[6];

			col[0] = Color::Transparent;
			col[1] = pTopBorder->color;
			col[2] = frontFill;
			col[3] = backFill;
			col[4] = pBottomBorder->color;
			col[5] = Color::Transparent;

			_transformDrawSegments(dest, 6, col, length + 1, pEdgeBuffer, 5, TintMode::Flat, simpleTransform);
		}


		// Free temporary work memory

		Base::memStackRelease(edgeBufferSize);
		Base::memStackRelease(lineBufferSize);
	}

	//____ drawElipse() ______________________________________________________

	void GfxDevice::drawElipse(const RectF& canvas, float thickness, Color fillColor, float outlineThickness, Color outlineColor )
	{
		// Center and corners in 24.8 format.

		int x1 = (int)(canvas.x * 256);
		int y1 = (int)(canvas.y * 256);
		int x2 = (int)((canvas.x + canvas.w) * 256);
		int y2 = (int)((canvas.y + canvas.h) * 256);

		CoordI center = { (x1 + x2) / 2, (y1 + y2) / 2 };

		// Outer rect of elipse rounded to full pixels.

		RectI outerRect;
		outerRect.x = x1 >> 8;
		outerRect.y = y1 >> 8;
		outerRect.w = ((x2 + 255) >> 8) - outerRect.x;
		outerRect.h = ((y2 + 255) >> 8) - outerRect.y;

		// Adjusted clip

		RectI clip(m_clipBounds, outerRect);
		if (clip.w == 0 || clip.h == 0)
			return;

		int clipLeft = clip.x - outerRect.x;

		// Calculate maximum width and height from center for the 4 edges of the elipse.

		int radiusY[4];
		radiusY[0] = (y2 - y1) / 2;
		radiusY[1] = (int)(radiusY[0] - (outlineThickness * 256));
		radiusY[2] = (int)(radiusY[1] - (thickness * 256));
		radiusY[3] = (int)(radiusY[2] - (outlineThickness * 256));

		int radiusX[4];
		radiusX[0] = (x2 - x1) / 2;
		radiusX[1] = (int)(radiusX[0] - (outlineThickness * 256));
		radiusX[2] = (int)(radiusX[1] - (thickness * 256));
		radiusX[3] = (int)(radiusX[2] - (outlineThickness * 256));

		// Reserve buffer for our line traces

		int samplePoints = clip.w + 1;

		int bufferSize = samplePoints * sizeof(int) * 4 * 2;		// length+1 * sizeof(int) * 4 separate traces * 2 halves.
		int * pBuffer = (int*)Base::memStackAlloc(bufferSize);

		// Do line traces.

		int yMid = (center.y & 0xFFFFFF00) - outerRect.y * 256;
		int yAdjust = center.y & 0xFF;						// Compensate for center not being on pixel boundary.
		int centerOfs = center.x - (outerRect.x << 8);
		int samplePitch = 4;

		for (int edge = 0; edge < 4; edge++)
		{
			int * pOutUpper = pBuffer + edge;
			int * pOutLower = pBuffer + 3 - edge + samplePoints * 4;

			if (radiusX[edge] <= 0 || radiusY[edge] <= 0)
			{
				for (int sample = 0; sample < samplePoints; sample++)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
				}
			}
			else
			{
				int xStart = (centerOfs - radiusX[edge] + 255) >> 8;		// First pixel-edge inside curve.
				int xMid = centerOfs >> 8;								// Pixel edge on or right before center.
				int xEnd = (centerOfs + radiusX[edge]) >> 8;				// Last pixel-edge inside curve.


				int curveInc = int(((int64_t)65536) * 256 * (c_nCurveTabEntries - 1) / radiusX[edge]); // Keep as many decimals as possible, this is important!
				int curvePos = int((((radiusX[edge] - centerOfs) & 0xFF) * ((int64_t)curveInc)) >> 8);

				if (clipLeft > 0)
				{
					xStart -= clipLeft;
					xMid -= clipLeft;
					xEnd -= clipLeft;

					if (xStart < 0)
						curvePos += (-xStart) * curveInc;
				}

				// Clip xStart, xMid and xEnd

				if (xEnd >= samplePoints)
				{
					xEnd = samplePoints - 1;

					xStart = min(xStart, xEnd);
					xMid = min(xMid, xEnd);
				}

				//

				int sample = 0;
				while (sample < xStart)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
					sample++;
				}

				while (sample <= xMid)
				{
					int i = curvePos >> 16;
					uint32_t f = curvePos & 0xFFFF;

					uint32_t heightFactor = (s_pCurveTab[i] * (65535 - f) + s_pCurveTab[i + 1] * f) >> 16;
					int height = ((radiusY[edge] >> 16) * heightFactor) + ((radiusY[edge] & 0xFFFF) * heightFactor >> 16);  // = (radiusY[edge] * heightFactor) / 65536, but without overflow.

					pOutUpper[sample*samplePitch] = yMid + yAdjust - height;
					pOutLower[sample*samplePitch] = yMid + yAdjust + height;
					sample++;
					curvePos += curveInc;
				}

				curvePos = (c_nCurveTabEntries - 1) * 65536 * 2 - curvePos;

				while (sample <= xEnd)
				{
					int i = curvePos >> 16;
					uint32_t f = curvePos & 0xFFFF;

					uint32_t heightFactor = (s_pCurveTab[i] * (65535 - f) + s_pCurveTab[i + 1] * f) >> 16;
					int height = ((radiusY[edge] >> 16) * heightFactor) + ((radiusY[edge] & 0xFFFF) * heightFactor >> 16); // = (radiusY[edge] * heightFactor) / 65536, but without overflow.

					pOutUpper[sample*samplePitch] = yMid + yAdjust - height;
					pOutLower[sample*samplePitch] = yMid + yAdjust + height;
					sample++;
					curvePos -= curveInc;
				}

				while (sample < samplePoints)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
					sample++;
				}

				// Take care of left and right edges that needs more calculations to get the angle right.

				int pixFracLeft = (xStart << 8) - (centerOfs - radiusX[edge]);
				int pixFracRight = (centerOfs + radiusX[edge]) & 0xFF;

				if (pixFracLeft > 0 && xStart > 0)
				{
					pOutUpper[(xStart - 1)*samplePitch] = pOutUpper[xStart*samplePitch] + (yMid + yAdjust - pOutUpper[xStart*samplePitch]) * 256 / pixFracLeft;
					pOutLower[(xStart - 1)*samplePitch] = pOutLower[xStart*samplePitch] + (yMid + yAdjust - pOutLower[xStart*samplePitch]) * 256 / pixFracLeft;

				}
				if (pixFracRight > 0 && xEnd < samplePoints - 1)
				{
					pOutUpper[(xEnd + 1)*samplePitch] = pOutUpper[xEnd*samplePitch] + (yMid + yAdjust - pOutUpper[xEnd*samplePitch]) * 256 / pixFracRight;
					pOutLower[(xEnd + 1)*samplePitch] = pOutLower[xEnd*samplePitch] + (yMid + yAdjust - pOutLower[xEnd*samplePitch]) * 256 / pixFracRight;
				}

			}
		}

		// Split clip rectangles into upper and lower half, so we clip at the middle.

		int split = min(clip.y + clip.h, outerRect.y + (yMid >> 8));

		int clipBufferSize = sizeof(RectI)*m_nClipRects * 2;
		RectI * pTopClips = (RectI*)Base::memStackAlloc(clipBufferSize);
		RectI * pBottomClips = pTopClips + m_nClipRects;
		int nTopClips = 0;
		int nBottomClips = 0;

		for (int i = 0; i < m_nClipRects; i++)
		{
			const RectI& clip = m_pClipRects[i];

			if (clip.y < split)
			{
				pTopClips[nTopClips] = clip;
				if (clip.bottom() > split)
					pTopClips[nTopClips].h = split - clip.y;
				nTopClips++;
			}
			if (clip.bottom() > split)
			{
				pBottomClips[nBottomClips] = clip;
				if (clip.y < split)
				{
					pBottomClips[nBottomClips].h -= split - clip.y;
					pBottomClips[nBottomClips].y = split;
				}
				nBottomClips++;
			}
		}

		// Render columns

		Color	col[5];
		col[0] = Color::Transparent;
		col[1] = outlineColor;
		col[2] = fillColor;
		col[3] = outlineColor;
		col[4] = Color::Transparent;

		const RectI * pOldClipRects = m_pClipRects;
		int nOldClipRects = m_nClipRects;

		setClipList(nTopClips, pTopClips);
		drawSegments({clip.x,outerRect.y,clip.w,outerRect.h}, 5, col, samplePoints, pBuffer, 4);
		setClipList(nBottomClips, pBottomClips);
		drawSegments({clip.x,outerRect.y,clip.w,outerRect.h}, 5, col, samplePoints, pBuffer + samplePoints * 4, 4);
		setClipList(nOldClipRects, pOldClipRects);

		// Free temporary work memory

		Base::memStackRelease(clipBufferSize);
		Base::memStackRelease(bufferSize);

	}

	//____ drawPieChart() _____________________________________________________

	void GfxDevice::drawPieChart(const RectI& _canvas, float start, int nSlices, const float * _pSliceSizes, const Color * pSliceColors, float hubSize, Color hubColor, Color backColor, bool bRectangular)
	{
		static const int c_maxSlices = c_maxSegments - 2;

		if (nSlices < 0 || nSlices > c_maxSlices)
		{
			//TODO: Error handling;
			return;
		}

		//TODO: Early out if no slices, no hud and no background
		//TODO: Replace with fill if no hub, no slices.

		RectI canvas = _canvas;
		canvas.w = canvas.w + 1 & 0xFFFFFFFC;
		canvas.h = canvas.h + 1 & 0xFFFFFFFC;

		if (canvas.w <= 0 || canvas.h <= 0 || (hubSize == 1.f && !bRectangular))
			return;


		// Setup our slices

		struct Slice
		{
			float ofs;
			float size;
			Color color;
		};

		Slice slices[c_maxSlices+2];			// Maximum two extra slices in the end. Beginning offset + end transparency.

		float totalSize = 0.f;

		// Trim our slices, so we have a length of most 1.0.

		Slice trimmedSlices[c_maxSlices+1];

		float ofs = start;
		for (int i = 0; i < nSlices; i++)
		{
			if (ofs >= 1.f)
				ofs = fmod(ofs, 1.f);

			float sliceSize = _pSliceSizes[i];

			if (totalSize + sliceSize >= 1.f)
			{
				trimmedSlices[i] = { ofs, 1.f - totalSize, pSliceColors[i] };
				totalSize = 1.f;
				nSlices = i + 1;
				break;
			}


			trimmedSlices[i] = { ofs,sliceSize, pSliceColors[i] };
			totalSize += sliceSize;
			ofs += sliceSize;
		}

		// Adding an empty slice at end if totalSize < 1.0

		if (totalSize < 0.9999f)
		{
			if (ofs >= 1.f)
				ofs = fmod(ofs, 1.f);

			trimmedSlices[nSlices++] = { ofs,1.f-totalSize, backColor };
		}

		// Find first slice (one with smallest offset)

		int		firstSlice, lastSlice;
		float	firstSliceOfs = 1.f;

		for (int i = 0; i < nSlices; i++)
		{
			if (trimmedSlices[i].ofs < firstSliceOfs)
			{
				firstSlice = i;
				firstSliceOfs = trimmedSlices[i].ofs;
			}

		}

		// Find last slice

		if (firstSlice == 0)
			lastSlice = nSlices - 1;
		else
			lastSlice = firstSlice - 1;

		// Take care of possible rounding errors on inparameters

		if (firstSliceOfs < 0.0001f)
			trimmedSlices[firstSlice].ofs = firstSliceOfs = 0.f;

		// Rearrange our slices so we start from offset 0. Possibly adding one more slice in the beginning.

		int sliceIdx = 0;

		if (nSlices == 1)
		{
			slices[sliceIdx++] = { 0.f, 1.f, trimmedSlices[0].color };
		}
		else
		{
			// Fill in rollover from last slice (or transparent gap) due to rotation

			if (firstSliceOfs > 0.f )
				slices[sliceIdx++] = { 0.f, firstSliceOfs, trimmedSlices[lastSlice].color };

			// Our buffer is circular, take care of slices from first slice to end of buffer.

			for (int i = firstSlice; i < nSlices; i++)
				slices[sliceIdx++] = trimmedSlices[i];

			// Take care of slices from beginning of buffer to last slice.

			if (lastSlice < firstSlice)
			{
				for (int i = 0; i <= lastSlice; i++)
					slices[sliceIdx++] = trimmedSlices[i];
			}

			// Correct for any rollover or inprecision for last slice

			slices[sliceIdx - 1].size = 1.f - slices[sliceIdx - 1].ofs;
		}

		nSlices = sliceIdx;			// Repurpose this variable 

		// Slices now in order, lets render the quadrants

		int quadW = canvas.w / 2, quadH = canvas.h / 2;

		RectI quadCanvas[4] = { {canvas.x + quadW, canvas.y, quadW, quadH },
								{canvas.x + quadW, canvas.y + quadH, quadW, quadH},
								{canvas.x, canvas.y + quadH, quadW, quadH},
								{canvas.x, canvas.y, quadW, quadH} };

		GfxFlip quadFlip[4] = { GfxFlip::Normal, GfxFlip::Rot90, GfxFlip::Rot180, GfxFlip::Rot270 };

		Color colors[c_maxSegments];

		int maxSegments = nSlices + 2;
		int edgePitch = maxSegments - 1;
		int bufferSize = (quadW + 1) * edgePitch * sizeof(int);

		int * pBuffer = (int*) Base::memStackAlloc(bufferSize);


		// Setting the outer edge (same for all quads)

		if (bRectangular)
		{
			int * pEdge = pBuffer;
			for (int i = 0; i <= quadW; i++)
			{
				*pEdge = 0;
				pEdge += edgePitch;
			}
		}
		else
		{
			int * pEdge = pBuffer;

			int curveTabInc = ((c_nCurveTabEntries << 16)-1) / (quadW);

			int curveTabOfs = 0;
			for (int i = 0; i <= quadW; i++)
			{
				*pEdge = (quadH<<8) - ((s_pCurveTab[(c_nCurveTabEntries-1)-(curveTabOfs >> 16)] * quadH) >> 8);
				pEdge += edgePitch;
				curveTabOfs += curveTabInc;
			}
		}

		// Generating an inner edge if we have one.
		// Storing it separately, used for all 4 quads. Copied into right place later.

		int * pHubBuffer = nullptr;
		int hubBufferSize = 0;
		if (hubSize > 0.f)
		{
			hubBufferSize = (quadW + 1) * sizeof(int);
			pHubBuffer = (int*)Base::memStackAlloc(hubBufferSize);

			int * p = pHubBuffer;

			int ringW = int(hubSize * quadW);
			int ringH = int(hubSize * quadH);

			int inc = ((c_nCurveTabEntries << 16)-1) / (ringW);
			int ofs = 0;

			for (int i = 0; i <= ringW; i++)
			{
				*p++ = (quadH << 8) - ((s_pCurveTab[(c_nCurveTabEntries - 1) - (ofs >> 16)] * ringH) >> 8);
				ofs += inc;
			}

			int maxVal = quadH << 8;
			for (int i = ringW + 1; i <= quadW; i++)
				*p++ = maxVal;
		}

		//

		for (int quad = 0; quad < 4; quad++)
		{
			int nSegments = 0;
			Slice * pSlice = slices;
			Slice * pSliceEnd = slices + nSlices;

			float quadStartOfs = 0.25f * quad;

			// Add background as first segment

			colors[nSegments] = backColor;
			nSegments++;

			// Find first slice to include

			while (pSlice != pSliceEnd && pSlice->ofs + pSlice->size < quadStartOfs )
				pSlice++;

			colors[nSegments] = pSlice->color;
			nSegments++;
			pSlice++;

			// Generate edges for all following slices included

			while (pSlice != pSliceEnd && pSlice->ofs < quadStartOfs + 0.25f)
			{
				int * pEdge = pBuffer + nSegments-1;

				// Set startvalue and decrease per strip.

				int value = quadH << 8;
				int dec;

				float rot = (pSlice->ofs - quadStartOfs);

				if (rot == 0.f)
				{
					value = 0;
					dec = 0;
				}
				else
				{
					rot *= 3.14159265358979f * 2;
					float s = sin(rot);
					float c = cos(rot);

					float decF = (c/s) * (quadH << 12) / float(quadW);

					if (decF > 400000*4096)
						decF = 400000*4096;

					dec = int(decF);
				}

				// Fill in the edge

				int strip;
				int precisionValue = value << 4;
				for (strip = 0; strip <= quadW && value >= pEdge[-1] ; strip++)
				{
					* pEdge = value;
					pEdge += edgePitch;
					precisionValue -= dec;
					value = precisionValue >> 4;
				}

				while (strip <= quadW)
				{
					*pEdge = pEdge[-1];
					pEdge += edgePitch;
					strip++;
				}

				colors[nSegments] = pSlice->color;
				nSegments++;
				pSlice++;
			}

			// Add edge for hub if present

			if (pHubBuffer)
			{
				int * pEdge = pBuffer + nSegments - 1;

				for (int i = 0; i <= quadW; i++)
				{
					int value = pHubBuffer[i];
					*pEdge = value;

					int * pPrev = pEdge - 1;
					while (*pPrev > value)
					{
						* pPrev = value;
						pPrev--;
					}

					pEdge += edgePitch;
				}

				colors[nSegments] = hubColor;
				nSegments++;
			}

			// Draw

			int * pEdges = pBuffer;
			Color * pColors = colors;
			if (bRectangular)
			{
				pEdges++;
				pColors++;
				nSegments--;
			}

			uint8_t		alphaCheck = 0;
			for (int i = 0; i < nSegments; i++)
				alphaCheck |= pColors[i].a;

			if (alphaCheck == 0)
				continue;

			if (nSegments == 1)
				fill(quadCanvas[quad], pColors[0]);
			else
				_transformDrawSegments(quadCanvas[quad], nSegments, pColors, quadW+1, pEdges, edgePitch, TintMode::Flat, blitFlipTransforms[(int)quadFlip[quad]]);
		}

		if( hubBufferSize != 0 )
			Base::memStackRelease(hubBufferSize);

		Base::memStackRelease(bufferSize);
	}

	//____ drawSegments() ______________________________________________________

	void GfxDevice::drawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
	{
		_transformDrawSegments( dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, tintMode, blitFlipTransforms[(int)GfxFlip::Normal] );
	}

	//____ flipDrawSegments() ______________________________________________________

	void GfxDevice::flipDrawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
	{
		_transformDrawSegments(dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, tintMode, blitFlipTransforms[(int)flip] );
	}


	//____ blitHorrBar() ______________________________________________________

	void GfxDevice::blitHorrBar( 	  	const RectI& _src, const BorderI& _borders,
									  	bool _bTile, CoordI dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/

		// Blit left edge

		RectI	r( _src.x, _src.y, _borders.left, _src.h );
		blit( dest, r );

		_len -= _borders.width();			// Remove left and right edges from len.
		dest.x += _borders.left;

		// Blit tiling part

		r.x += _borders.left;
		r.w = _src.w - _borders.width();

		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( dest, r );
				_len -= r.w;
				dest.x += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( dest, r );
				dest.x += _len;
			}
		}
		else
		{
			stretchBlit( RectI(dest, _len, r.h), r );
			dest.x += _len;
		}

		// Blit right edge

		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( dest, r );
	}

	//____ blitVertBar() ______________________________________________________

	void GfxDevice::blitVertBar(	  	const RectI& _src, const BorderI& _borders,
									  	bool _bTile, CoordI dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/

		// Blit top edge

		RectI	r( _src.x, _src.y, _src.w, _borders.top );
		blit( dest, r );

		_len -= _borders.height();			// Remove top and bottom edges from len.
		dest.y += _borders.top;

		// Blit tiling part

		r.y += _borders.top;
		r.h = _src.h - _borders.height();

		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( dest, r );
				_len -= r.h;
				dest.y += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( dest, r );
				dest.y += _len;
			}
		}
		else
		{
			stretchBlit( RectI(dest, r.w, _len), r );
			dest.y += _len;
		}

		// Blit bottom edge

		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( dest, r );
	}

	//____ _genCurveTab() ___________________________________________________________

	void GfxDevice::_genCurveTab()
	{
		s_pCurveTab = new int[c_nCurveTabEntries];

		//		double factor = 3.14159265 / (2.0 * c_nCurveTabEntries);

		for (int i = 0; i < c_nCurveTabEntries; i++)
		{
			double y = 1.f - i / (double)c_nCurveTabEntries;
			s_pCurveTab[i] = (int)(Util::squareRoot(1.f - y*y)*65536.f);
		}
	}

	//____ _traceLine() __________________________________________________________

	void GfxDevice::_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset)
	{
		static const int c_supersamples = 4;

		static int brush[128 * c_supersamples];
		static float prevThickness = -1.f;

		float thickness = pWave->thickness;
		int brushSteps = (int)(thickness * c_supersamples / 2);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);

			brush[0] = scaledThickness;
			for (int i = 1; i <= brushSteps; i++)
				brush[i] = (scaledThickness * s_pCurveTab[c_nCurveTabEntries - (i*c_nCurveTabEntries) / brushSteps]) >> 16;
			prevThickness = thickness;
		}

		int nTracePoints = max(0, min(nPoints, pWave->length - offset));
		int nFillPoints = nPoints - nTracePoints;

		// Trace...

		int * pSrc = pWave->pWave + offset;
		for (int i = 0; i < nTracePoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps + 1);

			for (int j = 1; j < end; j++)
			{
				int from = pSrc[i - j / c_supersamples];
				int to = pSrc[i - j / c_supersamples - 1];

				int sample = (to - from) * (j%c_supersamples) / c_supersamples + from;

				int topCover = sample - brush[j];
				int bottomCover = sample + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints - i, brushSteps + 1);

			for (int j = 1; j < end; j++)
			{
				int from = pSrc[i + j / c_supersamples];
				int to = pSrc[i + j / c_supersamples + 1];

				int sample = (to - from) * (j%c_supersamples) / c_supersamples + from;

				int topCover = sample - brush[j];
				int bottomCover = sample + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			*pDest++ = top;
			*pDest++ = bottom;
		}

		// Fill...

		if (nFillPoints)
		{
			int top = pWave->hold - brush[0];
			int bottom = pWave->hold + brush[0];
			for (int i = 0; i < nFillPoints; i++)
			{
				*pDest++ = top;
				*pDest++ = bottom;
			}
		}
	}

} // namespace wg
