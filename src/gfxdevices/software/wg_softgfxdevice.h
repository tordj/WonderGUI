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
#ifndef WG_SOFTGFXDEVICE_DOT_H
#define WG_SOFTGFXDEVICE_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_gfxdevice.h>
#include <wg_softsurface.h>

namespace wg
{


	struct SegmentEdge
	{
		int			begin;				// Pixeloffset, 24.8 format.
		int			end;				// Pixeloffset, 24.8 format. First pixel after edge (segment after the edge has 100% coverage)
		int			coverage;			// 0-65536
		int			coverageInc;		// Increment of coverage for each full pixel we progress
	};


	class SoftGfxDevice;
	typedef	StrongPtr<SoftGfxDevice>	SoftGfxDevice_p;
	typedef	WeakPtr<SoftGfxDevice>	SoftGfxDevice_wp;

	class SoftGfxDevice : public GfxDevice
	{
		friend class SoftSurface;

	public:

		//.____ Creation __________________________________________

		static SoftGfxDevice_p	create();

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&		surfaceType(void) const override;

		//.____ Misc _______________________________________________________

		bool		defineCanvas( CanvasRef ref, SoftSurface * pSurface );

		using GfxDevice::canvas;
		const CanvasInfo& canvas(CanvasRef ref) const override;

		SurfaceFactory_p	surfaceFactory() override;

		//.____ State _________________________________________________

		void		setTintColor(HiColor color) override;

		void		setTintGradient(const RectSPX& rect, const Gradient& gradient) override;
		void		clearTintGradient() override;

		bool		setBlendMode(BlendMode blendMode) override;
		void		setMorphFactor(float factor) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

		//

		using 	GfxDevice::fill;
		void	fill(const RectSPX& rect, HiColor col) override;
		void	fill(const RectF& rect, HiColor col) override;

		void    plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors) override;

		void	drawLine(CoordSPX begin, CoordSPX end, HiColor color, float thickness = 1.f) override;
		void	drawLine(CoordSPX begin, Direction dir, spx length, HiColor col, float thickness = 1.f) override;

