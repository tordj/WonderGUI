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
		static const int	c_maxBlockSize = 4096;		// Includes the block headers!

		struct Header
		{
			GfxChunkId	type;
			int			size;
		};

		struct DataChunk
		{
			int				bytes;
			const void *	pBuffer;
		};


	};

};


#endif //WG_GFXSTREAM_DOT_H
