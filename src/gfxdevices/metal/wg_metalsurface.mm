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
        
        _setupMetalTexture( nullptr, 0, nullptr, flags, pClut );
	}


	MetalSurface::MetalSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags, const Color * pClut ) : Surface(flags)
	{
		// Set general information

		_setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;

        PixelDescription srcFormat;
        Util::pixelFormatToDescription(format, srcFormat);

		_setupMetalTexture(pBlob->data(), pitch, &srcFormat, flags, pClut);
	}

	MetalSurface::MetalSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut ) : Surface(flags)
	{
	   _setPixelDetails(format);
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= size;

        PixelDescription srcFormat;
        
        if( pPixelDescription == nullptr )
        {
            Util::pixelFormatToDescription(format, srcFormat);
            pPixelDescription = &srcFormat;
        }
        
		_setupMetalTexture( pPixels, pitch, pPixelDescription, flags, pClut);
	}


	MetalSurface::MetalSurface( Surface * pOther, int flags ) : Surface(flags)
	{
		_setPixelDetails(pOther->pixelFormat());
		m_scaleMode = ScaleMode::Interpolate;
		m_size	= pOther->size();

        
        auto pixbuf = pOther->allocPixelBuffer();
        if( pOther->pushPixels(pixbuf) )
            _setupMetalTexture(pixbuf.pPixels, pixbuf.pitch, pOther->pixelDescription(), flags, pOther->clut());
        else
        {
            // Error handling
        }

        pOther->freePixelBuffer(pixbuf);
	}

    //____ _setupMetalTexture() __________________________________________________________________

    void MetalSurface::_setupMetalTexture(void * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags, const Color * pClut )
    {
        if(flags & SurfaceFlag::Mipmapped)
            m_bMipmapped = true;
        
        // Create our shared buffer
        
        int     bufferLength = m_size.w * m_size.h * m_pixelSize;

        m_textureBuffer = [MetalGfxDevice::s_metalDevice newBufferWithLength:bufferLength options:MTLResourceStorageModeShared];
        
        // Copy pixel data to our shared buffer
              
        if( pPixels )
        {
            _copyFrom( pPixelDescription, (uint8_t*) pPixels, pitch, m_size, m_size );
        }
               
        // Setup the clut if present
        
        if( pClut )
        {
            // Create the clut buffer and copy data

            m_clutBuffer = [MetalGfxDevice::s_metalDevice newBufferWithBytes:pClut length:1024 options:MTLResourceStorageModeShared];
            m_pClut = (Color*) [m_clutBuffer contents];
        }
        
        //
        
        _createAndSyncTextures();
        
        setScaleMode(m_scaleMode);
    }

    //____ _createAndSyncTextures() __________________________________________________

    void MetalSurface::_createAndSyncTextures()
    {
        // Create the private texture
        
        MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];

        textureDescriptor.pixelFormat   = m_internalFormat;
        textureDescriptor.width         = m_size.w;
        textureDescriptor.height        = m_size.h;
        textureDescriptor.storageMode   = MTLStorageModePrivate;
        textureDescriptor.usage         = (m_flags & SurfaceFlag::Canvas) ? (MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead) : MTLTextureUsageShaderRead;
        
        if(m_bMipmapped)
        {
            int heightLevels = ceil(log2(m_size.h));
            int widthLevels = ceil(log2(m_size.w));
            int mipCount = (heightLevels > widthLevels) ? heightLevels : widthLevels;
            
            textureDescriptor.mipmapLevelCount = mipCount;
        }

        m_texture = [MetalGfxDevice::s_metalDevice newTextureWithDescriptor:textureDescriptor];

        // Create the clut texture
        
        if( m_pClut )
        {
            MTLTextureDescriptor *clutDescriptor = [[MTLTextureDescriptor alloc] init];

            clutDescriptor.pixelFormat   = m_pixelDescription.bLinear ? MTLPixelFormatBGRA8Unorm : MTLPixelFormatBGRA8Unorm_sRGB;
            clutDescriptor.width         = 256;
            clutDescriptor.height        = 1;
            clutDescriptor.storageMode   = MTLStorageModePrivate;

            m_clutTexture = [MetalGfxDevice::s_metalDevice newTextureWithDescriptor:clutDescriptor];
        }

        // Copy from buffers to textures (pixels and cluts)
        
        MTLSize textureSize = { (unsigned) m_size.w, (unsigned) m_size.h, 1};
        MTLOrigin textureOrigin = {0,0,0};
        
        id<MTLCommandBuffer> commandBuffer = [MetalGfxDevice::s_metalCommandQueue commandBuffer];
        
        id<MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder copyFromBuffer:     m_textureBuffer
                            sourceOffset:       0
                            sourceBytesPerRow:  m_size.w * m_pixelSize
                            sourceBytesPerImage:0
                            sourceSize:         textureSize
                            toTexture:          m_texture
                            destinationSlice:   0
                            destinationLevel:   0
                            destinationOrigin:  textureOrigin];

        if( m_pClut )
        {
            MTLSize clutSize = { 256, 1, 1 };
            MTLOrigin clutOrigin = {0,0,0};

            [blitCommandEncoder copyFromBuffer:     m_clutBuffer
                                sourceOffset:       0
                                sourceBytesPerRow:  1024
                                sourceBytesPerImage:0
                                sourceSize:         clutSize
                                toTexture:          m_clutTexture
                                destinationSlice:   0
                                destinationLevel:   0
                                destinationOrigin:  clutOrigin];
        }

        if(m_bMipmapped)
            [blitCommandEncoder generateMipmapsForTexture:m_texture];
        
        [blitCommandEncoder endEncoding];

        m_bTextureSyncInProgress = true;
        
        // Add a completion handler and commit the command buffer.
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Private texture is populated.
            
            m_bTextureSyncInProgress = false;
        }];
        [commandBuffer commit];
    }

    //____ _setPixelDetails() __________________________________________________________

	void MetalSurface::_setPixelDetails( PixelFormat format )
	{
		switch (format)
		{
            case PixelFormat::BGR_8:
                format = PixelFormat::BGRX_8;
            case PixelFormat::BGRX_8:
			case PixelFormat::BGRA_8:
				m_internalFormat = Base::activeContext()->gammaCorrection() ? MTLPixelFormatBGRA8Unorm_sRGB : MTLPixelFormatBGRA8Unorm;
				break;
                
            case PixelFormat::BGR_8_sRGB:
                format = PixelFormat::BGRX_8_sRGB;
            case PixelFormat::BGRX_8_sRGB:
			case PixelFormat::BGRA_8_sRGB:
				m_internalFormat = MTLPixelFormatBGRA8Unorm_sRGB;
				break;

            case PixelFormat::BGR_8_linear:
                format = PixelFormat::BGRX_8_linear;
            case PixelFormat::BGRX_8_linear:
			case PixelFormat::BGRA_8_linear:
				m_internalFormat = MTLPixelFormatBGRA8Unorm;
				break;

			case PixelFormat::BGR_565_linear:
#if TARGET_OS_IPHONE
				m_internalFormat = MTLPixelFormatB5G6R5Unorm;
#else
                format = PixelFormat::BGRX_8_linear;
                m_internalFormat = MTLPixelFormatBGRA8Unorm;
#endif
				break;
                
            case PixelFormat::BGRA_4_linear:
                format = PixelFormat::BGRA_8_linear;
                m_internalFormat = MTLPixelFormatBGRA8Unorm;
                break;
                
			case PixelFormat::CLUT_8:
			case PixelFormat::CLUT_8_sRGB:
			case PixelFormat::CLUT_8_linear:
				m_internalFormat = MTLPixelFormatR8Unorm;
				break;

			case PixelFormat::A_8:
				m_internalFormat = MTLPixelFormatR8Unorm;
				break;

			default:
                Base::handleError( ErrorSeverity::Critical, ErrorCode::InvalidParam, "Specified PixelFormat not supported", this, TYPEINFO, __func__, __FILE__, __LINE__ );
				assert(false);           // Should never get here, just avoiding compiler warnings.
				break;
		}

		Util::pixelFormatToDescription(format, m_pixelDescription);
        m_pixelSize = m_pixelDescription.bits / 8;
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
		Surface::setScaleMode(mode);
	}

	//____ isOpaque() ______________________________________________________________

	bool MetalSurface::isOpaque() const
	{
//		if( m_internalFormat == GL_RGB )
//			return true;

        //TODO: Implement!
        
		return false;
	}

    //____ allocPixelBuffer() ______________________________________________________

    const PixelBuffer MetalSurface::allocPixelBuffer(const RectI& rect)
    {
        PixelBuffer pixbuf;

        pixbuf.format = m_pixelDescription.format;
        pixbuf.pClut = m_pClut;
        pixbuf.pitch = m_size.w * m_pixelSize;
        pixbuf.pPixels = ((uint8_t*)[m_textureBuffer contents]) + rect.y * pixbuf.pitch + rect.x * m_pixelSize;
        pixbuf.rect = rect;
        return pixbuf;
    }

    //____ pushPixels() ____________________________________________________________

    bool MetalSurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
    {
        if( !m_texture )
            return false;
        
        // Make sure we have any changes made by GPU
         
         if( m_bBufferNeedsSync )
             _syncBufferAndWait();
         else
             _waitForSyncedTexture();
        
        return true;
    }

    //____ pullPixels() _____________________________________________________________

    void MetalSurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
    {
        if( m_texture )
            _syncTexture( bufferRect + buffer.rect.pos() );
    }

    //____ freePixelBuffer() ________________________________________________________

    void MetalSurface::freePixelBuffer(const PixelBuffer& buffer)
    {
        // Do nothing.
    }

	//____ alpha() ____________________________________________________________

	uint8_t MetalSurface::alpha( CoordI coord )
	{
        if( m_bBufferNeedsSync )
            _syncBufferAndWait();

        //TODO: Take endianess into account.

        uint8_t * p = (uint8_t*) [m_textureBuffer contents];

        int pitch = m_size.w * m_pixelSize;
        
        switch (m_pixelDescription.format)
        {
            case PixelFormat::CLUT_8_sRGB:
            case PixelFormat::CLUT_8_linear:
            {
                uint8_t index = p[pitch * coord.y + coord.x];
                return m_pClut[index].a;
            }
            case PixelFormat::A_8:
            {
                uint8_t * pPixel = p + pitch * coord.y + coord.x;
                return pPixel[0];
            }
            case PixelFormat::BGRA_4_linear:
            {
                uint16_t pixel = * (uint16_t *)(p + pitch * coord.y + coord.x);
                const uint8_t * pConvTab = s_pixelConvTabs[4];

                return pConvTab[(pixel & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift];
            }
            case PixelFormat::BGRA_8_sRGB:
            case PixelFormat::BGRA_8_linear:
            {
                uint8_t * pPixel = p + pitch * coord.y + coord.x * 4;
                return pPixel[3];
            }
            default:
                return 0xFF;
        }
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

	//____ reload() _________________________________________________________

	void MetalSurface::reload()
	{
        _createAndSyncTextures();
	}

	//____ _syncBufferAndWait() ____________________________________________

	void MetalSurface::_syncBufferAndWait()
	{
		// Flush any active device to make sure our texture is up-to-date

		if (MetalGfxDevice::s_pActiveDevice)
			MetalGfxDevice::s_pActiveDevice->flush();

		//
        MTLSize textureSize = { (unsigned) m_size.w, (unsigned) m_size.h,1};
        MTLOrigin textureOrigin = { 0, 0, 0};
        
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

        [commandBuffer commit];

        [commandBuffer waitUntilCompleted];

        m_bBufferNeedsSync = false;

    }

    //____ _syncTexture() _______________________________________________

    void MetalSurface::_syncTexture(RectI region)
    {
//        if (m_bPendingReads)
//            MetalGfxDevice::s_pActiveDevice->flush();
//        if (MetalGfxDevice::s_pActiveDevice)
//            MetalGfxDevice::s_pActiveDevice->flush();

        //

        MTLSize textureSize = { (unsigned) region.w, (unsigned) region.h,1};
        MTLOrigin textureOrigin = { (unsigned) region.x, (unsigned) region.y,0};

        int     sourceOffset = region.y * m_size.w * m_pixelSize + region.x * m_pixelSize;
        
        id<MTLCommandBuffer> commandBuffer = [MetalGfxDevice::s_metalCommandQueue commandBuffer];
        
        id<MTLBlitCommandEncoder> blitCommandEncoder = [commandBuffer blitCommandEncoder];
        [blitCommandEncoder copyFromBuffer:     m_textureBuffer
                            sourceOffset:       sourceOffset
                            sourceBytesPerRow:  m_size.w * m_pixelSize
                            sourceBytesPerImage:0
                            sourceSize:         textureSize
                            toTexture:          m_texture
                            destinationSlice:   0
                            destinationLevel:   0
                            destinationOrigin:  textureOrigin];

        if(m_bMipmapped)
            [blitCommandEncoder generateMipmapsForTexture:m_texture];
        
        [blitCommandEncoder endEncoding];

        m_bTextureSyncInProgress = true;
        
        // Add a completion handler and commit the command buffer.
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Private texture is populated.
            
            m_bTextureSyncInProgress = false;
        }];
        [commandBuffer commit];
        
        _waitForSyncedTexture();
    }


    //____ _waitForSyncedTexture() _______________________________________________

    void MetalSurface::_waitForSyncedTexture()
    {
        while( m_bTextureSyncInProgress )
            usleep(20);        // Sleep for 0.02 millisec
    }



} // namespace wg
