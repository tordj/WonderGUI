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

#include <memory.h>

#include <wg_metalsurface.h>
#include <wg_metalgfxdevice.h>
#include <wg_util.h>
#include <wg_blob.h>
#include <wg_base.h>
#include <wg_context.h>
#include <assert.h>



namespace wg
{
	const TypeInfo MetalSurface::TYPEINFO = { "MetalSurface", &Surface::TYPEINFO };

	SizeI	MetalSurface::s_maxSize;

#define HANDLE_GLERROR(check) { GLenum err = check; if(err != 0) GlGfxDevice::onGlError(err, this, TYPEINFO, __func__, __FILE__, __LINE__ ); }



	//____ maxSize() _______________________________________________________________

	SizeI MetalSurface::maxSize()
	{
		if (s_maxSize.w == 0)
		{
			GLint max = 16384;
			s_maxSize.w = max;
			s_maxSize.h = max;
		}

		return s_maxSize;
	}

	//____ create ______________________________________________________________

	MetalSurface_p	MetalSurface::create( SizeI size, PixelFormat format, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return MetalSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr))
			return MetalSurface_p();

		return MetalSurface_p(new MetalSurface(size,format,flags,pClut));
	}

	MetalSurface_p	MetalSurface::create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return MetalSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max || ((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || !pBlob || pitch % 4 != 0)
			return MetalSurface_p();

		return MetalSurface_p(new MetalSurface(size,format,pBlob,pitch,flags,pClut));
	}

	MetalSurface_p	MetalSurface::create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut )
	{
		SizeI max = maxSize();
		if (size.w > max.w || size.h > max.h)
			return MetalSurface_p();

		if (format == PixelFormat::Unknown || format == PixelFormat::Custom || format < PixelFormat_min || format > PixelFormat_max ||
			((format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear) && pClut == nullptr) || pPixels == nullptr || pitch <= 0 )
			return MetalSurface_p();

		return  MetalSurface_p(new MetalSurface(size,format,pPixels,pitch, pPixelDescription,flags,pClut));
	};

	MetalSurface_p	MetalSurface::create( Surface * pOther, int flags )
	{
		if (!pOther)
			return MetalSurface_p();

		SizeI max = maxSize();
		SizeI size = pOther->size();
		if (size.w > max.w || size.h > max.h)
			return MetalSurface_p();

		return MetalSurface_p(new MetalSurface( pOther,flags ));
	}



	//____ constructor _____________________________________________________________


	MetalSurface::MetalSurface( SizeI size, PixelFormat format, int flags, const Color * pClut ) : Surface(flags)
	{
		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
        
		m_pitch = ((size.w*m_pixelDescription.bits/8)+3)&0xFFFFFFFC;
        _setupMetalTexture( nullptr, 0, nullptr, pClut );
	}


	MetalSurface::MetalSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) : Surface(flags)
	{
		// Set general information

		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pitch = pitch;

		_setupMetalTexture(pBlob->data(), pitch, &m_pixelDescription, pClut);
	}

	MetalSurface::MetalSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut ) : Surface(flags)
	{
	   _setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;
		m_pitch = ((size.w*m_pixelDescription.bits/8)+3)&0xFFFFFFFC;

        if( pPixelDescription == nullptr )
            pPixelDescription = &m_pixelDescription;
        
		_setupMetalTexture( pPixels, pitch, pPixelDescription, pClut);
	}


	MetalSurface::MetalSurface( Surface * pOther, int flags ) : Surface(flags)
	{
		_setPixelDetails(pOther->pixelFormat());
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= pOther->size();
		m_pitch = m_size.w * m_pixelSize;

        pOther->lock(AccessMode::ReadOnly);
		_setupMetalTexture(pOther->pixels(), pOther->pitch(), pOther->pixelDescription(), pOther->clut());
        pOther->unlock();
	}

    //____ _setupMetalTexture() __________________________________________________________________

    void MetalSurface::_setupMetalTexture(void * pPixels, int pitch, const PixelDescription * pPixelDescription, const Color * pClut )
    {
        // Create our shared buffer
        
        int     bufferLength = m_size.w * m_size.h * m_pixelSize;
        int     lineLength = m_size.w * m_pixelSize;

        m_textureBuffer = [MetalGfxDevice::s_metalDevice newBufferWithLength:bufferLength options:MTLResourceStorageModeShared];
        
        // Copy pixel data to our shared buffer
       
        if( pPixels )
        {
            m_pPixels = (uint8_t*) [m_textureBuffer contents];
            _copyFrom( pPixelDescription, (uint8_t*) pPixels, pitch, m_size, m_size );
            m_pPixels = 0;

 /*
            char *  pDest = (char *) [m_textureBuffer contents];
            char *  pSource = (char *) pPixels;
            
            for( int y = 0 ; y < m_size.h ; y++ )
            {
                memcpy(pDest, pSource, lineLength);
                pDest += lineLength;
                pSource += pitch;
            }
 */
        }
        
        // Create the private texture
        
        MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];

        textureDescriptor.pixelFormat   = m_internalFormat;
        textureDescriptor.width         = m_size.w;
        textureDescriptor.height        = m_size.h;
        textureDescriptor.storageMode   = MTLStorageModePrivate;

        m_texture = [MetalGfxDevice::s_metalDevice newTextureWithDescriptor:textureDescriptor];

        // Setup the clut if present
        
        if( pClut )
        {
            // Create the clut buffer and copy data

            m_clutBuffer = [MetalGfxDevice::s_metalDevice newBufferWithBytes:pClut length:1024 options:MTLResourceStorageModeShared];
            
            MTLTextureDescriptor *clutDescriptor = [[MTLTextureDescriptor alloc] init];

            clutDescriptor.pixelFormat   = m_pixelDescription.bLinear ? MTLPixelFormatBGRA8Unorm : MTLPixelFormatBGRA8Unorm_sRGB;
            clutDescriptor.width         = 256;
            clutDescriptor.height        = 1;
            clutDescriptor.storageMode   = MTLStorageModePrivate;

            m_clutTexture = [MetalGfxDevice::s_metalDevice newTextureWithDescriptor:clutDescriptor];

            m_pClut = (Color*) [m_clutBuffer contents];
        }
        
        // Copy from buffers to textures (pixels and cluts)
        
        MTLSize textureSize = { (unsigned) m_size.w, (unsigned) m_size.h, 1};
        MTLOrigin textureOrigin = {0,0,0};
        
        id<MTLCommandBuffer> commandBuffer = [MetalGfxDevice::s_metalCommandQueue commandBuffer];
        
        id<MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder copyFromBuffer:     m_textureBuffer
                            sourceOffset:       0
                            sourceBytesPerRow:  lineLength
                            sourceBytesPerImage:bufferLength
                            sourceSize:         textureSize
                            toTexture:          m_texture
                            destinationSlice:   0
                            destinationLevel:   0
                            destinationOrigin:  textureOrigin];

        if( pClut )
        {
            MTLSize clutSize = { 256, 1, 1 };
            MTLOrigin clutOrigin = {0,0,0};

            [blitCommandEncoder copyFromBuffer:     m_clutBuffer
                                sourceOffset:       0
                                sourceBytesPerRow:  1024
                                sourceBytesPerImage:1024
                                sourceSize:         clutSize
                                toTexture:          m_clutTexture
                                destinationSlice:   0
                                destinationLevel:   0
                                destinationOrigin:  clutOrigin];
        }

        [blitCommandEncoder endEncoding];

        m_bTextureSyncInProgress = true;
        m_bTextureNeedsSync = false;
        
        // Add a completion handler and commit the command buffer.
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Private texture is populated.
            
            m_bTextureSyncInProgress = false;
        }];
        [commandBuffer commit];
        
        //TODO: Mipmap handling.
        

        setScaleMode(m_scaleMode);
    }

    //____ _setPixelDetails() __________________________________________________________

	void MetalSurface::_setPixelDetails( PixelFormat format )
	{
		switch (format)
		{
			case PixelFormat::BGRA_8:
				m_internalFormat = Base::activeContext()->gammaCorrection() ? MTLPixelFormatBGRA8Unorm_sRGB : MTLPixelFormatBGRA8Unorm;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRA_8_sRGB:
				m_internalFormat = MTLPixelFormatBGRA8Unorm_sRGB;
				m_pixelSize = 4;
				break;

			case PixelFormat::BGRA_8_linear:
				m_internalFormat = MTLPixelFormatBGRA8Unorm;
				m_pixelSize = 4;
				break;

#if TARGET_OS_IPHONE
			case PixelFormat::BGR_565_linear:
				m_internalFormat = MTLPixelFormatB5G6R5Unorm;
				m_pixelSize = 2;
				break;
#endif
			case PixelFormat::CLUT_8:
			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
				m_internalFormat = MTLPixelFormatR8Uint;
				m_pixelSize = 1;
				break;

			case PixelFormat::A_8:
				m_internalFormat = MTLPixelFormatA8Unorm;
				m_pixelSize = 1;
				break;

			default:
                Base::handleError( ErrorSeverity::Critical, ErrorCode::InvalidParam, "Specified PixelFormat not supported for MetalSurface on this OS", this, TYPEINFO, __func__, __FILE__, __LINE__ );
				assert(false);           // Should never get here, just avoiding compiler warnings.
				break;
		}

		Util::pixelFormatToDescription(format, m_pixelDescription);
	}

	//____ Destructor ______________________________________________________________

	MetalSurface::~MetalSurface()
	{
		// Free the stuff

        m_texture = nil;
        m_textureBuffer = nil;
        
		m_clutTexture = nil;
        m_clutBuffer = nil;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& MetalSurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScaleMode() __________________________________________________________

	void MetalSurface::setScaleMode( ScaleMode mode )
	{
/*
		if (m_pClut == nullptr)
		{
			GLint oldBinding;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBinding);
			glBindTexture(GL_TEXTURE_2D, m_texture);

			switch (mode)
			{
			case ScaleMode::Interpolate:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bMipmapped ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case ScaleMode::Nearest:
			default:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			}

			glBindTexture(GL_TEXTURE_2D, oldBinding);
		}
*/
		Surface::setScaleMode(mode);
	}

	//____ isOpaque() ______________________________________________________________

	bool MetalSurface::isOpaque() const
	{
//		if( m_internalFormat == GL_RGB )
//			return true;

		return false;
	}

	//____ setTiling() ________________________________________________________

	void MetalSurface::setTiling(bool bTiling)
	{
/*
        GLint mode = bTiling ? GL_REPEAT : GL_CLAMP_TO_EDGE;

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
 */
	}


	//____ lock() __________________________________________________________________

	uint8_t * MetalSurface::lock( AccessMode mode )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

        // Make sure we have any changes made by GPU
        
        if( m_bBufferNeedsSync )
            _syncBuffer();
        
        // Wait for any sync in progress (either way) to complete
        
        _waitForSyncToComplete();
        
        //
		//

		m_pPixels = (uint8_t*) [m_textureBuffer contents];
		m_lockRegion = RectI(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * MetalSurface::lockRegion( AccessMode mode, const RectI& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

        // Make sure we have any changes made by GPU
        
        if( m_bBufferNeedsSync )
            _syncBuffer();
        
        // Wait for any sync in progress (either way) to complete
        
        _waitForSyncToComplete();
        
		//

		if( region.x + region.w > m_size.w || region.y + region.h > m_size.h || region.x < 0 || region.y < 0 )
			return 0;

		m_pPixels = ((uint8_t*)[m_textureBuffer contents]) + region.y*m_pitch + region.x*m_pixelSize;
		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels;
	}


	//____ unlock() ________________________________________________________________

	void MetalSurface::unlock()
	{
		if(m_accessMode == AccessMode::None )
			return;

		if( m_accessMode != AccessMode::ReadOnly )
		{
            _syncTexture( m_lockRegion );
		}
		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
	}

	//____ pixel() ______________________________________________________________

	uint32_t MetalSurface::pixel( CoordI coord ) const
	{
        //TODO: Figure out how to refresh stale backing buffer

//		if (m_bBackingBufferStale)
//			_refreshBackingBuffer();

        uint32_t val;

        uint8_t * pPixel = ((uint8_t*) [m_textureBuffer contents]) + coord.y*m_pitch + coord.x*m_pixelSize;

        switch( m_pixelSize )
        {
            case 1:
                val = (uint32_t) *pPixel;
            case 2:
                val = (uint32_t) ((uint16_t*) pPixel)[0];
            case 3:
                val = ((uint32_t) pPixel[0]) + (((uint32_t) pPixel[1]) << 8) + (((uint32_t) pPixel[2]) << 16);
            default:
                val = *((uint32_t*) pPixel);
        }

        return val;
	}



	//____ alpha() ____________________________________________________________

	uint8_t MetalSurface::alpha( CoordI coord ) const
	{
        //TODO: Figure out how to refresh stale backing buffer
        //TODO: Support more pixelformats than BGRA_8 only.
        
//		if (m_bBackingBufferStale)
//			_refreshBackingBuffer();

		if( m_pixelDescription.format == PixelFormat::BGRA_8 )
		{
			uint8_t * p = (uint8_t*) [m_textureBuffer contents];
			return p[coord.y*m_pitch+coord.x*4+3];
		}
		else
			return 255;
	}

	//____ unload() ___________________________________________________________

	bool MetalSurface::unload()
	{
		if( m_texture == 0 )
			return true;

		m_texture = nil;
        m_clutTexture = nil;

		return true;
	}

	//____ isLoaded() _________________________________________________________

	bool MetalSurface::isLoaded()
	{
		return (m_texture != nil);
	}

	//____ reloaded() _________________________________________________________

	void MetalSurface::reload()
	{
/*        _setupMetalTexture(m_pBlob->data());

		m_bMipmapStale = m_bMipmapped;
 */
	}

	//____ _syncBuffer() ____________________________________________

	void MetalSurface::_syncBuffer()
	{
		// Flush any active device to make sure our texture is up-to-date

		if (MetalGfxDevice::s_pActiveDevice)
			MetalGfxDevice::s_pActiveDevice->flush();

        _waitForSyncToComplete();
        
        m_bBufferSyncInProgress = true;
        m_bBufferNeedsSync = false;

		//
        MTLSize textureSize = { (unsigned) m_size.w, (unsigned) m_size.h};
        MTLOrigin textureOrigin = { 0, 0};
        
        id<MTLCommandBuffer> commandBuffer = [MetalGfxDevice::s_metalCommandQueue commandBuffer];

        id <MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder     copyFromTexture:    m_texture
                                sourceSlice:        0
                                sourceLevel:        0
                                sourceOrigin:       textureOrigin
                                sourceSize:         textureSize
                                toBuffer:           m_textureBuffer
                                destinationOffset:  0
                                destinationBytesPerRow: m_pixelSize * m_size.w
                                destinationBytesPerImage: m_pixelSize * m_size.w * m_size.h ];
        [blitCommandEncoder endEncoding];


        // Add a completion handler and commit the command buffer.
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Shared buffer is populated.
            m_bBufferSyncInProgress = false;
        }];
        [commandBuffer commit];
	}

    //____ _syncTexture() _______________________________________________

    void MetalSurface::_syncTexture(RectI region)
    {
        // If we have pending reads we need to flush them first
        
        if (m_bPendingReads && (m_accessMode == AccessMode::ReadWrite || m_accessMode == AccessMode::WriteOnly))
            MetalGfxDevice::s_pActiveDevice->flush();
        
        //TODO: We should wait for commands to complete!
        
        //
        
        int     bufferLength = m_size.w * m_size.h * m_pixelSize;
        int     lineLength = m_size.w * m_pixelSize;

        MTLSize textureSize = { (unsigned) region.w, (unsigned) region.h};
        MTLOrigin textureOrigin = { (unsigned) region.x, (unsigned) region.y,0};

        id<MTLCommandBuffer> commandBuffer = [MetalGfxDevice::s_metalCommandQueue commandBuffer];
        
        id<MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder copyFromBuffer:     m_textureBuffer
                            sourceOffset:       0
                            sourceBytesPerRow:  lineLength
                            sourceBytesPerImage:bufferLength
                            sourceSize:         textureSize
                            toTexture:          m_texture
                            destinationSlice:   0
                            destinationLevel:   0
                            destinationOrigin:  textureOrigin];

        [blitCommandEncoder endEncoding];

        m_bTextureSyncInProgress = true;
        m_bTextureNeedsSync = false;
        
        // Add a completion handler and commit the command buffer.
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Private texture is populated.
            
            m_bTextureSyncInProgress = false;
        }];
        [commandBuffer commit];
        
        m_bMipmapStale = m_bMipmapped;
    }


    //____ _waitForSyncToComplete() _______________________________________________

    void MetalSurface::_waitForSyncToComplete()
    {
        while( m_bBufferSyncInProgress || m_bTextureSyncInProgress )
            usleep(50);        // Sleep for 0.05 millisec
    }



} // namespace wg
