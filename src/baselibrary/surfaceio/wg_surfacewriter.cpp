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

#include <wg_surfacewriter.h>
#include <wg_base.h>

namespace wg
{
	const TypeInfo SurfaceWriter::TYPEINFO = { "SurfaceWriter", &Object::TYPEINFO };


	//.____ create() _____________________________________________________________

	SurfaceWriter_p SurfaceWriter::create( const Blueprint& blueprint )
	{
		return SurfaceWriter_p( new SurfaceWriter(blueprint) );
		
	}
	
	//____ constructor ___________________________________________________________

	SurfaceWriter::SurfaceWriter( const Blueprint& bp )
	{
		m_saveInfo = bp.saveInfo;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SurfaceWriter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ writeSurfaceToStream() _____________________________________________

	void SurfaceWriter::writeSurfaceToStream( std::ostream& stream, Surface * pSurface, int extraDataSize, char * pExtraData )
	{
		SurfaceFileHeader	header;
		
		auto bp = pSurface->blueprint();
		
		
		auto sz = pSurface->pixelSize();
		
		header.width = bp.size.w;
		header.height = bp.size.h;
		header.format = bp.format;

		if( m_saveInfo.sampleMethod )
			header.sampleMethod = bp.sampleMethod;
		
		if( m_saveInfo.buffered )
			header.buffered = bp.buffered;
		
		if( m_saveInfo.canvas )
			header.canvas = bp.canvas;
		
		if( m_saveInfo.dynamic )
			header.dynamic = bp.dynamic;
		
		if( m_saveInfo.tiling )
			header.tiling = bp.tiling;
		
		if( m_saveInfo.mipmap )
			header.mipmap = bp.mipmap;

		if( m_saveInfo.scale )
			header.scale = bp.scale;
		
		if( m_saveInfo.identity )
			header.identity = bp.identity;
		
		if( bp.clut )
		{
			header.clutDataBytes = 256*sizeof(Color8);
			header.clutEntries = 256;
		}
		

		int headerBytes;
		if( extraDataSize != 0 )
			headerBytes = 88;
		else if( bp.clut )
			headerBytes = 80;
		else if( (m_saveInfo.scale && bp.scale != 64) || (m_saveInfo.identity && bp.identity != 0) )
			headerBytes = 40;
		else
			headerBytes = 32;
		
		header.headerBytes = headerBytes;
		
		int lineBytes = bp.size.w * pSurface->pixelBytes();

		header.imageDataBytes = lineBytes * bp.size.h;
		header.extraDataBytes = extraDataSize;
		
		// Write header

		stream.write((char*) &header, headerBytes);

		// Write pixels
		
		auto pixbuf = pSurface->allocPixelBuffer();
		pSurface->pushPixels(pixbuf);
		
		if( pixbuf.pitch > lineBytes )
		{
			// Pitch is involved, we need to write line by line
			
			char * pPixels = (char *) pixbuf.pPixels;
			
			for( int y = 0 ; y < header.height ; y++ )
			{
				stream.write( pPixels, lineBytes );
				pPixels += pixbuf.pitch;
			}
		}
		else
		{
			stream.write( (char*) pixbuf.pPixels, lineBytes * header.height );
		}

		pSurface->freePixelBuffer(pixbuf);

		// write clut
		
		if( bp.clut )
			stream.write( (char*) bp.clut, 256*sizeof(Color8) );

		// write extra data
		
		if( extraDataSize > 0 )
			stream.write( pExtraData, extraDataSize );
	}

} // namespace wg









