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

#ifndef	WG_GFXSTREAM_DOT_H
#define	WG_GFXSTREAM_DOT_H
#pragma once

#include <wg_types.h>

namespace wg
{


	class GfxStream
	{
	public:
		static constexpr int	c_maxBlockSize = 1024+256;		// Includes the block headers!	Must be at least 4096+14+block_header_size due to CLUT possibly included in CreateSurface-chunks.

		static constexpr int	c_maxClipRects = 256;		// Largest number of patches allowed for a drawing primitive.


		struct Header
		{
			GfxChunkId      type;
            uint8_t			spxFormat;
    		int				size;
		};

		struct DataChunk
		{
			int				bytes;
			const void *	pBuffer;
		};



		inline static GfxChunkId chunkType(const uint8_t* pChunk)
		{
			return (GfxChunkId) pChunk[0];
		}

		inline static int chunkSize(const uint8_t* pChunk)
		{
			uint8_t sizeEtc = pChunk[1] & 0x1F;
			if (sizeEtc <= 30)
				return sizeEtc + 2;
			else
				return *(uint16_t*)&pChunk[2] + 4;
		}
	};

};


#endif //WG_GFXSTREAM_DOT_H
