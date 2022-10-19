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

#ifndef WG_SURFACEFILEHEADER_DOT_H
#define WG_SURFACEFILEHEADER_DOT_H
#pragma once

#include <cstdint>
#include <wg_types.h>
#include <wg_geo.h>

namespace wg
{

//____ wgsf_header ____________________________________________________________

struct SurfaceFileHeader
{
	char 			header[4]				= { 'S', 'U', 'R', 'F' };
	int16_t			versionNumber			= 1;
	int16_t			headerBytes				= 0;
	int32_t			clutDataBytes			= 0;
	int32_t			imageDataBytes			= 0;
	int32_t			extraDataBytes			= 0;

	int32_t			width					= 0;
	int32_t			height					= 0;
	PixelFormat		format					= PixelFormat::Undefined;
	SampleMethod	sampleMethod			= SampleMethod::Undefined;

	union
	{
		uint16_t	flags					= 0;
		struct
		{
			uint16_t		buffered: 1;
			uint16_t		canvas: 1;
			uint16_t		dynamic: 1;
			uint16_t		tiling: 1;
			uint16_t		mipmap: 1;
		};
	};


	//	--- 32 bytes header ends here

	int16_t			reserved1				= 0;
	int16_t			scale					= 64;
	int32_t			identity				= 0;

	//	-- 40 bytes header ends here.

	char			pixelFiltering[4]		= { 'N','O','N','E' };
	int16_t			filterBlocksX			= 1;
	int16_t			filterBlocksY			= 1;
	char			pixelCompression[4]		= { 'N','O','N','E' };
	int16_t			pixelDecompressMargin	= 0;
	int16_t			reserved2				= 0;
	
	//	-- 56 bytes header ends here.

	int32_t			clutEntries 			= 0;
	char			clutFiltering[4]		= { 'N','O','N','E' };
	char 			clutCompression[4]	= { 'N','O','N','E' };
	char			clutFilteringParams[8] = { 0,0,0,0,0,0,0,0 };
	int16_t			clutDecompressMargin	= 0;
	int16_t			reserved3				= 0;

	//	-- 80 bytes header ends here.

	char			extraDataCompression[4] = { 'N','O','N','E' };
	int16_t			extraDataDecompressMargin = 0;
	int16_t			reserved4				= 0;

	//	-- 88 bytes header ends here.

};

} //namespace

#endif //WG_SURFACEFILEHEADER_DOT_H
