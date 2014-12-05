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
#ifndef WG_SOFTSURFACE_DOT_H
#define WG_SOFTSURFACE_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif


#include <vector>

class WgSoftSurface;
typedef	WgStrongPtr<WgSoftSurface,WgSurfacePtr>		WgSoftSurfacePtr;
typedef	WgWeakPtr<WgSoftSurface,WgSurfaceWeakPtr>	WgSoftSurfaceWeakPtr;

//____ Class WgSoftSurface _____________________________________________________________________

class WgSoftSurface : public WgSurface
{
	friend class WgSoftGfxDevice;
	friend class WgSoftSurfaceFactory;

 public:
	static WgSoftSurfacePtr	Create( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 ) { return WgSoftSurfacePtr(new WgSoftSurface(size,type)); }
	static WgSoftSurfacePtr	Create( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, const WgObjectPtr& pBlob ) { return WgSoftSurfacePtr(new WgSoftSurface(size,type,pPixels,pitch,pBlob)); }
	static WgSoftSurfacePtr	Create( const WgSoftSurfacePtr& pOther ) { return WgSoftSurfacePtr(new WgSoftSurface( pOther.RawPtr() )); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSoftSurfacePtr	Cast( const WgObjectPtr& pObject );

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		Pixel( WgCoord coord ) const;
	Uint8		Alpha( WgCoord coord ) const;

	void *		Lock( WgAccessMode mode );
	void *		LockRegion( WgAccessMode mode, const WgRect& region );
	void		Unlock();

	inline float ScaleAlpha() { return m_fScaleAlpha; }
	void SetScaleAlpha(float fScaleAlpha);

	void PutPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<Uint32> &col, int length, bool replace);

protected:
	WgSoftSurface( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 );
	WgSoftSurface( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, const WgObjectPtr& pFinalizer );
	WgSoftSurface( const WgSoftSurface * pOther );
	virtual ~WgSoftSurface();

	void _copy(const WgSoftSurface * pOther);

	WgObjectPtr	m_pFinalizer;
	WgSize		m_size;
	float    	m_fScaleAlpha;
	bool		m_bOwnsData;
	Uint8 *		m_pData;
};


//========================================================================================
#endif // WG_SOFTSURFACE_DOT_H
