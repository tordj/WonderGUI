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


class WgSoftGfxDevice;
typedef	WgSmartPtr<WgSoftGfxDevice,WgGfxDevicePtr>	WgSoftGfxDevicePtr;
typedef	WgWeakPtr<WgSoftGfxDevice,WgGfxDeviceWeakPtr>	WgSoftGfxDeviceWeakPtr;

class WgSoftGfxDevice : public WgGfxDevice
{
public:
	static WgSoftGfxDevicePtr	Create();
	static WgSoftGfxDevicePtr	Create( const WgSoftSurfacePtr& pCanvas );

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgSoftGfxDevicePtr	Cast( const WgObjectPtr& pObject );

	void	SetCanvas( const WgSoftSurfacePtr& pCanvas );

	//

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurfacePtr& pSrc, const WgRect& srcrect, int dx, int dy  );

	void	DrawArcNE( const WgRect& rect, WgColor color );
	void	DrawElipse( const WgRect& rect, WgColor color );
	void	DrawFilledElipse( const WgRect& rect, WgColor color );

	void	ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color );

	void	ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness );

	void	StretchBlit( const WgSurfacePtr& pSrc, bool bTriLinear = false, float mipmapBias = 0.f );
	void	StretchBlit( const WgSurfacePtr& pSrc, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );
	void	StretchBlit( const WgSurfacePtr& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipmapBias = 0.f );

	void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear = false, float mipBias = 0.f );
	void	ClipStretchBlit( const WgRect& clip, const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias = 0.f);

	void	FillSubPixel( const WgRectF& rect, const WgColor& col );
	void	StretchBlitSubPixel( const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );
	
	void	SetBilinearFiltering( bool bEnable ) { m_bBilinearFiltering = bEnable; }
	bool	GetBilinearFiltering() const { return m_bBilinearFiltering; }

protected:
	WgSoftGfxDevice();
	WgSoftGfxDevice( const WgSoftSurfacePtr& pCanvas );
	~WgSoftGfxDevice();

	void	_initTables();
	void	_genCurveTab();

	void	_drawHorrFadeLine( Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color );
	void	_clipDrawHorrFadeLine( int clipX1, int clipX2, Uint8 * pLineStart, int begOfs, int peakOfs, int endOfs, WgColor color );

	void	_plotAA( int _x, int _y, const WgColor& _col, WgBlendMode blendMode, int _aa );
	void	_drawHorrVertLineAA( int _x, int _y, int _length, const WgColor& _col, WgBlendMode blendMode, int _aa, WgOrientation orientation );
	void	_drawHorrVertLine( int _x, int _y, int _length, const WgColor& _col, WgOrientation orientation );

	void 	_blit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  );
	void 	_tintBlit( const WgSurface* _pSrcSurf, const WgRect& srcrect, int dx, int dy  );

	void	_stretchBlitTintedOpaque(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedBlend32(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void 	_stretchBlitTintedBlend24(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedAdd32(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedAdd24(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedMultiply(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitTintedInvert(	const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );

	void	_stretchBlitOpaque(			const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitBlend32(		const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void 	_stretchBlitBlend24(		const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitAdd32(			const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitAdd24(			const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitMultiply(		const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );
	void	_stretchBlitInvert(			const WgSoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
										int dx, int dy, int dw, int dh );

	bool			m_bBilinearFiltering;
	WgSoftSurfacePtr m_pCanvas;
	Uint8			m_limitTable[512];
	int *			m_pCurveTab;
	Uint8 *			m_pDivTab;
};

#endif //WG_SOFTGFXDEVICE_DOT_H