		bool	setBlitSource(Surface * pSource) override;
		void	rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f,0.5f }, CoordF destCenter = { 0.5f,0.5f } ) override;

		void	tile(const RectSPX& dest, CoordSPX shift = { 0,0 }) override;
		void	flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift = { 0,0 }) override;

		void	scaleTile(const RectSPX& dest, float scale, CoordSPX shift = { 0,0 }) override;
		void	scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift = { 0,0 }) override;


	protected:
		SoftGfxDevice();
		~SoftGfxDevice();

		void	_canvasWasChanged() override;
		void	_renderLayerWasChanged() override;


		void	_transformBlit(const RectSPX& dest, CoordSPX src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectSPX& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) override;

		const static TintMode      TintMode_min = TintMode::None;
		const static TintMode      TintMode_max = TintMode::GradientXY;
		const static int           TintMode_size = (int)TintMode::GradientXY + 1;

		enum EdgeOp
		{
			None,
			Clip,
			Tile
		};


		struct ColTrans
		{
			TintMode mode;

			HiColor	flatTintColor;	// Used in TintMode::Color

			// Following used in TintMode GradientX, GradientY and GradientXY.

			RectI	tintRect;

			int32_t	topLeftR;		// Scale: 0 -> (1<<18)
			int32_t	topLeftG;
			int32_t	topLeftB;
			int32_t	topLeftA;

			int32_t	leftIncR;
			int32_t	leftIncG;
			int32_t	leftIncB;
			int32_t	leftIncA;

			int32_t	topRightR;
			int32_t	topRightG;
			int32_t	topRightB;
			int32_t	topRightA;

			int32_t	rightIncR;
			int32_t	rightIncG;
			int32_t	rightIncB;
			int32_t	rightIncA;

			int			morphFactor;	// Scale: 0 -> 4096
		};

		struct SegmentGradient
		{
			int			begR;		// Blue value at top of segment column. 0-255 << 16;
			int			begG;
			int			begB;
			int			begA;

			int			incR;		// Increment of begR for each row.
			int			incG;
			int			incB;
			int			incA;
		};


		struct Pitches
		{
			int srcX;			// Pitch in bytes for each pixel read.
			int srcY;			// Pitch in bytes from end of line to beginning of next for each line read.
			int dstX;			// Pitch in bytes for each pixel written.
			int dstY;			// Pitch in bytes from end of line to beginning of next for each line written.
		};

		inline static void _read_pixel_fast8(const uint8_t* pPixel, PixelFormat format, const Color8* pClut, const HiColor* pClut4096, int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA);
		inline static void _write_pixel_fast8(uint8_t* pPixel, PixelFormat format, int16_t b, int16_t g, int16_t r, int16_t a);

		inline static void	_blend_pixels_fast8(BlendMode mode, int morphFactor, PixelFormat destFormat,
			int16_t srcB, int16_t srcG, int16_t srcR, int16_t srcA,
			int16_t backB, int16_t backG, int16_t backR, int16_t backA,
			int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA);


		inline static void _read_pixel(const uint8_t * pPixel, PixelFormat format, const Color8 * pClut, const HiColor* pClut4096, int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA);
		inline static void _write_pixel(uint8_t * pPixel, PixelFormat format, int16_t b, int16_t g, int16_t r, int16_t a);

		inline static void	_blend_pixels(	BlendMode mode, int morphFactor, PixelFormat destFormat,
											int16_t srcB, int16_t srcG, int16_t srcR, int16_t srcA,
											int16_t backB, int16_t backG, int16_t backR, int16_t backA,
											int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA);

		inline static void	_color_tint_init(	TintMode tintMode, const ColTrans& tint, int bits, int16_t inB, int16_t inG, int16_t inR, int16_t inA,
												int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
												uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
												uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA, 
												uint32_t& leftIncB, uint32_t& leftIncG, uint32_t& leftIncR, uint32_t& leftIncA,
												uint32_t& rightIncB, uint32_t& rightIncG, uint32_t& rightIncR, uint32_t& rightIncA,
												uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA, CoordI patchOfs);
			
		inline static void _color_tint_line(	TintMode tintMode, const ColTrans& tint, int bits, int16_t inB, int16_t inG, int16_t inR, int16_t inA,
												int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
												uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
												uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
												uint32_t& leftIncB, uint32_t& leftIncG, uint32_t& leftIncR, uint32_t& leftIncA,
												uint32_t& rightIncB, uint32_t& rightIncG, uint32_t& rightIncR, uint32_t& rightIncA,
												uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
												uint32_t& pixelB, uint32_t& pixelG, uint32_t& pixelR, uint32_t& pixelA, CoordI patchOfs);

		inline static void _color_tint_pixel(TintMode tintMode, int bits,
											int16_t& outB, int16_t& outG, int16_t& outR, int16_t& outA,
											uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
											uint32_t& pixelB, uint32_t& pixelG, uint32_t& pixelR, uint32_t& pixelA);

		inline static void _texel_tint_init(TintMode tintMode, const ColTrans& tint, int bits,
											uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
											uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
											uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
											uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA, CoordI patchOfs);

		inline static void _texel_tint_line(TintMode tintMode, const ColTrans& tint, int bits,
											uint32_t& leftB, uint32_t& leftG, uint32_t& leftR, uint32_t& leftA,
											uint32_t& rightB, uint32_t& rightG, uint32_t& rightR, uint32_t& rightA,
											uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
											uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA, CoordI patchOfs);

		inline static void _texel_tint_pixel(TintMode tintMode, int bits, int16_t& pixelB, int16_t& pixelG, int16_t& pixelR, int16_t& pixelA,
											uint32_t& xIncB, uint32_t& xIncG, uint32_t& xIncR, uint32_t& xIncA,
											uint32_t& tintB, uint32_t& tintG, uint32_t& tintR, uint32_t& tintA);

		inline static int _segment_alpha(bool GRADIENT, int offset, const int16_t * pSegmentColor, const SegmentGradient * pSegmentGradient);
		inline static void _add_segment_color(bool GRADIENT, int blendFraction, int offset, const int16_t * pSegmentColor, const SegmentGradient * pSegmentGradient, int& accB, int& accG, int& accR, int& accA);


