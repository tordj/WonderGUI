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
#ifndef WG_LINEOUTGFXDEVICE_DOT_H
#define WG_LINEOUTGFXDEVICE_DOT_H
#pragma once

#include <wg_softgfxdevice.h>

namespace wg
{


	class LinearGfxDevice;
	typedef	StrongPtr<LinearGfxDevice>	LinearGfxDevice_p;
	typedef	WeakPtr<LinearGfxDevice>	LinearGfxDevice_wp;

	class LinearGfxDevice : public SoftGfxDevice
	{
	public:

		struct Segment
		{
			RectI		rect;			// In pixels, not spx!
			uint8_t *	pBuffer;
			int 		pitch;
		};
		
		//.____ Creation __________________________________________

		static LinearGfxDevice_p	create( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
										   std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Misc _______________________________________________________

		bool		defineCanvas( CanvasRef ref, const SizeSPX size, PixelFormat pixelFormat, int scale = 64 );

		using SoftGfxDevice::canvas;
		const CanvasInfo canvas(CanvasRef ref) const override;

		void		setSegmentPadding( int bytes );
		inline int	segmentPadding() const { return m_segmentPadding; }

		//.____ State _________________________________________________

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;
		
		void    endCanvasUpdate() override;

		using 	SoftGfxDevice::fill;
		void	fill(const RectSPX& rect, HiColor col) override;


		void    plotPixels( int nCoords, const CoordSPX * pCoords, const HiColor * pColors) override;

		void	drawLine( CoordSPX begin, CoordSPX end, HiColor color, spx thickness = 64 ) override;
		void	drawLine( CoordSPX begin, Direction dir, spx length, HiColor color, spx thickness = 64 ) override;


		void	stretchBlit(const RectSPX& dest) override;
		void	stretchBlit(const RectSPX& dest, const RectSPX& src) override;

		void	stretchFlipBlit(const RectSPX& dest, GfxFlip flip) override;
		void	stretchFlipBlit(const RectSPX& dest, const RectSPX& src, GfxFlip flip) override;

		void	precisionBlit(const RectSPX& dest, const RectF& srcSPX) override;
		void	transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2]) override;
		void	rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f }) override;

		void	tile(const RectSPX& dest, CoordSPX shift = { 0,0 }) override;
		void	flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift = { 0,0 }) override;

		void	scaleTile(const RectSPX& dest, float scale, CoordSPX shift = { 0,0 }) override;
		void	scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift = { 0,0 }) override;

		void	drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill) override;
		void	flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip) override;

		void	drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness = 0, HiColor outlineColor = HiColor::Black) override;
		void	drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = HiColor::Transparent, HiColor backColor = HiColor::Transparent, bool bRectangular = false) override;

		void	drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat ) override;
		void	flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat) override;

		void	drawWaveform(CoordSPX dest, Waveform * pWaveform ) override;
		void	flipDrawWaveform(CoordSPX dest, Waveform * pWaveform, GfxFlip flip) override;

 		void	blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale) override;



		//.____ Internal _____________________________________________________

		

	protected:
		LinearGfxDevice( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
						 std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender );
		~LinearGfxDevice();

		void	_canvasWasChanged() override;
		void 	_clipListWasChanged() override;
		void	_updateBlitFunctions() override;

		bool 	_beginCanvasUpdate(CanvasRef ref, Surface * pCanvas, int nUpdateRects,
								   const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer) override;
		
		void	_transformBlitSimple(const RectSPX& dest, CoordSPX src, const int simpleTransform[2][2]) override;
		void	_transformBlitComplex(const RectSPX& dest, BinalCoord src, const binalInt complexTransform[2][2]) override;


		
		std::function<void*(CanvasRef ref, int nBytes)> m_beginCanvasRenderCallback;
		std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> m_endCanvasRenderCallback;

		typedef void(LinearGfxDevice::*LinearStraightBlitProxy_Op)(uint8_t * pDst, int destPitch, int width, int height, CoordI src, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp);
		typedef void(LinearGfxDevice::*LinearTransformBlitProxy_Op)(uint8_t * pDst, int destPitch, int width, int height, BinalCoord pos, const binalInt matrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp);
		
		LinearStraightBlitProxy_Op	m_pLinearStraightBlitOp 	= nullptr;		// Function called to perform a straight blit.
		LinearStraightBlitProxy_Op	m_pLinearStraightTileOp 	= nullptr;		// Function called to perform a straight tile.

		LinearTransformBlitProxy_Op m_pLinearTransformBlitOp 	= nullptr;		// Function called to perform a transform blit.
		LinearTransformBlitProxy_Op m_pLinearTransformClipBlitOp= nullptr;	// Function called to perform a transform clip blit.
		LinearTransformBlitProxy_Op m_pLinearTransformTileOp 	= nullptr;		// Function called to perform a transform tile.

		void	_onePassLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI pos, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp);
		void	_twoPassLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI pos, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp);
		void	_dummyLinearStraightBlit(uint8_t * pDst, int destPitch, int width, int height, CoordI pos, const int simpleTransform[2][2], CoordI patchPos, StraightBlitOp_p pPassOneOp);

		void	_onePassLinearTransformBlit(uint8_t * pDst, int destPitch, int width, int height, BinalCoord pos, const binalInt matrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp);
		void	_twoPassLinearTransformBlit(uint8_t * pDst, int destPitch, int width, int height, BinalCoord pos, const binalInt matrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp);
		void	_dummyLinearTransformBlit(uint8_t * pDst, int destPitch, int width, int height, BinalCoord pos, const binalInt matrix[2][2], CoordI patchPos, TransformBlitOp_p pPassOneOp);
		
		
		CanvasInfo					m_canvasDefinitions[CanvasRef_size];

		std::vector<Segment>		m_canvasSegments;
		std::vector<Segment>		m_clipSegments;

		Segment *					m_pClipSegments;
		int							m_nClipSegments;
		
		int							m_canvasPixelBytes;
		int							m_nbSurfaceCanvases = 0;
		
		int							m_segmentPadding = 0;

	};


} // namespace wg
#endif //WG_LINEOUTGFXDEVICE_DOT_H

