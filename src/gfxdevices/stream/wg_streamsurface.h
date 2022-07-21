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
#include <wg_gfxstreamencoder.h>

namespace wg
{

	class StreamSurface;
	typedef	StrongPtr<StreamSurface>	StreamSurface_p;
	typedef	WeakPtr<StreamSurface>	    StreamSurface_wp;

	//____ StreamSurface _____________________________________________________________

	class StreamSurface : public Surface
	{
		friend class StreamSurfaceFactory;
		friend class StreamGfxDevice;

	public:

		//.____ Creation __________________________________________

		static StreamSurface_p	create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint);
		static StreamSurface_p	create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Blob* pBlob, int pitch = 0);
		static StreamSurface_p	create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, uint8_t* pPixels, int pitch = 0, const PixelDescription* pPixelDescription = nullptr);
		static StreamSurface_p	create(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Surface* pOther);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

        //.____ Content _______________________________________________________

		int			alpha(CoordSPX coord) override;

		//.____ Control _______________________________________________________

		const PixelBuffer	allocPixelBuffer(const RectI& rect) override;
		bool				pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				freePixelBuffer(const PixelBuffer& buffer) override;

		//.____  Rendering ____________________________________________________

		bool		fill(HiColor col) override;
		bool		fill(const RectI& region, HiColor col ) override;
//		bool		copyFrom(Surface * pSrcSurf, const RectI& srcRect, CoordI dst) override;
//		bool		copyFrom(Surface * pSrcSurf, CoordI dst) override;

		//.____ Deprecated ____________________________________________________

		static StreamSurface_p	create(GfxStreamEncoder* pEncoder, SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color8* pClut = nullptr);
		static StreamSurface_p	create(GfxStreamEncoder* pEncoder, SizeI size, PixelFormat format, Blob* pBlob, int pitch, int flags = SurfaceFlag::Static, const Color8* pClut = nullptr);
		static StreamSurface_p	create(GfxStreamEncoder* pEncoder, SizeI size, PixelFormat format, uint8_t* pPixels, int pitch, const PixelDescription* pPixelDescription = nullptr, int flags = SurfaceFlag::Static, const Color8* pClut = nullptr);
		static StreamSurface_p	create(GfxStreamEncoder* pEncoder, Surface* pOther, int flags = SurfaceFlag::Static);

		//.____ Misc __________________________________________________________

		bool		streamAsNew(GfxStreamEncoder* pEncoder);

	private:
		StreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& blueprint);
		StreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Blob* pBlob, int pitch = 0);
		StreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, uint8_t* pPixels, int pitch = 0, const PixelDescription* pPixelDescription = nullptr);
		StreamSurface(GfxStreamEncoder * pEncoder, const Blueprint& blueprint, Surface* pOther);

		~StreamSurface();

		uint16_t	_sendCreateSurface(const Blueprint& bp);
		void		_sendPixels(GfxStreamEncoder* pEncoder, RectI rect, const uint8_t * pSource, int pitch);
		void		_sendDeleteSurface();
		uint8_t*	_genAlphaLayer(const char * pSource, int pitch);

		GfxStreamEncoder_p	m_pEncoder;
		uint16_t		m_inStreamId;		// Id of this surface in the stream.
		bool			m_bDynamic;

		Blob_p			m_pBlob;
		uint8_t*		m_pAlphaLayer;		// Separate alpha layer if whole blob was not kept.
		int				m_pitch;
	};
} // namespace wg
#endif //WG_STREAMSURFACE_DOT_H