/*
		inline static void _init_tint_color(TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
											uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		inline static void _init_tint_color_line(int lineNb, TintMode tintMode, const ColTrans& tint,
											uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		inline static void _tint_color(int columnNb, TintMode tintMode, const ColTrans& tint,
											uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
											uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		inline static void _init_tint_texel(TintMode tintMode, const ColTrans& tint, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		inline static void _init_tint_texel_line(int lineNb, TintMode tintMode, const ColTrans& tint, uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
											uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		inline static void _tint_texel(int columnNb, TintMode tintMode, const ColTrans& tint, uint8_t texB, uint8_t texG, uint8_t texR, uint8_t texA,
											uint8_t inB, uint8_t inG, uint8_t inR, uint8_t inA,
											uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA );
*/

		template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
		static void _plot(uint8_t * pDst, HiColor col, const ColTrans& tint, CoordI patchOfs);

		template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
		static void _plot_list(const RectI& clip, int nCoords, const CoordI * pCoords, const HiColor * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint);

		template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
		static void _draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const ColTrans& tint, CoordI patchPos);

		template<BlendMode BLEND, TintMode TINT, PixelFormat DSTFORMAT>
		static void _clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const ColTrans& tint, CoordI patchPos);

		template<TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void _fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, HiColor col, const ColTrans& tint, CoordI patchPos);

		template<PixelFormat SRCFORMAT, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT, bool TILE>
		static void	_simple_blit(const uint8_t * pSrc, uint8_t * pDst, const SoftSurface * pSrcSurf, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2]);

		template<PixelFormat SRCFORMAT, SampleMethod SAMPLEMETHOD, TintMode TINT, BlendMode BLEND, PixelFormat DSTFORMAT, EdgeOp EDGEOP>
		static void _complex_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint, CoordI patchPos);

		template<bool GRADIENT, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void	_draw_segment_strip(int clipBeg, int clipEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, const int16_t * pSegmentColors, const SegmentGradient * pSegmentGradients, const bool * pTransparentSegments, const bool* pOpaqueSegments, int morphFactor);

		void	_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch);

		void	_initTables();
		void	_updateBlitFunctions();
		int 	_scaleLineThickness(float thickness, int slope);

		void	_updateTintSettings();

		void	_colTransRect(int baseB[4], int baseG[4], int baseR[4], int baseA[], const RectI& _dest);

		//

		typedef	void(*PlotOp_p)(uint8_t * pDst, HiColor col, const ColTrans& tint, CoordI patchPos);
		typedef	void(*PlotListOp_p)(const RectI& clip, int nCoords, const CoordI * pCoords, const HiColor * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint);
		typedef	void(*LineOp_p)(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const ColTrans& tint, CoordI patchPos);
		typedef	void(*ClipLineOp_p)(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, HiColor color, const ColTrans& tint, CoordI patchPos);
		typedef	void(*FillOp_p)(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, HiColor col, const ColTrans& tint, CoordI patchPos);
		typedef	void(*SimpleBlitOp_p)(const uint8_t * pSrc, uint8_t * pDst, const SoftSurface * pSrcSurf, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint, CoordI patchPos, const int simpleTransform[2][2]);
		typedef	void(*ComplexBlitOp_p)(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const ColTrans& tint, CoordI patchPos);
		typedef void(*SegmentOp_p)(int clipBeg, int clipEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, const int16_t * pSegmentColors, const SegmentGradient * pSegmentGradients, const bool * pTransparentSegments, const bool* pOpaqueSegments, int morphFactor);


		typedef void(SoftGfxDevice::*SimpleBlitProxy_Op)(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos);
		typedef void(SoftGfxDevice::*ComplexBlitProxy_Op)(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool bClipSource);


		void	_onePassSimpleBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos);
		void	_twoPassSimpleBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos);

		void	_onePassComplexBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool bClipSource);
		void	_twoPassComplexBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool bClipSource);

		void	_dummySimpleBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos);
		void	_dummyComplexBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool bClipSource);

		//


		static PlotOp_p			s_plotOpTab[BlendMode_size][PixelFormat_size];
		static LineOp_p			s_LineOpTab[BlendMode_size][PixelFormat_size];
		static ClipLineOp_p		s_clipLineOpTab[BlendMode_size][PixelFormat_size];
		static FillOp_p			s_fillOpTab[TintMode_size][BlendMode_size][PixelFormat_size];		//[BlendMode][TintMode][DestFormat]
		static PlotListOp_p		s_plotListOpTab[BlendMode_size][PixelFormat_size];
		static SegmentOp_p		s_segmentOpTab[2][BlendMode_size][PixelFormat_size];				//[bVerticalTint][BlendMode][DestFormat]

		static SimpleBlitOp_p	s_pass2OpTab[TintMode_size][BlendMode_size][PixelFormat_size];
		static SimpleBlitOp_p	s_pass2OpTab_fast8[TintMode_size][BlendMode_size][PixelFormat_size];

		static SimpleBlitOp_p	s_moveTo_internal_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]

		static SimpleBlitOp_p	s_moveTo_BGRA_8_linear_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_moveTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_moveTo_BGR_8_linear_OpTab[PixelFormat_size][2];								// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_moveTo_BGR_8_sRGB_OpTab[PixelFormat_size][2];								// [SourceFormat][Normal/Tile]

		static SimpleBlitOp_p	s_blendTo_BGRA_8_linear_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_blendTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_blendTo_BGR_8_linear_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]
		static SimpleBlitOp_p	s_blendTo_BGR_8_sRGB_OpTab[PixelFormat_size][2];							// [SourceFormat][Normal/Tile]

		static ComplexBlitOp_p	s_transformTo_internal_OpTab[PixelFormat_size][2][3];				// [SourceFormat][SampleMethod][Normal/Clip/Tile]

		static ComplexBlitOp_p	s_transformTo_BGRA_8_linear_OpTab[PixelFormat_size][2][3];					// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2][3];					// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformTo_BGR_8_linear_OpTab[PixelFormat_size][2][3];					// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformTo_BGR_8_sRGB_OpTab[PixelFormat_size][2][3];					// [SourceFormat][SampleMethod][Normal/Clip/Tile]

		static ComplexBlitOp_p	s_transformBlendTo_BGRA_8_linear_OpTab[PixelFormat_size][2][3];			// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformBlendTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2][3];			// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformBlendTo_BGR_8_linear_OpTab[PixelFormat_size][2][3];				// [SourceFormat][SampleMethod][Normal/Clip/Tile]
		static ComplexBlitOp_p	s_transformBlendTo_BGR_8_sRGB_OpTab[PixelFormat_size][2][3];				// [SourceFormat][SampleMethod][Normal/Clip/Tile]


		static int			s_mulTab[256];

		static int16_t		s_limit4096Tab[4097 * 3];


		SurfaceFactory_p	m_pSurfaceFactory;


		// Members controlling render states

		ColTrans			m_colTrans = { TintMode::None, HiColor(4096,4096,4096,4096), {0,0,0,0} };		// Color transformation data

		bool				m_bTintOpaque = true;						// Set if tint alpha is 255 after combining tintColor and gradient.

		SoftSurface_p		m_pBlitSource				= nullptr;		// Source surface for blits.
		bool				m_bClipSource				= false;		// Set if complex transformBlit might need to do clipping.

		SimpleBlitProxy_Op	m_pSimpleBlitOp				= nullptr;		// Function called to perform a simple blit.
		ComplexBlitProxy_Op m_pComplexBlitOp			= nullptr;		// Function called to perform a complex blit.

		// These are called by SimpleBlitProxy_Op and ComplexBlitProxy_Op.

		SimpleBlitOp_p		m_pSimpleBlitOnePassOp			= nullptr;
		SimpleBlitOp_p		m_pSimpleTileBlitOnePassOp		= nullptr;
		ComplexBlitOp_p		m_pComplexBlitOnePassOp			= nullptr;
		ComplexBlitOp_p		m_pComplexTileBlitOnePassOp		= nullptr;
		ComplexBlitOp_p		m_pComplexClipBlitOnePassOp		= nullptr;

		SimpleBlitOp_p		m_pSimpleBlitFirstPassOp		= nullptr;
		SimpleBlitOp_p		m_pSimpleTileBlitFirstPassOp	= nullptr;
		ComplexBlitOp_p		m_pComplexBlitFirstPassOp		= nullptr;
		ComplexBlitOp_p		m_pComplexTileBlitFirstPassOp	= nullptr;
		ComplexBlitOp_p		m_pComplexClipBlitFirstPassOp	= nullptr;
		SimpleBlitOp_p		m_pBlitSecondPassOp				= nullptr;		// Second pass is same for simple and complex blits (always a simple blit).

		//
		
		std::vector<CanvasInfo>	m_definedCanvases;

		int				m_lineThicknessTable[17];

		Surface_p		m_pRenderLayerSurface = nullptr;	// render layer surface
		PixelBuffer		m_canvasPixelBuffer;
		uint8_t *		m_pCanvasPixels;	// Pixels of render layer surface
		int				m_canvasPixelBits;	// PixelBits of render layer surface
		int				m_canvasPitch;
	};


} // namespace wg
#endif //WG_SOFTGFXDEVICE_DOT_H

