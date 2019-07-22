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
#include <wg_gfxoutstream.h>

namespace wg
{

	class StreamSurface;
	typedef	StrongPtr<StreamSurface>	StreamSurface_p;
	typedef	WeakPtr<StreamSurface>	StreamSurface_wp;

	//____ StreamSurface _____________________________________________________________

	class StreamSurface : public Surface
	{
		friend class StreamSurfaceFactory;
		friend class StreamGfxDevice;

	public:

		//.____ Creation __________________________________________

		static StreamSurface_p	create( GfxOutStream& stream, SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( GfxOutStream& stream, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( GfxOutStream& stream, SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( GfxOutStream& stream, Surface * pOther, int flags = SurfaceFlag::Static );

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static StreamSurface_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		SizeI		size() const;
		static SizeI	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode);
		bool		isOpaque() const;

		//.____ Content _______________________________________________________

		uint32_t	pixel(CoordI coord) const;
		uint8_t		alpha(CoordI coord) const;

		//.____ Control _______________________________________________________

		uint8_t *	lock(AccessMode mode);
		uint8_t *	lockRegion(AccessMode mode, const RectI& region);
		void		unlock();

		//.____  Rendering ____________________________________________________

		bool		fill(Color col);
		bool		fill(Color col, const RectI& region);
		bool		copyFrom(Surface * pSrcSurf, const RectI& srcRect, CoordI dst);
		bool		copyFrom(Surface * pSrcSurf, CoordI dst);


		//.____ Misc __________________________________________________________


	private:
		StreamSurface( GfxOutStream& stream, SizeI size, PixelFormat format, int flags, const Color * pClut );
		StreamSurface( GfxOutStream& stream, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut );
		StreamSurface( GfxOutStream& stream, SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut );
		StreamSurface( GfxOutStream& stream, Surface * pOther, int flags = SurfaceFlag::Static );
		~StreamSurface();

		short		_sendCreateSurface(SizeI size, PixelFormat format, int flags, const Color * pClut);
		void		_sendPixels(RectI rect, const uint8_t * pSource, int pitch);
		void		_sendDeleteSurface();
		uint8_t*	_genAlphaLayer(const char * pSource, int pitch);

		GfxOutStream_p	m_pStream;
		short			m_inStreamId;		// Id of this surface in the stream.

		Blob_p			m_pBlob;
		uint8_t*		m_pAlphaLayer;		// Separate alpha layer if whole blob was not kept.

		SizeI			m_size;				// Width and height in pixels.



	};
} // namespace wg
#endif //WG_STREAMSURFACE_DOT_H

