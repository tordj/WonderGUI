/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free Ioware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Ioware Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_STREAMSURFACE_DOT_H
#define	WG_STREAMSURFACE_DOT_H
#pragma once


#include <wg_surface.h>

namespace wg
{

	class StreamSurface;
	typedef	StrongPtr<StreamSurface>	StreamSurface_p;
	typedef	WeakPtr<StreamSurface>	StreamSurface_wp;

	//____ StreamSurface _____________________________________________________________

	class StreamSurface : public Surface
	{
		friend class StreamSurfaceFactory;

	public:

		//.____ Creation __________________________________________

        static StreamSurface_p	create( Size size, PixelType type = PixelType::BGRA_8, SurfaceHint hint = SurfaceHint::Static );
        static StreamSurface_p	create( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint = SurfaceHint::Static );
        static StreamSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0, SurfaceHint hint = SurfaceHint::Static );
        static StreamSurface_p	create( Surface * pOther, SurfaceHint hint = SurfaceHint::Static );
       
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static StreamSurface_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		Size		size() const;
		static Size	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode);
		bool		isOpaque() const;

		//.____ Content _______________________________________________________

		uint32_t	pixel(Coord coord) const;
		uint8_t		alpha(Coord coord) const;

		//.____ Control _______________________________________________________

		uint8_t *	lock(AccessMode mode);
		uint8_t *	lockRegion(AccessMode mode, const Rect& region);
		void		unlock();

		//.____ Misc __________________________________________________________


	private:
        StreamSurface( Size size, PixelType type = PixelType::BGRA_8, SurfaceHint hint = SurfaceHint::Static );
        StreamSurface( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint = SurfaceHint::Static );
        StreamSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, SurfaceHint hint = SurfaceHint::Static );
        StreamSurface( Surface * pOther, SurfaceHint hint = SurfaceHint::Static );
		~StreamSurface();



        Blob_p      m_pBlob;
		Size		m_size;				// Width and height in pixels.
		uint32_t	m_pixelSize;		// Size in bytes of a pixel.

	};
} // namespace wg
#endif //WG_STREAMSURFACE_DOT_H

