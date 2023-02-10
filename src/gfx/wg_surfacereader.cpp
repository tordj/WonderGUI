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
#include <wg_gfxbase.h>
#include <cstring>

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

	Surface_p SurfaceReader::readSurfaceFromStream(std::istream& stream)
	{
		Surface::Blueprint dummy;
		return readSurfaceFromStream(stream, dummy);
	}

	Surface_p SurfaceReader::readSurfaceFromStream(std::istream& stream, const Surface::Blueprint& _bp)
	{
		SurfaceFileHeader	header;

		// Read the header

		stream.read( (char*) &header, 8 );
		stream.read( ((char*)(&header))+8, header.headerBytes - 8);

		// Prepare surface blueprint

		Surface::Blueprint bp = _blueprintFromHeader(&header);
		if (_addFlagsFromOtherBlueprint(bp, _bp) != 0)
		{
			GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::InvalidParam, "Provided blueprint can not alter format, scale or palette of loaded surface but have one or more of these parameters set.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

		// Read and prepare palette

		int paletteBytes = header.paletteEntries*sizeof(Color8) + header.paletteDecompressMargin;
		Color8 * pPalette = nullptr;
		if( paletteBytes > 0 )
		{
			pPalette = (Color8*) GfxBase::memStackAlloc(paletteBytes);

			// We only support uncompressed palette for the moment

			stream.read( (char*) pPalette, paletteBytes );

			bp.palette = pPalette;
		}

		// Create surface

		auto pSurface = m_pFactory->createSurface(bp);

		if( paletteBytes > 0 )
			GfxBase::memStackRelease(paletteBytes);

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

	Surface_p SurfaceReader::readSurfaceFromBlob(const Blob* pBlob)
	{
		return readSurfaceFromMemory(static_cast<const char*>(pBlob->data()));
	}

	Surface_p SurfaceReader::readSurfaceFromBlob( const Blob * pBlob, const Surface::Blueprint& bp )
	{
		return readSurfaceFromMemory( static_cast<const char*>(pBlob->data()), bp);
	}

	//____ readSurfaceFromMemory() _____________________________________________

	Surface_p SurfaceReader::readSurfaceFromMemory(const char* pData)
	{
		Surface::Blueprint dummy;
		return readSurfaceFromMemory(pData, dummy);
	}

	Surface_p SurfaceReader::readSurfaceFromMemory(const char* pData, const Surface::Blueprint& _bp)
	{
		SurfaceFileHeader	header;

		// Read the header

		int headerSize = * (const int16_t*)&pData[6];
		std::memcpy( &header, pData, headerSize);
		pData+= headerSize;

		// Prepare surface blueprint

		Surface::Blueprint bp = _blueprintFromHeader(&header);
		if (_addFlagsFromOtherBlueprint(bp, _bp) != 0)
		{
			GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::InvalidParam, "Provided blueprint can not alter format, scale or palette of loaded surface but have one or more of these parameters set.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}


		// Prepare palette

		int paletteBytes = header.paletteEntries*sizeof(Color8);
		if( paletteBytes > 0 )
		{
			bp.palette = (Color8*) pData;
			pData += paletteBytes;
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

//____ _addFlagsFromOtherBlueprint() __________________________________________

int SurfaceReader::_addFlagsFromOtherBlueprint(Surface::Blueprint& dest, const Surface::Blueprint& extraFlags)
{
	int errorCode = 0;

	if ( extraFlags.buffered )
		dest.buffered = true;

	if (extraFlags.canvas)
		dest.canvas = true;

	if (extraFlags.palette)
		errorCode = 1;

	if (extraFlags.dynamic)
		dest.dynamic = true;

	if (extraFlags.format != PixelFormat::Undefined)
		errorCode = 2;

	if (extraFlags.identity != 0)
		dest.identity = extraFlags.identity;

	if (extraFlags.mipmap)
		dest.mipmap = true;

	if (extraFlags.sampleMethod != SampleMethod::Undefined)
		dest.sampleMethod = extraFlags.sampleMethod;

	if (extraFlags.scale != 64)
		errorCode = 3;

	if (extraFlags.tiling)
		dest.tiling = true;

	return errorCode;
}




} // namespace wg



