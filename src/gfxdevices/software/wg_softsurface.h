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
#pragma once

#include <wg_surface.h>

#include <vector>

namespace wg 
{
	
	class SoftSurface;
	typedef	StrongPtr<SoftSurface>	SoftSurface_p;
	typedef	WeakPtr<SoftSurface>		SoftSurface_wp;
	
	//____ Class SoftSurface _____________________________________________________________________
	
	class SoftSurface : public Surface
	{
		friend class SoftGfxDevice;
		friend class SoftSurfaceFactory;
	
	 public:

		 //.____ Creation __________________________________________
		 
		 static SoftSurface_p	create( Size size, PixelType type = PixelType::BGRA_8, SurfaceHint hint = SurfaceHint::Static );
        static SoftSurface_p	create( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint = SurfaceHint::Static );
        static SoftSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0, SurfaceHint hint = SurfaceHint::Static );
		static SoftSurface_p	create( Surface * pOther, SurfaceHint hint = SurfaceHint::Static );
	
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static SoftSurface_p	cast( Object * pObject );
	
		//.____ Geometry _________________________________________________

		Size		size() const;
		static Size	maxSize();

		//.____ Appearance ____________________________________________________

		bool		isOpaque() const;
	
		//.____ Content _______________________________________________________

		uint32_t	pixel( Coord coord ) const;
		uint8_t		alpha( Coord coord ) const;
	
		//.____ Control _______________________________________________________

		uint8_t *	lock( AccessMode mode );
		uint8_t *	lockRegion( AccessMode mode, const Rect& region );
		void		unlock();

		//.____ Misc __________________________________________________________

		inline float scaleAlpha() { return m_fScaleAlpha; }
		void		setScaleAlpha(float fScaleAlpha);
	
		//.____  Internal ____________________________________________________

		void putPixels(const std::vector<int> &x, const std::vector<int> &y, const std::vector<uint32_t> &col, int length, bool replace);

	
	protected:
		SoftSurface( Size size, PixelType type = PixelType::BGRA_8 );
		SoftSurface( Size size, PixelType type, Blob * pBlob, int pitch );
		SoftSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat );
		SoftSurface( Surface * pOther );

		virtual ~SoftSurface();
		
		Blob_p		m_pBlob;
		Size		m_size;
		float    	m_fScaleAlpha;
		uint8_t*	m_pData;
	};
	
	
	//========================================================================================

} // namespace wg
#endif // WG_SOFTSURFACE_DOT_H
