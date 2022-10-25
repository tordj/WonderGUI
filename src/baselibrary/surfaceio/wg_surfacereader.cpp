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

#include <wg_surfacereader.h>
#include <wg_base.h>

namespace wg
{
	const TypeInfo SurfaceReader::TYPEINFO = { "SurfaceReader", &Object::TYPEINFO };


	//.____ create() _____________________________________________________________

	SurfaceReader_p SurfaceReader::create( const Blueprint& blueprint )
	{
		return SurfaceReader_p( new SurfaceReader(blueprint) );
	}
	
	//____ constructor ___________________________________________________________

	SurfaceReader::SurfaceReader( const Blueprint& bp )
	{
		m_pFactory = bp.factory;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SurfaceReader::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ readSurfaceFromStream() _____________________________________________

	Surface_p SurfaceReader::readSurfaceFromStream( std::istream& stream )
	{
		SurfaceFileHeader	header;
		
		// Read the header
		
		stream.read( (char*) &header, 8 );
		stream.read( ((char*)(&header))+8, header.headerBytes - 8);
		
		// Prepare surface blueprint
		
		Surface::Blueprint bp = _blueprintFromHeader(&header);
		
		// Read and prepare CLUT
		
		int clutBytes = header.paletteEntries*sizeof(Color8) + header.paletteDecompressMargin;
		Color8 * pClut = nullptr;
		if( clutBytes > 0 )
		{
			pClut = (Color8*) Base::memStackAlloc(clutBytes);

			// We only support uncompressed CLUT for the moment
			
			stream.read( (char*) pClut, clutBytes );
						
			bp.clut = pClut;
		}
		
		// Create surface
		
		auto pSurface = m_pFactory->createSurface(bp);
		
		if( clutBytes > 0 )
			Base::memStackRelease(clutBytes);
		
		// Read pixels into PixelBuffer
		// We only support uncompressed pixels for the moment
		
		auto pixbuf = pSurface->allocPixelBuffer();

		
		int lineBytes = header.width * pSurface->pixelBytes();
		
		if( pixbuf.pitch > lineBytes )
		{
			// Pitch is involved, we need to read line by line
			
			char * pPixels = (char *) pixbuf.pPixels;
			
			for( int y = 0 ; y < header.height ; y++ )
			{
				stream.read( pPixels, lineBytes );
				pPixels += pixbuf.pitch;
			}
		}
		else
		{
			stream.read( (char*) pixbuf.pPixels, lineBytes * header.height );
		}
		
		pSurface->pullPixels(pixbuf);
		pSurface->freePixelBuffer(pixbuf);

		return pSurface;
	}

	//____ readSurfaceFromBlob() _______________________________________________

	Surface_p SurfaceReader::readSurfaceFromBlob( const Blob * pBlob )
	{
		return readSurfaceFromMemory( static_cast<const char*>(pBlob->data()) );
	}

	//____ readSurfaceFromMemory() _____________________________________________

	Surface_p SurfaceReader::readSurfaceFromMemory( const char * pData )
	{
		SurfaceFileHeader	header;
		 		 
		// Read the header

		int headerSize = * (const int16_t*)&pData[6];
		std::memcpy( &header, pData, headerSize);
		pData+= headerSize;
		 
		// Prepare surface blueprint
		 
		Surface::Blueprint bp = _blueprintFromHeader(&header);
		 
		// Prepare CLUT
		 
		int clutBytes = header.paletteEntries*sizeof(Color8);
		if( clutBytes > 0 )
		{
			bp.clut = (Color8*) pData;
			pData += clutBytes;
		}
		 
		// Create surface
		 
		auto pSurface = m_pFactory->createSurface(bp);
		 		 
		// Read pixels into PixelBuffer
		// We only support uncompressed pixels for the moment
		 
		auto pixbuf = pSurface->allocPixelBuffer();

		int lineBytes = header.width * pSurface->pixelBytes();
		 
		if( pixbuf.pitch > lineBytes )
		{
			// Pitch is involved, we need to read line by line
			 
			char * pPixels = (char *) pixbuf.pPixels;
			 
			for( int y = 0 ; y < header.height ; y++ )
			{
				std::memcpy( pPixels, pData, lineBytes );
				pData += lineBytes;
				pPixels += pixbuf.pitch;
			}
		}
		else
		{
			std::memcpy( pixbuf.pPixels, pData, lineBytes * header.height );
		}
		 
		pSurface->pullPixels(pixbuf);
		pSurface->freePixelBuffer(pixbuf);

		return pSurface;
	}


//____ _blueprintFromHeader() _________________________________________________

Surface::Blueprint SurfaceReader::_blueprintFromHeader( const SurfaceFileHeader * pHeader )
{
	return WGBP(Surface,
				_.size 			= {pHeader->width, pHeader->height},
				_.scale 		= pHeader->scale,
				_.format 		= pHeader->format,
				_.buffered 		= pHeader->buffered,
				_.canvas 		= pHeader->canvas,
				_.dynamic 		= pHeader->dynamic,
				_.mipmap 		= pHeader->mipmap,
				_.identity		= pHeader->identity,
				_.tiling		= pHeader->tiling,
				_.sampleMethod 	= pHeader->sampleMethod );
}


} // namespace wg



