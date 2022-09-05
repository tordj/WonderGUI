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

#ifndef	WG_GFXSTREAMBUFFER_DOT_H
#define	WG_GFXSTREAMBUFFER_DOT_H
#pragma once

#include <wg_gfxstream.h>
#include <wg_gfxstreamsource.h>
#include <wg_gfxstreamsink.h>

#include <vector>
#include <tuple>

namespace wg
{

	class GfxStreamBuffer;
	typedef	StrongPtr<GfxStreamBuffer>	GfxStreamBuffer_p;
	typedef	WeakPtr<GfxStreamBuffer>	GfxStreamBuffer_wp;

	class GfxStreamBuffer : public Object, protected GfxStreamSink::Holder, protected GfxStreamSource::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamBuffer_p	create( int initialCapacity = GfxStream::c_maxBlockSize * 4 );

		//.____ Components _______________________________________

		GfxStreamSink		input;
		GfxStreamSource	output;

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _____________________________________________________
		
		inline int	bufferCapacity() const { return m_bufferSize; };
		inline bool	chunkInBuffer() const { return _hasStreamChunks(); };
		int			bytesInBuffer() const;
		
	protected:

		GfxStreamBuffer( int initialCapacity );
		~GfxStreamBuffer();

		void			_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) override;

		bool			_hasStreamChunks() const override;
		std::tuple<int, const DataSegment*> _showStreamChunks() override;
		GfxChunkId 		_peekStreamChunk() override;
		void			_discardStreamChunks(int bytes) override;
		bool			_fetchStreamChunks() override;

		void			_resizeBuffer( int newSize );

		static constexpr int c_bufferMargin = GfxStream::c_maxBlockSize;

		char*		m_pBuffer;
		int			m_bufferSize;
		int			m_readOfs;
		int			m_writeOfs;

		int			m_processedOfs;			// Offset of end of full chunks. 
		int			m_bufferOverflow;		// How much processed data overflows the buffer with in order to hold a
											// complete chunk. Overflowing data is a copy of data from beginning of buffer.

		DataSegment	m_dataSegments[2];
		
	};


} // namespace wg
#endif //WG_GFXSTREAMBUFFER_DOT_H
