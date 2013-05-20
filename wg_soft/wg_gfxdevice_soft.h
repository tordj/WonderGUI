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
#ifndef WG_GFXDEVICE_SOFT_DOT_H
#define WG_GFXDEVICE_SOFT_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif


class WgSurfaceSoft;


class WgGfxDeviceSoft : public WgGfxDevice
{
public:
	WgGfxDeviceSoft();
	WgGfxDeviceSoft( WgSurfaceSoft * pCanvas );
	~WgGfxDeviceSoft();

	void	SetCanvas( WgSurfaceSoft * pCanvas );

	//

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurface* pSrc, const WgRect& srcrect, int dx, int dy  );

	void	DrawArcNE( const WgRect& rect, WgColor color );
	void	DrawElipse( const WgRect& rect, WgColor color );
	void	DrawFilledElipse( const WgRect& rect, WgColor color );

	void	ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color );

	void	ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness );

	void	StretchBlit( const WgSurface * pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
	void	StretchBlit( const WgSurface * pSrc, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	void	StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );

	void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);

	void	FillSubPixel( const WgRectF& rect, const WgColor& col );
	void	StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );
	
	void	SetBilinearFiltering( bool bEnable ) { m_bBilinearFiltering = bEnable; }
	bool	GetBilinearFiltering() const { return m_bBilinearFiltering; }

protected:

	void	_initTables();
	void	_genCurveTab();

	void	_drawHorrFadeLine( Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color );
	void	_clipDrawHorrFadeLine( int clipX1, int clipX2, Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color );

	void	_plotAA( int _x, int _y, const WgColor& _col, WgBlendMode blendMode, int _aa );
	void	_drawHorrVertLineAA( int _x, int _y, int _length, const WgColor& _col, WgBlendMode blendMode, int _aa, WgOrientation orientation );
	void	_drawHorrVertLine( int _x, int _y, int _length, const WgColor& _col, WgOrientation orientation );

	void 	_blit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  );
	void 	_tintBlit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  );

	void	_stretchBlitTintedOpaque(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedBlend32(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void 	_stretchBlitTintedBlend24(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedAdd32(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedAdd24(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedMultiply(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedInvert(	const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );

	void	_stretchBlitOpaque(			const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitBlend32(		const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void 	_stretchBlitBlend24(		const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitAdd32(			const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitAdd24(			const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitMultiply(		const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitInvert(			const WgSurfaceSoft * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );

	bool			m_bBilinearFiltering;
	WgSurfaceSoft * m_pCanvas;
	Uint8			m_limitTable[512];
	int *			m_pCurveTab;
	Uint8 *			m_pDivTab;
};

#endif //WG_GFXDEVICE_SOFT_DOT_H

