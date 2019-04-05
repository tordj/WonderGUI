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

	GfxInStream& GfxInStream::operator>> (int16_t& i)
	{
		i = m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (uint16_t& i)
	{
		i = m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (int32_t& i)
	{
		i = m_pHolder->_pullInt();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (float& f)
	{
		f = m_pHolder->_pullFloat();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Coord& coord)
	{
		coord.x = m_pHolder->_pullShort();
		coord.y = m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (CoordF& coord)
	{
		coord.x = m_pHolder->_pullFloat();
		coord.y = m_pHolder->_pullFloat();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Size& sz)
	{
		sz.w = m_pHolder->_pullShort();
		sz.h = m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (SizeF& sz)
	{
		sz.w = m_pHolder->_pullFloat();
		sz.h = m_pHolder->_pullFloat();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Rect& rect)
	{
		rect.x = m_pHolder->_pullShort();
		rect.y = m_pHolder->_pullShort();
		rect.w = m_pHolder->_pullShort();
		rect.h = m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (RectF& rect)
	{
		rect.x = m_pHolder->_pullFloat();
		rect.y = m_pHolder->_pullFloat();
		rect.w = m_pHolder->_pullFloat();
		rect.h = m_pHolder->_pullFloat();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Color& color)
	{
		color.argb = m_pHolder->_pullInt();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Direction& dir)
	{
		dir = (Direction)m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (BlendMode& blendMode)
	{
		blendMode = (BlendMode) m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (Orientation& o)
	{
		o = (Orientation)m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (PixelFormat& t)
	{
		t = (PixelFormat)m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (ScaleMode& m)
	{
		m = (ScaleMode)m_pHolder->_pullShort();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (const DataChunk& data)
	{
		m_pHolder->_pullBytes(data.bytes, (char*) data.pBuffer);
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (int mtx[2][2])
	{
		mtx[0][0] = m_pHolder->_pullChar();
		mtx[0][1] = m_pHolder->_pullChar();
		mtx[1][0] = m_pHolder->_pullChar();
		mtx[1][1] = m_pHolder->_pullChar();
		return *this;
	}

	GfxInStream& GfxInStream::operator>> (float mtx[2][2])
	{
		mtx[0][0] = m_pHolder->_pullFloat();
		mtx[0][1] = m_pHolder->_pullFloat();
		mtx[1][0] = m_pHolder->_pullFloat();
		mtx[1][1] = m_pHolder->_pullFloat();
		return *this;
	}


} // namespace wg
