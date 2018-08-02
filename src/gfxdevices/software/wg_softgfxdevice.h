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
			int (*setCanvas)( void * pPixels, int pixelDescription, int pitch );
			void (*beginRender)(void );
			void (*endRender)( void );

			void (*fillReplace)( int x, int y, int w, int h, uint32_t color );
			void (*fillBlend)( int x, int y, int w, int h, uint32_t color );
			void (*fillAdd)( int x, int y, int w, int h, uint32_t color );
			void (*fillSubtract)( int x, int y, int w, int h, uint32_t color );
			void (*fillMultiply)( int x, int y, int w, int h, uint32_t color );
			void (*fillInvert)( int x, int y, int w, int h, uint32_t color );

			void (*blitReplace)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitBlend)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitAdd)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitSubtract)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitMultiply)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitInvert)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY );

			void (*tintBlitReplace)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitBlend)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitAdd)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitSubtract)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitMultiply)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitInvert)( void * pSource, int pixelDescription, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
		};	
	
	
		//.____ Creation __________________________________________

		static SoftGfxDevice_p	create();
		static SoftGfxDevice_p	create( Surface * pCanvas );
	
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static SoftGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();
		
		inline CustomFunctionTable *	customFunctions() { return &m_customFunctions; }
		inline void						enableCustomFunctions( bool enable ) { m_bEnableCustomFunctions = enable; };
		inline bool						customFunctionsEnabled() const { return m_bEnableCustomFunctions; }
		
		//.____ Geometry _________________________________________________

		bool	setCanvas( Surface * pCanvas );

		//.____ Rendering ________________________________________________

		bool	beginRender();
		bool	endRender();
		
		//
	
		void	fill( const Rect& rect, const Color& col )  override;
		void	blit( Surface * pSrc, const Rect& srcrect, Coord dest ) override;
	
		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) override;
		void	drawLine(Coord begin, Direction dir, int length, Color col, float thickness = 1.f) override;

		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f ) override;
		void	clipDrawLine(const Rect& clip, Coord begin, Direction dir, int length, Color col, float thickness = 1.f) override;

        void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) override;
        void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) override;
	
		void	fillSubPixel( const RectF& rect, const Color& col ) override;
		void	stretchBlit( Surface * pSrc, const RectF& source, const Rect& dest) override;

		void	clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * PTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill);


		struct ColTrans
		{
			Color	baseTint;
			Color * pTintX;
			Color * pTintY;
		};

		static int		s_mulTab[256];

		
	protected:
		SoftGfxDevice();
		SoftGfxDevice( Surface * pCanvas );
		~SoftGfxDevice();

		struct Pitches
		{
			int srcX;
			int srcY;
			int dstX;
			int dstY;
		};

		inline static void _read_pixel(const uint8_t * pPixel, PixelFormat format, uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);
		inline static void _write_pixel(uint8_t * pPixel, PixelFormat format, uint8_t b, uint8_t g, uint8_t r, uint8_t a);

		inline static void	_blend_pixels(BlendMode mode, uint8_t srcB, uint8_t srcG, uint8_t srcR, uint8_t srcA,
			uint8_t backB, uint8_t backG, uint8_t backR, uint8_t backA,
			uint8_t& outB, uint8_t& outG, uint8_t& outR, uint8_t& outA);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _plot(uint8_t * pDst, Color col, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _plot_list(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _draw_line(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _clip_draw_line(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _fill(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint);

		template<BlendMode BLEND, int TINTFLAGS, PixelFormat DSTFORMAT>
		static void _draw_wave_column(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch);



		template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void	_blit(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint);

		template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void _stretch_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint);

		template<PixelFormat SRCFORMAT, int TINTFLAGS, BlendMode BLEND, PixelFormat DSTFORMAT>
		static void _transform_blit(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const SoftGfxDevice::ColTrans& tint);





		void	_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch);

		void	_drawStraightLine( Coord start, Orientation orientation, int _length, const Color& _col) override;

		void	_initTables();
		void	_clearCustomFunctionTable();
		int 	_scaleLineThickness( float thickness, int slope );
		

//		void	_clipDrawSegmentColumn(int clipBeg, int clipEnd, uint8_t * pColumn, int linePitch, int nEdges, SegmentEdge * pEdges, Color * pSegmentColors);


		void	_drawHorrFadeLine( uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );
		void	_clipDrawHorrFadeLine( int clipX1, int clipX2, uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );

//

		typedef	void(*WaveOp_p)(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch);

		static void	_clip_wave_blend_24(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch);
		static void	_clip_wave_blend_32(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch);

		//

		typedef	void(*PlotListOp_p)(int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);
		typedef	void(*ClipPlotListOp_p)(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);

		static void _plotlist_blend_32(int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);
		static void _plotlist_blend_24(int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);

		static void _clip_plotlist_blend_32(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);
		static void _clip_plotlist_blend_24(const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors, uint8_t * pCanvas, int pitchX, int pitchY);



		//

		typedef	void(*PlotOp_p)(uint8_t * pDst, Color col, const ColTrans& tint);
		typedef	void(*LineOp_p)(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);
		typedef	void(*ClipLineOp_p)(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);

		static void _drawLineSegment_blend_32(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);
		static void _drawLineSegment_blend_24(uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);

		static void _clipDrawLineSegment_blend_32(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);
		static void _clipDrawLineSegment_blend_24(int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color);


		//

		typedef	void(*FillOp_p)(uint8_t * pDst, int pitchX, int pitchY, int nLines, int lineLength, Color col, const ColTrans& tint);
		typedef	void(*BlitOp_p)(const uint8_t * pSrc, uint8_t * pDst, const Pitches& pitches, int nLines, int lineLength, const ColTrans& tint);
		typedef	void(*TransformOp_p)(const SoftSurface * pSrcSurf, CoordF pos, const float matrix[2][2], uint8_t * pDst, int dstPitchX, int dstPitchY, int nLines, int lineLength, const ColTrans& tint);



		void	_onePassStraightBlit(BlitOp_p, const SoftSurface * pSource, const Rect& srcrect, Coord dest, const ColTrans& tint);
		void	_onePassTransformBlit(TransformOp_p, const SoftSurface * pSource, CoordF pos, const float matrix[2][2], const Rect& dest, const ColTrans& tint);

		void	_twoPassStraightBlit(BlitOp_p, BlitOp_p, const SoftSurface * pSource, const Rect& srcrect, Coord dest, const ColTrans& tint);
		void	_twoPassTransformBlit(TransformOp_p, BlitOp_p, const SoftSurface * pSource, CoordF pos, const float matrix[2][2], const Rect& dest, const ColTrans& tint);

		//


		static FillOp_p			s_fillOpTab[BlendMode_size][PixelFormat_size];
		static BlitOp_p			s_pass2OpTab[BlendMode_size][PixelFormat_size];
		static LineOp_p			s_LineOpTab[BlendMode_size][PixelFormat_size];
		static PlotOp_p			s_plotOpTab[BlendMode_size][PixelFormat_size];
		static PlotListOp_p		s_plotListOpTab[BlendMode_size][PixelFormat_size];
		static WaveOp_p			s_waveOpTab[BlendMode_size][PixelFormat_size];


		static ClipLineOp_p		s_clipLineOpTab[BlendMode_size][PixelFormat_size];
		static ClipPlotListOp_p	s_clipPlotListOpTab[BlendMode_size][PixelFormat_size];


		SurfaceFactory_p	m_pSurfaceFactory;

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

