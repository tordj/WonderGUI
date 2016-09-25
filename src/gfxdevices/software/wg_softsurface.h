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
	typedef	StrongPtr<SoftSurface,Surface_p>	SoftSurface_p;
	typedef	WeakPtr<SoftSurface,Surface_wp>		SoftSurface_wp;
	
	//____ Class SoftSurface _____________________________________________________________________
	
	class SoftSurface : public Surface
	{
		friend class SoftGfxDevice;
		friend class SoftSurfaceFactory;
	
	 public:
		static SoftSurface_p	create( Size size, PixelType type = PixelType::BGRA_8 );
        static SoftSurface_p	create( Size size, PixelType type, const Blob_p& pBlob, int pitch );
        static SoftSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0 );
		static SoftSurface_p	create( const Surface_p& pOther );
	
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
		SoftSurface( Size size, PixelType type = PixelType::BGRA_8 );
		SoftSurface( Size size, PixelType type, const Blob_p& pBlob, int pitch );
		SoftSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat );
		SoftSurface( const Surface_p& pOther );

		virtual ~SoftSurface();
		
		Blob_p		m_pBlob;
		Size		m_size;
		float    	m_fScaleAlpha;
		uint8_t*	m_pData;
	};
	
	
	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACE_DOT_H
