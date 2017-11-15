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

#include <wg_gfxinstream.h>
#include <assert.h>

namespace wg 
{
	//____ isEmpty() __________________________________________________________

	bool GfxInStream::isEmpty()
	{
		return !m_pHolder->_hasChunk();
	}

	//____ peek() _____________________________________________________________

	GfxStream::Header GfxInStream::peek()
	{
		if (m_pHolder->_hasChunk())
			return m_pHolder->_peekChunk();

		return { GfxChunkId::OutOfData, 0 };
	}

	//____ operator>> _________________________________________________________

	GfxInStream& GfxInStream::operator>> (GfxStream::Header& header)
	{
		if (m_pHolder->_hasChunk())
		{
			header.type = (GfxChunkId)m_pHolder->_pullShort();
			header.size = (uint16_t)m_pHolder->_pullShort();
		}
		else
		{
			header.type = GfxChunkId::OutOfData;
			header.size = 0;
		}

		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Rect& rect)
	{
		rect.x = m_pHolder->_pullInt();
		rect.y = m_pHolder->_pullInt();
		rect.w = m_pHolder->_pullInt();
		rect.h = m_pHolder->_pullInt();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Color& color)
	{
		color.argb = m_pHolder->_pullInt();
		return *this;
	}



} // namespace wg
