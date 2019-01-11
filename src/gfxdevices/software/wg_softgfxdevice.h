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
	public:

		struct CustomFunctionTable
		{
			int(*setCanvas)(void * pPixels, int pixelDescription, int pitch);
			void(*beginRender)(void);
			void(*endRender)(void);

			void(*fillReplace)(int x, int y, int w, int h, uint32_t color);
			void(*fillBlend)(int x, int y, int w, int h, uint32_t color);
			void(*fillAdd)(int x, int y, int w, int h, uint32_t color);
			void(*fillSubtract)(int x, int y, int w, int h, uint32_t color);
			void(*fillMultiply)(int x, int y, int w, int h, uint32_t color);
			void(*fillInvert)(int x, int y, int w, int h, uint32_t color);

			void(*blitReplace)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);
			void(*blitBlend)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);
			void(*blitAdd)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);
			void(*blitSubtract)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);
			void(*blitMultiply)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);
			void(*blitInvert)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY);

			void(*tintBlitReplace)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
			void(*tintBlitBlend)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
			void(*tintBlitAdd)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
			void(*tintBlitSubtract)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
			void(*tintBlitMultiply)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
			void(*tintBlitInvert)(void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color);
		};


		//.____ Creation __________________________________________

		static SoftGfxDevice_p	create();
		static SoftGfxDevice_p	create(Surface * pCanvas);

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static SoftGfxDevice_p	cast(Object * pObject);
		const char *			surfaceClassName(void) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

		inline CustomFunctionTable *	customFunctions() { return &m_customFunctions; }
		inline void						enableCustomFunctions(bool enable) { m_bEnableCustomFunctions = enable; };
		inline bool						customFunctionsEnabled() const { return m_bEnableCustomFunctions; }

		//.____ Geometry _________________________________________________

		bool	setCanvas(Surface * pCanvas) override;

		//.____ State _________________________________________________

		void		setTintColor(Color color) override;
		bool		setBlendMode(BlendMode blendMode) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

		//

		virtual void	fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches) override;
		virtual void	fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches) override;

		virtual void    plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches) override;

		virtual void	drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches) override;
		virtual void	drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches) override;

