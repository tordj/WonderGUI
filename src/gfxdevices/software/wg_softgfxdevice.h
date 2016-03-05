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

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

#ifndef WG_SOFTSURFACE_DOT_H
#	include <wg_softsurface.h>
#endif

namespace wg 
{
	
	
	class SoftGfxDevice;
	typedef	StrongPtr<SoftGfxDevice,GfxDevice_p>	SoftGfxDevice_p;
	typedef	WeakPtr<SoftGfxDevice,GfxDevice_wp>	SoftGfxDevice_wp;
	
	class SoftGfxDevice : public GfxDevice
	{
	public:
		static SoftGfxDevice_p	create();
		static SoftGfxDevice_p	create( const SoftSurface_p& pCanvas );
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static SoftGfxDevice_p	cast( const Object_p& pObject );
	
		void	setCanvas( const SoftSurface_p& pCanvas );
	
		//
	
		void	fill( const Rect& rect, const Color& col );
		void	blit( const Surface_p& pSrc, const Rect& srcrect, int dx, int dy  );
	
		void	drawArcNE( const Rect& rect, Color color );
		void	drawElipse( const Rect& rect, Color color );
		void	drawFilledElipse( const Rect& rect, Color color );
	
		void	clipDrawArcNE( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawElipse( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color );

		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f );
		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f );
	
		void	clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col );
		void	clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col );
		void	clipPlotSoftPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color& col, float thickness );
	
		void	stretchBlit( const Surface_p& pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
		void	stretchBlit( const Surface_p& pSrc, const Rect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
		void	stretchBlit( const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	
		void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, bool bTriLinear = false, float mipBias = 0.f );
		void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& dest, bool bTriLinear = false, float mipBias = 0.f );
		void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear = false, float mipBias = 0.f );
		void	clipStretchBlit( const Rect& clip, const Surface_p& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);
	
		void	fillSubPixel( const RectF& rect, const Color& col );
		void	stretchBlitSubPixel( const Surface_p& pSrc, float sx, float sy, float sw, float sh,
							   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );
		
		void	setBilinearFiltering( bool bEnable ) { m_bBilinearFiltering = bEnable; }
		bool	getBilinearFiltering() const { return m_bBilinearFiltering; }
	
	protected:
		SoftGfxDevice();
		SoftGfxDevice( const SoftSurface_p& pCanvas );
		~SoftGfxDevice();

		const static int c_nCurveTabEntries = 1024;

	
		void	_initTables();
		void	_genCurveTab();
	
		void 	_drawLineSegment( uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color );
		void 	_clipDrawLineSegment( int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color );
	
	
		void	_drawHorrFadeLine( uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );
		void	_clipDrawHorrFadeLine( int clipX1, int clipX2, uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color );
	
		void	_plotAA( int _x, int _y, const Color& _col, BlendMode blendMode, int _aa );
		void	_drawHorrVertLineAA( int _x, int _y, int _length, const Color& _col, BlendMode blendMode, int _aa, Orientation orientation );
		void	_drawHorrVertLine( int _x, int _y, int _length, const Color& _col, Orientation orientation );
	
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
		void	_stretchBlitTintedMultiply(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitTintedInvert(	const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
	
		void	_stretchBlitOpaque(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitBlend32(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void 	_stretchBlitBlend24(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitAdd32(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitAdd24(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitMultiply(		const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
		void	_stretchBlitInvert(			const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
											int dx, int dy, int dw, int dh );
	
		bool			m_bBilinearFiltering;
		SoftSurface_p 	m_pCanvas;
		uint8_t			m_limitTable[512];
		int				m_lineThicknessTable[17];
		int *			m_pCurveTab;
		uint8_t *		m_pDivTab;
	};
	

} // namespace wg
#endif //WG_SOFTGFXDEVICE_DOT_H

