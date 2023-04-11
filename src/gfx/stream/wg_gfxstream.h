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

#include <wg_gfxtypes.h>

namespace wg
{


	class GfxStream
	{
	public:
		static constexpr int	c_maxBlockSize = 1024+256;		// Includes the block headers!	Must be at least 4096+14+block_header_size due to palette possibly included in CreateSurface-chunks.

		static constexpr int	c_maxClipRects = 256;		// Largest number of patches allowed for a drawing primitive.

		struct SPX
		{
			SPX() {}
			SPX(spx v) : value(v) {}
			
			operator spx() const {return value;}
			
			spx value;
		};

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


		class Chunk
		{
		public:

			GfxChunkId	type() const { return m_type; }

			int			chunkSize() const
			{
				uint8_t sizeEtc = m_flags_and_size & 0x1F;
				if (sizeEtc <= 30)
					return sizeEtc + 2;
				else
					return ((uint16_t*)this)[1] + 4;
			}

			int			dataSize() const
			{
				uint8_t sizeEtc = m_flags_and_size & 0x1F;
				if (sizeEtc <= 30)
					return sizeEtc;
				else
					return ((uint16_t*)this)[1] + 4;
			}

			void*		data()
			{
				if ( (m_flags_and_size & 0x1F) <= 30)
					return ((uint8_t*)this) + 2;
				else
					return ((uint8_t*)this) + 2;

			}

			Chunk *		next() const
			{
				uint8_t sizeEtc = m_flags_and_size & 0x1F;
				if (sizeEtc <= 30)
					return (Chunk*) (((uint8_t*)this) + sizeEtc + 2);
				else
					return (Chunk*) (((uint8_t*)this) + ((uint16_t*)this)[1] + 4);
			}


		protected:
			GfxChunkId	m_type;
			uint8_t		m_flags_and_size;
		};



		class iterator
		{
		public:
			iterator(void* pChunk) : m_pChunk((Chunk*)pChunk) {}


			inline Chunk operator*() const
			{
				return *m_pChunk;
			}

			inline Chunk* operator->() const
			{
				return m_pChunk;
			}

			inline iterator& operator++()
			{
				m_pChunk = m_pChunk->next();
				return *this;
			}

			inline iterator operator++(int)
			{
				iterator it = *this;
				m_pChunk = m_pChunk->next();
				return it;
			}

			inline bool operator==(const iterator& rhs) const
			{
				return m_pChunk == rhs.m_pChunk;
			}

			inline bool operator!=(const iterator& rhs) const
			{
				return m_pChunk != rhs.m_pChunk;
			}

			inline bool operator< (const iterator& rhs) const { return m_pChunk < rhs.m_pChunk; }
			inline bool operator> (const iterator& rhs) const { return m_pChunk > rhs.m_pChunk; }
			inline bool operator<=(const iterator& rhs) const { return m_pChunk <= rhs.m_pChunk; }
			inline bool operator>=(const iterator& rhs) const { return m_pChunk >= rhs.m_pChunk; }

			inline operator uint8_t*() const
			{
				return (uint8_t*) m_pChunk;
			}


		protected:
			Chunk* m_pChunk;

			virtual void	_inc() {};
	
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

		inline static int headerSize(const uint8_t* pChunk)
		{
			uint8_t sizeEtc = pChunk[1] & 0x1F;
			if (sizeEtc <= 30)
				return 2;
			else
				return 4;
		}

		inline static int dataSize(const uint8_t* pChunk)
		{
			uint8_t sizeEtc = pChunk[1] & 0x1F;
			if (sizeEtc <= 30)
				return sizeEtc;
			else
				return *(uint16_t*)&pChunk[2];
		}
		
		inline static int spxSize(int spxFormat)
		{
			static const int size[8] = {4,2,2,1,2,2,1,1};
		
			return size[spxFormat];
		}
	};

};


#endif //WG_GFXSTREAM_DOT_H
