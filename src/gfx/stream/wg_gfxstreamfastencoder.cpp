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

#include <wg_gfxstreamfastencoder.h>

#include <algorithm>

namespace wg
{
	const TypeInfo GfxStreamFastEncoder::TYPEINFO = { "GfxStreamFastEncoder", &Object::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamFastEncoder::GfxStreamFastEncoder(const GfxStreamSink_p& pStream, int bufferBytes) : GfxStreamEncoder(pStream)
	{
		m_pBuffer = new uint8_t[bufferBytes];
		m_capacity = bufferBytes;
		m_pWriteData = m_pBuffer;
	}

	//____ destructor ________________________________________________________

	GfxStreamFastEncoder::~GfxStreamFastEncoder()
	{
		delete[] m_pBuffer;
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamFastEncoder::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ flush() ____________________________________________________________

	void GfxStreamFastEncoder::flush()
	{
		if (m_pStream)
			m_pStream->processChunks( m_pBuffer, m_pWriteData);

		m_pWriteData = m_pBuffer;
	}

	//____ _beginChunk() _________________________________________________________

	void GfxStreamFastEncoder::_beginChunk(GfxStream::Header header)
	{
		if (header.size <= 30)
		{
			if (m_pWriteData + (header.size + 2 ) - m_pBuffer > m_capacity )
				flush();

			_pushChar((char)header.type);
			_pushChar(header.size + (header.spxFormat << 5));
		}
		else
		{
			if (m_pWriteData + (header.size + 4 ) - m_pBuffer > m_capacity )
				flush();

			_pushChar((char)header.type);
			_pushChar(31  + (header.spxFormat << 5));
			_pushShort((short)header.size);
		}
	}
}