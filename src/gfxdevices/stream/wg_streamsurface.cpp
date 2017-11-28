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

#include <wg_streamsurface.h>
#include <wg_util.h>
#include <wg_blob.h>
#include <assert.h>



namespace wg
{
	const char StreamSurface::CLASSNAME[] = {"StreamSurface"};

	//____ maxSize() _______________________________________________________________

	Size StreamSurface::maxSize()
	{
		return Size(4096,4096);
	}

	//____ create ______________________________________________________________

    StreamSurface_p	StreamSurface::create( GfxOutStream * pStream, Size size, PixelType type, int hint )
    {
        if( type != PixelType::BGRA_8 && type != PixelType::BGR_8)
            return StreamSurface_p();
        
        return StreamSurface_p(new StreamSurface(pStream,size,type,hint));
    }
    
    StreamSurface_p	StreamSurface::create( GfxOutStream * pStream, Size size, PixelType type, Blob * pBlob, int pitch, int hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || !pBlob || pitch % 4 != 0 )
            return StreamSurface_p();
        
        return StreamSurface_p(new StreamSurface(pStream,size,type,pBlob,pitch,hint));
    }
    
    StreamSurface_p	StreamSurface::create( GfxOutStream * pStream,Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint )
    {
        if( (type != PixelType::BGRA_8 && type != PixelType::BGR_8) || pPixels == 0 )
            return StreamSurface_p();
        
        return  StreamSurface_p(new StreamSurface(pStream,size,type,pPixels,pitch, pPixelFormat,hint));
    };
    
    StreamSurface_p	StreamSurface::create( GfxOutStream * pStream, Surface * pOther, int hint )
    {
        return StreamSurface_p(new StreamSurface( pStream,pOther,hint ));
    }

    
    
	//____ Constructor _____________________________________________________________

    StreamSurface::StreamSurface( GfxOutStream * pStream,Size size, PixelType type, int hint )
    {
		assert( type == PixelType::BGR_8 || type == PixelType::BGRA_8 );
		Util::pixelTypeToFormat(type, m_pixelFormat);

		m_pStream = pStream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelFormat.bits / 8;

		m_inStreamId = _sendCreateSurface(size, type);

		if (hint & SurfaceHint::WriteOnly)
		{
			if (m_pixelFormat.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
			{
				m_pAlphaLayer = new uint8_t[size.w*size.h];
				std::memset(m_pAlphaLayer, 0, size.w*size.h*2);
			}
		}
		else
		{
			m_pBlob = Blob::create(m_pitch*size.h);
			std::memset(m_pBlob->data(), 0, m_pitch*size.h);

			m_pAlphaLayer = nullptr;
		}
    }
        
	StreamSurface::StreamSurface( GfxOutStream * pStream,Size size, PixelType type, Blob * pBlob, int pitch, int hint )
	{
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pBlob && pitch % 4 == 0 );
		Util::pixelTypeToFormat(type, m_pixelFormat);

		m_pStream = pStream;
		m_size = size;
		m_pitch = pitch;

		m_inStreamId = _sendCreateSurface(size, type);

		if (hint & SurfaceHint::WriteOnly)
		{
			if (m_pixelFormat.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pBlob->data(), pitch);
		}
		else
		{
			m_pBlob = pBlob;
			m_pAlphaLayer = nullptr;
		}

		_sendPixels(size, (uint8_t*) pBlob->data(), pitch);
	}
   
    StreamSurface::StreamSurface( GfxOutStream * pStream,Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, int hint )
    {
		assert( (type == PixelType::BGR_8 || type == PixelType::BGRA_8) && pPixels != 0 );
		Util::pixelTypeToFormat(type, m_pixelFormat);

		m_pStream = pStream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelFormat.bits / 8;

		m_inStreamId = _sendCreateSurface(size, type);

		// We always convert the data even if we throw it away, since we need to stream the converted data.
		// (but we could optimize and skip conversion if format already is correct)

		Blob_p pBlob = Blob::create(m_pitch*m_size.h);

		m_pPixels = (uint8_t*)m_pBlob->data();	// Simulate a lock
		_copyFrom(pPixelFormat == 0 ? &m_pixelFormat : pPixelFormat, pPixels, pitch, size, size);
		m_pPixels = 0;


		if (hint & SurfaceHint::WriteOnly)
		{
			if (m_pixelFormat.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}
		else
		{
			m_pBlob = pBlob;
			m_pAlphaLayer = nullptr;
		}

		_sendPixels(size, pPixels, pitch);
	}


    StreamSurface::StreamSurface( GfxOutStream * pStream,Surface * pOther, int hint )
    {
		assert( pOther );

		PixelType type = pOther->pixelFormat()->type;
		uint8_t * pPixels = (uint8_t*)pOther->lock(AccessMode::ReadOnly);
		int pitch = pOther->pitch();
		Size size = pOther->size();

		m_pStream = pStream;
		m_size = size;
		m_pitch = ((size.w + 3) & 0xFFFFFFFC)*m_pixelFormat.bits / 8;

		assert(type == PixelType::BGR_8 || type == PixelType::BGRA_8);
		Util::pixelTypeToFormat(type, m_pixelFormat);

		if (hint & SurfaceHint::WriteOnly)
		{
			if (m_pixelFormat.A_bits == 0)
				m_pAlphaLayer = nullptr;
			else
				m_pAlphaLayer = _genAlphaLayer((char*)pPixels, pitch);
		}
		else
		{
			m_pBlob = Blob::create(m_pitch*m_size.h);

			m_pPixels = (uint8_t*)m_pBlob->data();	// Simulate a lock
			_copyFrom(&m_pixelFormat, pPixels, pitch, Rect(size), Rect(size));
			m_pPixels = 0;
		}

		_sendPixels(size, pPixels, pitch);
		pOther->unlock();
    }
    
    
	//____ Destructor ______________________________________________________________

	StreamSurface::~StreamSurface()
	{
		if (!m_pBlob && m_pPixels)
			delete[] m_pPixels;		// Surface shouldn't really be deleted while locked, but let's clean up nicely anyway...

		_sendDeleteSurface();
	}

	//____ isInstanceOf() _________________________________________________________

	bool StreamSurface::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;

		return Surface::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * StreamSurface::className( void ) const
	{ 
		return CLASSNAME; 
	}

	//____ cast() _________________________________________________________________

	StreamSurface_p StreamSurface::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StreamSurface_p( static_cast<StreamSurface*>(pObject) );

		return 0;
	}

	//____ setScaleMode() __________________________________________________________

	void StreamSurface::setScaleMode( ScaleMode mode )
	{
		switch( mode )
		{
			case ScaleMode::Interpolate:
				break;
				
			case ScaleMode::Nearest:
			default:
				break;
		}
		
		Surface::setScaleMode(mode);
	}

	//____ size() ______________________________________________________________

	Size StreamSurface::size() const
	{
		return m_size;
	}

	//____ isOpaque() ______________________________________________________________

	bool StreamSurface::isOpaque() const
	{
		if (m_pixelFormat.A_bits == 0)
			return true;

		return false;
	}

	//____ lock() __________________________________________________________________

	uint8_t * StreamSurface::lock( AccessMode mode )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if (!m_pBlob && mode != AccessMode::WriteOnly)
			return 0;

		if (m_pBlob)
			m_pPixels = (uint8_t*) m_pBlob->data();
		else
			m_pPixels = new uint8_t[m_size.h*m_pitch];

		m_lockRegion = Rect(0,0,m_size);
		m_accessMode = mode;
		return m_pPixels;
	}

	//____ lockRegion() __________________________________________________________________

	uint8_t * StreamSurface::lockRegion( AccessMode mode, const Rect& region )
	{
		if( m_accessMode != AccessMode::None || mode == AccessMode::None )
			return 0;

		if (!m_pBlob && mode != AccessMode::WriteOnly)
			return 0;

		if( region.x + region.w > m_size.w || region.y + region.w > m_size.h || region.x < 0 || region.y < 0 )
			return 0;

		if( m_pBlob )
			m_pPixels = ((uint8_t*)m_pBlob->data()) + m_pitch*region.y + region.x*m_pixelFormat.bits / 8;
		else
		{
			m_pitch = ((region.w + 3) & 0xFFFFFFFC)*m_pixelFormat.bits / 8;
			m_pPixels = new uint8_t[m_pitch*region.h];
		}
		m_lockRegion = region;
		m_accessMode = mode;
		return m_pPixels;
	}


	//____ unlock() ________________________________________________________________

	void StreamSurface::unlock()
	{
		if(m_accessMode ==  AccessMode::None )
			return;

		_sendPixels(m_lockRegion, m_pPixels, m_pitch);

		if (!m_pBlob)
		{
			delete[] m_pPixels;
			m_pitch = 0;
		}

		m_accessMode = AccessMode::None;
		m_pPixels = 0;
		m_lockRegion.w = 0;
		m_lockRegion.h = 0;
	}


	//____ pixel() ______________________________________________________________

	uint32_t StreamSurface::pixel( Coord coord ) const
	{
		if (!m_pBlob)
			return 0;

		uint8_t * pPixel = &((uint8_t*)m_pBlob->data())[coord.x*m_pixelFormat.bits / 8 + coord.y*m_pitch];

		switch (m_pixelFormat.bits)
		{
		case 8:
			while (true);	// Error: unsupported pixel format.
		case 16:
			while (true);	// Error: unsupported pixel format.
		case 24:
#if WG_IS_BIG_ENDIAN
			return pPixel[2] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[0]) << 16);
