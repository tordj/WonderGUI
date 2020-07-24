/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free Glware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Glware Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_METALSURFACE_DOT_H
#define	WG_METALSURFACE_DOT_H
#pragma once

#import "simd/simd.h"
#import "MetalKit/MetalKit.h"

#include <wg_surface.h>

namespace wg
{

	class MetalSurface;
	typedef	StrongPtr<MetalSurface>	MetalSurface_p;
	typedef	WeakPtr<MetalSurface>	MetalSurface_wp;

	//____ MetalSurface _____________________________________________________________

	class MetalSurface : public Surface
	{
        
        friend class MetalSurfaceFactory;
		friend class MetalGfxDevice;

	public:
		//.____ Creation __________________________________________

		static MetalSurface_p	create( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static MetalSurface_p	create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static MetalSurface_p	create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static MetalSurface_p	create( Surface * pOther, int flags = SurfaceFlag::Static );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode) override;
		bool		isOpaque() const override;

		void		setTiling(bool bTiling) override;

		//.____ Content _______________________________________________________

		uint32_t	pixel(CoordI coord) const override;
		uint8_t		alpha(CoordI coord) const override;

		//.____ Control _______________________________________________________

		uint8_t *	lock(AccessMode mode) override;
		uint8_t *	lockRegion(AccessMode mode, const RectI& region) override;
		void		unlock() override;

		bool		unload();
		bool		isLoaded();
		void		reload();


		//.____ Misc __________________________________________________________

		inline	id<MTLTexture>	getTexture() const { return m_texture; }
		inline	id<MTLTexture>	getClutTexture() const { return m_clutTexture; }

	private:
		MetalSurface( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		MetalSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		MetalSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		MetalSurface( Surface * pOther, int flags = SurfaceFlag::Static );
		~MetalSurface();


		void		    _setPixelDetails( PixelFormat format );
        void            _setupMetalTexture(void * pPixels, int pitch, const PixelDescription * pPixelDescription, const Color * pClut );

        void            _syncBuffer();
        void            _syncTexture( RectI region );
        void            _waitForSyncToComplete();
                
		int			    m_bPendingReads = false;					// Set if there are queued commands that will use surface as source. Active MetalGfxDevice needs to be flushed before we modify.
		bool            m_bMipmapStale = false;

        bool            m_bBufferNeedsSync = false;                 // Texture has changes that have not been synced to buffer yet.
        bool            m_bTextureNeedsSync = false;                // Buffer has changes that have not been synced to texture yet.
        bool            m_bBufferSyncInProgress = false;            // Buffer is being updated with changes from texture.
        bool            m_bTextureSyncInProgress = false;           // Texture is being updated with changes from buffer.


		id<MTLTexture>	m_clutTexture = nil;	    // Private texture with clut data, only accessed by GPU.
        id<MTLBuffer>   m_clutBuffer = nil;         // Shared buffer, containing clut data accessible to CPU.
        
		id<MTLTexture>	m_texture = nil;			// Private texture, only accessed by GPU.
        id<MTLBuffer>   m_textureBuffer = nil;      // Shared buffer, containing texture data accessible to CPU.
        
        MTLPixelFormat  m_internalFormat;

		uint32_t	    m_pixelSize;		// Size in bytes of a pixel.
		static SizeI	s_maxSize;


	};
} // namespace wg
#endif //WG_METALSURFACE_DOT_H

