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
#include <wg_cgfxoutstream.h>

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

		static StreamSurface_p	create( CGfxOutStream& stream, SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( CGfxOutStream& stream, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( CGfxOutStream& stream, SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = SurfaceFlag::Static, const Color * pClut = nullptr );
		static StreamSurface_p	create( CGfxOutStream& stream, Surface * pOther, int flags = SurfaceFlag::Static );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode) override;
		bool		isOpaque() const override;

		//.____ Content _______________________________________________________

		uint8_t		alpha(CoordI coord) override;

		//.____ Control _______________________________________________________

		const PixelBuffer	allocPixelBuffer(const RectI& rect) override;
		bool				pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				freePixelBuffer(const PixelBuffer& buffer) override;

		//.____  Rendering ____________________________________________________

		bool		fill(Color col) override;
		bool		fill(Color col, const RectI& region) override;
		bool		copyFrom(Surface * pSrcSurf, const RectI& srcRect, CoordI dst) override;
		bool		copyFrom(Surface * pSrcSurf, CoordI dst) override;


		//.____ Misc __________________________________________________________


	private:
		StreamSurface( CGfxOutStream& stream, SizeI size, PixelFormat format, int flags, const Color * pClut );
		StreamSurface( CGfxOutStream& stream, SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut );
		StreamSurface( CGfxOutStream& stream, SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut );
		StreamSurface( CGfxOutStream& stream, Surface * pOther, int flags = SurfaceFlag::Static );
		~StreamSurface();

		short		_sendCreateSurface(SizeI size, PixelFormat format, int flags, const Color * pClut);
		void		_sendPixels(RectI rect, const uint8_t * pSource, int pitch);
		void		_sendDeleteSurface();
		uint8_t*	_genAlphaLayer(const char * pSource, int pitch);

		CGfxOutStream_p	m_pStream;
		short			m_inStreamId;		// Id of this surface in the stream.

		Blob_p			m_pBlob;
		uint8_t*		m_pAlphaLayer;		// Separate alpha layer if whole blob was not kept.
		int				m_pitch;
	};
} // namespace wg
#endif //WG_STREAMSURFACE_DOT_H