#else
			return pPixel[0] + (((uint32_t)pPixel[1]) << 8) + (((uint32_t)pPixel[2]) << 16);
#endif
		case 32:
			return *((uint32_t*)pPixel);
		default:
			while (true);	// Error: unsupported pixel format.
		}
	}


	//____ alpha() ____________________________________________________________

	uint8_t StreamSurface::alpha( Coord coord ) const
	{
		if (m_pixelFormat.A_bits == 0)
			return 255;

		if (m_pAlphaLayer)
		{
			return m_pAlphaLayer[coord.x + coord.y*m_size.w];
		}
		else
		{
			return ((uint8_t*)m_pBlob->data())[coord.x*m_pixelFormat.bits / 8 + coord.y*m_pitch];
		}
	}

	//____ _sendCreateSurface() _______________________________________________

	short StreamSurface::_sendCreateSurface(Size size, PixelType type)
	{
		short surfaceId = m_pStream->allocObjectId();

		*m_pStream << GfxStream::Header{ GfxChunkId::CreateSurface, 8 };
		*m_pStream << surfaceId;
		*m_pStream << (short)type;
		*m_pStream << size;

		return surfaceId;
	}

	//____ _genAlphaLayer() _________________________________________________________

	uint8_t * StreamSurface::_genAlphaLayer(const char * pSource, int pitch)
	{
		uint8_t * pAlphaLayer = new uint8_t[m_size.w*m_size.h];

		//TODO: Support more than 8 bit alpha.

		uint8_t * pDest = m_pAlphaLayer;
		int pixelPitch = m_pixelFormat.bits / 8;
		for (int y = 0; y < m_size.h; y++)
		{
			const char * pPixel = pSource + y*pitch;

			for (int x = 0; x < m_size.w; x++)
			{
				*pDest++ = *pPixel;
				pPixel += pixelPitch;
			}
		}
		return pAlphaLayer;
	}

	//____ _sendPixels() _________________________________________________________

	void StreamSurface::_sendPixels(Rect rect, const uint8_t * pSource, int pitch)
	{
		*m_pStream << GfxStream::Header{ GfxChunkId::BeginSurfaceUpdate, 10 };
		*m_pStream << m_inStreamId;
		*m_pStream << rect;

		int	pixelSize = m_pixelFormat.bits / 8;
		int dataSize = rect.w * rect.h * pixelSize;

		const uint8_t * pLine = pSource;
		int ofs = 0;				// Offset in bytes within the current line.

		while( dataSize > 0 )
		{
			int chunkSize = min(dataSize, (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)));
			dataSize -= chunkSize;

			*m_pStream << GfxStream::Header{ GfxChunkId::SurfaceData, chunkSize };

			while (chunkSize > 0)
			{
				int len = rect.w * pixelSize - ofs;
				if (chunkSize < len)
				{
					*m_pStream << GfxStream::DataChunk{ chunkSize, (void *) (pLine + ofs) };
					ofs += chunkSize;
					chunkSize = 0;
				}
				else
				{
					*m_pStream << GfxStream::DataChunk{ len, (void *)(pLine + ofs) };
					ofs = 0;
					chunkSize -= len;
					pLine += pitch;
				}
			}
		}

		*m_pStream << GfxStream::Header{ GfxChunkId::EndSurfaceUpdate, 0 };
	}

	//____ _sendDeleteSurface() _______________________________________________

	void StreamSurface::_sendDeleteSurface()
	{
		*m_pStream << GfxStream::Header{ GfxChunkId::CreateSurface, 2 };
		*m_pStream << m_inStreamId;
	}

} // namespace wg
