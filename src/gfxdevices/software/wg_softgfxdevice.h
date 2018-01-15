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
#include <wg_softsurfacefactory.h>

namespace wg 
{
	
	
	class SoftGfxDevice;
	typedef	StrongPtr<SoftGfxDevice>	SoftGfxDevice_p;
	typedef	WeakPtr<SoftGfxDevice>	SoftGfxDevice_wp;
	
	class SoftGfxDevice : public GfxDevice
	{
	public:
	
		struct CustomFunctionTable
		{
			int (*setCanvas)( void * pPixels, int pixelFormat, int pitch );
			void (*beginRender)(void );
			void (*endRender)( void );

			void (*fillReplace)( int x, int y, int w, int h, uint32_t color );
			void (*fillBlend)( int x, int y, int w, int h, uint32_t color );
			void (*fillAdd)( int x, int y, int w, int h, uint32_t color );
			void (*fillSubtract)( int x, int y, int w, int h, uint32_t color );
			void (*fillMultiply)( int x, int y, int w, int h, uint32_t color );
			void (*fillInvert)( int x, int y, int w, int h, uint32_t color );

			void (*blitReplace)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitBlend)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitAdd)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitSubtract)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitMultiply)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );
			void (*blitInvert)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY );

			void (*tintBlitReplace)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitBlend)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitAdd)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitSubtract)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitMultiply)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
			void (*tintBlitInvert)( void * pSource, int pixelFormat, int pitch, int x, int y, int w, int h, int destX, int destY, uint32_t color );
		};	
	
	
		//.____ Creation __________________________________________

		static SoftGfxDevice_p	create();
		static SoftGfxDevice_p	create( SoftSurface * pCanvas );
	
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
		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f ) override;
	
        void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) override;
        void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) override;
	
		void	fillSubPixel( const RectF& rect, const Color& col ) override;
		void	stretchBlit( Surface * pSrc, const RectF& source, const Rect& dest) override;

		void	clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * PTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill);

		// Experimental stuff...

		void	drawArcNE( const Rect& rect, Color color );
		void	drawElipse( const Rect& rect, Color color );
		void	drawFilledElipse( const Rect& rect, Color color );
	
		void	clipDrawArcNE( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawElipse( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color );

		
	protected:
		SoftGfxDevice();
		SoftGfxDevice( SoftSurface * pCanvas );
		~SoftGfxDevice();

		void	_drawStraightLine( Coord start, Orientation orientation, int _length, const Color& _col) override;

		void	_initTables();
		void	_clearCustomFunctionTable();
		int 	_scaleLineThickness( float thickness, int slope );
	
		void 	_drawLineSegment( uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color );
		void 	_clipDrawLineSegment( int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color );
	
		void	_clipDrawWaveColumn( int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch);
	
		void	_drawHorrFadeLine( uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );
		void	_clipDrawHorrFadeLine( int clipX1, int clipX2, uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );
	
		void	_plotAA( int _x, int _y, const Color& _col, BlendMode blendMode, int _aa );
		void	_drawStraightLineAA( int _x, int _y, int _length, const Color& _col, BlendMode blendMode, int _aa, Orientation orientation );
	
		void 	_blit( const Surface* _pSrcSurf, const Rect& srcrect, int dx, int dy  );
		void 	_tintBlit( const Surface* _pSrcSurf, const Rect& srcrect, int dx, int dy  );
	
		void	_stretchBlitTintedOpaque(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedBlend32(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void 	_stretchBlitTintedBlend24(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedAdd32(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedAdd24(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedSub32(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedSub24(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedMultiply(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedInvert(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
	
		void	_stretchBlitOpaque(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitBlend32(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
//		void 	_stretchBlitBlend24(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,	// Not used, takes shortcut to _stretchBlitOpaque()
//											int dx, int dy, int dw, int dh );
		void	_stretchBlitAdd32(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitAdd24(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitSub32(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitSub24(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitMultiply(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitInvert(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
	
		SoftSurfaceFactory_p	m_pSurfaceFactory;
		int				m_lineThicknessTable[17];
		uint8_t *		m_pDivTab;
		
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