/*
		virtual void	fill(const Rect& rect, const Color& col) override;
		virtual void	fill(const RectF& rect, const Color& col) override;

		virtual void    plotPixels(int nCoords, const Coord * pCoords, const Color * pColors) override;

		virtual void	drawLine(Coord begin, Coord end, Color color, float thickness = 1.f) override;
		virtual void	drawLine(Coord begin, Direction dir, int length, Color col, float thickness = 1.f) override;
*/

		virtual bool	setBlitSource(Surface * pSource) override;

		virtual void	transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2]) override;
		virtual void	transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2]) override;

		virtual void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;
		virtual void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches) override;


		virtual void	transformDrawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;

		virtual void	drawSegments(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch) override;


		struct ColTrans
		{
			Color	baseTint;
			Color * pTintX;
			Color * pTintY;  
		};


	protected:
		SoftGfxDevice();
		SoftGfxDevice(Surface * pCanvas);
		~SoftGfxDevice();

		enum class TintMode
		{
			None = 0,
			Color,
			MapX,
			MapY,
			MapXY
		};

		const static TintMode      TintMode_min = TintMode::None;
		const static TintMode      TintMode_max = TintMode::MapXY;
		const static int           TintMode_size = (int)TintMode::MapXY + 1;

		struct Pitches
		{
			int srcX;
			int srcY;
			int dstX;
			int dstY;
		};

		inline static void _read_pixel(const uint8_t * pPixel, PixelFormat format, const Color * pClut, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);
		inline static void _write_pixel(uint8_t * pPixel, PixelFormat format, uint8_t b, uint8_t g, uint8_t r, uint8_t a);

		inline static void	_blend_pixels(BlendMode mode, uint8_t srcB, uint8_t srcG, uint8_t srcR, uint8_t srcA,
											uint8_t backB, uint8_t backG, uint8_t backR, uint8_t backA,
											uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

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



		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _plot(uint8_t * pDst, Color col, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _plot_list(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);

		template<BlendMode BLEND, TintMode TINTMODE, PixelFormat DSTFORMAT>
		static void _fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint);

		template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void	_simple_blit(const uint8_t * pSrc, uint8_t * pDst, const Color * pClut, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint);

		template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void _stretch_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint);

		template<PixelFormat SRCFORMAT, ScaleMode SCALEMODE, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void _complex_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void	_draw_segment_strip(int clipBeg, int clipEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, const Color * pSegmentColors);




		void	_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch);

		void	_initTables();
		void	_updateBlitFunctions();
		void	_clearCustomFunctionTable();
		int 	_scaleLineThickness(float thickness, int slope);

		//

		typedef	void(*PlotOp_p)(uint8_t * pDst, Color col, const ColTrans& tint);
		typedef	void(*PlotListOp_p)(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint);
		typedef	void(*LineOp_p)(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);
		typedef	void(*ClipLineOp_p)(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);
		typedef	void(*FillOp_p)(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint);
		typedef	void(*SimpleBlitOp_p)(const uint8_t * pSrc, uint8_t * pDst, const Color * pClut, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint);
		typedef	void(*ComplexBlitOp_p)(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const ColTrans& tint);
		typedef void(*SegmentOp_p)(int clipBeg, int clipEnd, uint8_t * pStripStart, int pixelPitch, int nEdges, SegmentEdge * pEdges, const Color * pSegmentColors);


		typedef void(SoftGfxDevice::*SimpleBlitProxy_Op)(const Rect& dest, Coord src, const int simpleTransform[2][2]);
		typedef void(SoftGfxDevice::*ComplexBlitProxy_Op)(const Rect& dest, CoordF pos, const float matrix[2][2]);


		void	_onePassSimpleBlit(const Rect& dest, Coord pos, const int simpleTransform[2][2]);
		void	_twoPassSimpleBlit(const Rect& dest, Coord pos, const int simpleTransform[2][2]);

		void	_onePassComplexBlit(const Rect& dest, CoordF pos, const float matrix[2][2]);
		void	_twoPassComplexBlit(const Rect& dest, CoordF pos, const float matrix[2][2]);

		void	_dummySimpleBlit(const Rect& dest, Coord pos, const int simpleTransform[2][2]);
		void	_dummyComplexBlit(const Rect& dest, CoordF pos, const float matrix[2][2]);

		//


		static PlotOp_p			s_plotOpTab[BlendMode_size][PixelFormat_size];
		static LineOp_p			s_LineOpTab[BlendMode_size][PixelFormat_size];
		static ClipLineOp_p		s_clipLineOpTab[BlendMode_size][PixelFormat_size];
		static FillOp_p			s_fillOpTab[BlendMode_size][TintMode_size][PixelFormat_size];		//[BlendMode][TintMode][DestFormat]
		static PlotListOp_p		s_plotListOpTab[BlendMode_size][PixelFormat_size];
		static SegmentOp_p		s_segmentOpTab[BlendMode_size][PixelFormat_size];

		static SimpleBlitOp_p	s_pass2OpTab[BlendMode_size][PixelFormat_size];

		static SimpleBlitOp_p	s_moveTo_BGRA_8_OpTab[PixelFormat_size][2];				// [SourceFormat][TintMode]
		static SimpleBlitOp_p	s_moveTo_BGR_8_OpTab[PixelFormat_size][2];				// [SourceFormat][TintMode]
			
		static SimpleBlitOp_p	s_blendTo_BGRA_8_OpTab[PixelFormat_size][2];			// [SourceFormat][TintMode]
		static SimpleBlitOp_p	s_blendTo_BGR_8_OpTab[PixelFormat_size][2];				// [SourceFormat][TintMode]

		static ComplexBlitOp_p	s_transformTo_BGRA_8_OpTab[PixelFormat_size][2][2];		// [SourceFormat][ScaleMode][TintMode]
		static ComplexBlitOp_p	s_transformTo_BGR_8_OpTab[PixelFormat_size][2][2];		// [SourceFormat][ScaleMode][TintMode]

		static ComplexBlitOp_p	s_transformBlendTo_BGRA_8_OpTab[PixelFormat_size][2][2];	// [SourceFormat][ScaleMode][TintMode]
		static ComplexBlitOp_p	s_transformBlendTo_BGR_8_OpTab[PixelFormat_size][2][2];	// [SourceFormat][ScaleMode][TintMode]


		static int			s_mulTab[256];

		SurfaceFactory_p	m_pSurfaceFactory;


		// Members controlling render states

		ColTrans			m_colTrans = { Color::White, nullptr, nullptr };	// Color transformation data

		SoftSurface_p		m_pBlitSource				= nullptr;		// Source surface for blits.

		SimpleBlitProxy_Op	m_pSimpleBlitOp				= nullptr;		// Function called to perform a simple blit.
		ComplexBlitProxy_Op m_pComplexBlitOp			= nullptr;		// Function called to perform a complex blit.

		// These are called by SimpleBlitProxy_Op and ComplexBlitProxy_Op.
		
		SimpleBlitOp_p		m_pSimpleBlitOnePassOp		= nullptr;
		ComplexBlitOp_p		m_pComplexBlitOnePassOp		= nullptr;
		SimpleBlitOp_p		m_pSimpleBlitFirstPassOp	= nullptr;
		ComplexBlitOp_p		m_pComplexBlitFirstPassOp	= nullptr;
		SimpleBlitOp_p		m_pBlitSecondPassOp			= nullptr;		// Second pass is same for simple and complex blits (always a simple blit).

		//

		int				m_lineThicknessTable[17];
		
		uint8_t *		m_pCanvasPixels;	// Pixels of m_pCanvas when locked 
		int				m_canvasPixelBits;	// PixelBits of m_pCanvas when locked
		int				m_canvasPitch;

		bool			m_bEnableCustomFunctions;	// Externally set.
		bool			m_bUseCustomFunctions;		// Internally set, based on m_bEnableCustomFunctions and return values from custom calls to beginRender and setCanvas.
													//Use overrided drawing primitives if available. 
		CustomFunctionTable m_customFunctions;

	};
	

} // namespace wg
#endif //WG_SOFTGFXDEVICE_DOT_H

