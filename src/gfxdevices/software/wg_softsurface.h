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

namespace wg 
{
	
	class SoftSurface;
	typedef	WgStrongPtr<SoftSurface,Surface_p>		SoftSurface_p;
	typedef	WgWeakPtr<SoftSurface,Surface_wp>	SoftSurface_wp;
	
	//____ Class SoftSurface _____________________________________________________________________
	
	class SoftSurface : public Surface
	{
		friend class SoftGfxDevice;
		friend class SoftSurfaceFactory;
	
	 public:
		static SoftSurface_p	create( Size size, PixelType type = WG_PIXEL_RGBA_8 ) { return SoftSurface_p(new SoftSurface(size,type)); }
		static SoftSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const Object_p& pBlob ) { return SoftSurface_p(new SoftSurface(size,type,pPixels,pitch,pBlob)); }
		static SoftSurface_p	create( const SoftSurface_p& pOther ) { return SoftSurface_p(new SoftSurface( pOther.rawPtr() )); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static SoftSurface_p	cast( const Object_p& pObject );
	
		Size		size() const;
		bool		isOpaque() const;
	
		uint32_t		pixel( Coord coord ) const;
		uint8_t		alpha( Coord coord ) const;
	
		void *		lock( AccessMode mode );
		void *		lockRegion( AccessMode mode, const Rect& region );
		void		unlock();
	
		inline float scaleAlpha() { return m_fScaleAlpha; }
		void setScaleAlpha(float fScaleAlpha);
	
		void putPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<uint32_t> &col, int length, bool replace);
	
	protected:
		SoftSurface( Size size, PixelType type = WG_PIXEL_RGBA_8 );
		SoftSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const Object_p& pFinalizer );
		SoftSurface( const SoftSurface * pOther );
		virtual ~SoftSurface();
	
		void _copy(const SoftSurface * pOther);
	
		Object_p	m_pFinalizer;
		Size		m_size;
		float    	m_fScaleAlpha;
		bool		m_bOwnsData;
		uint8_t *		m_pData;
	};
	
	
	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACE_DOT_H
