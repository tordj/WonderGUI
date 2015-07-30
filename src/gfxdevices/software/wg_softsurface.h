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
typedef	WgStrongPtr<WgSoftSurface,WgSurface_p>		WgSoftSurface_p;
typedef	WgWeakPtr<WgSoftSurface,WgSurface_wp>	WgSoftSurface_wp;

//____ Class WgSoftSurface _____________________________________________________________________

class WgSoftSurface : public WgSurface
{
	friend class WgSoftGfxDevice;
	friend class WgSoftSurfaceFactory;

 public:
	static WgSoftSurface_p	create( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 ) { return WgSoftSurface_p(new WgSoftSurface(size,type)); }
	static WgSoftSurface_p	create( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, const WgObject_p& pBlob ) { return WgSoftSurface_p(new WgSoftSurface(size,type,pPixels,pitch,pBlob)); }
	static WgSoftSurface_p	create( const WgSoftSurface_p& pOther ) { return WgSoftSurface_p(new WgSoftSurface( pOther.rawPtr() )); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgSoftSurface_p	cast( const WgObject_p& pObject );

	WgSize		size() const;
	bool		isOpaque() const;

	Uint32		pixel( WgCoord coord ) const;
	Uint8		alpha( WgCoord coord ) const;

	void *		lock( WgAccessMode mode );
	void *		lockRegion( WgAccessMode mode, const WgRect& region );
	void		unlock();

	inline float scaleAlpha() { return m_fScaleAlpha; }
	void setScaleAlpha(float fScaleAlpha);

	void putPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<Uint32> &col, int length, bool replace);

protected:
	WgSoftSurface( WgSize size, WgPixelType type = WG_PIXEL_ARGB_8 );
	WgSoftSurface( WgSize size, WgPixelType type, Uint8 * pPixels, int pitch, const WgObject_p& pFinalizer );
	WgSoftSurface( const WgSoftSurface * pOther );
	virtual ~WgSoftSurface();

	void _copy(const WgSoftSurface * pOther);

	WgObject_p	m_pFinalizer;
	WgSize		m_size;
	float    	m_fScaleAlpha;
	bool		m_bOwnsData;
	Uint8 *		m_pData;
};


//========================================================================================
#endif // WG_SOFTSURFACE_DOT_H
