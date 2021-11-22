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

#include <wg_cgfxinstream.h>
#include <assert.h>

namespace wg
{
	const TypeInfo CGfxInStream::TYPEINFO = { "CGfxInStream", &Component::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CGfxInStream::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isEmpty() __________________________________________________________

	bool CGfxInStream::isEmpty()
	{
		return !m_pHolder->_hasChunk();
	}

	//____ peek() _____________________________________________________________

	GfxStream::Header CGfxInStream::peek()
	{
		if (m_pHolder->_hasChunk())
			return m_pHolder->_peekChunk();

		return { GfxChunkId::OutOfData, 0 };
	}



	//____ operator>> _________________________________________________________

	CGfxInStream& CGfxInStream::operator>> (GfxStream::Header& header)
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

	CGfxInStream& CGfxInStream::operator>> (int16_t& i)
	{
		i = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (uint16_t& i)
	{
		i = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (int32_t& i)
	{
		i = m_pHolder->_pullInt();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (float& f)
	{
		f = m_pHolder->_pullFloat();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (CoordI& coord)
	{
		coord.x = m_pHolder->_pullShort();
		coord.y = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (CoordF& coord)
	{
		coord.x = m_pHolder->_pullFloat();
		coord.y = m_pHolder->_pullFloat();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (SizeI& sz)
	{
		sz.w = m_pHolder->_pullShort();
		sz.h = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (SizeF& sz)
	{
		sz.w = m_pHolder->_pullFloat();
		sz.h = m_pHolder->_pullFloat();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (RectI& rect)
	{
		rect.x = m_pHolder->_pullShort();
		rect.y = m_pHolder->_pullShort();
		rect.w = m_pHolder->_pullShort();
		rect.h = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (RectF& rect)
	{
		rect.x = m_pHolder->_pullFloat();
		rect.y = m_pHolder->_pullFloat();
		rect.w = m_pHolder->_pullFloat();
		rect.h = m_pHolder->_pullFloat();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (HiColor& color)
	{
		color.a= m_pHolder->_pullShort();
		color.r = m_pHolder->_pullShort();
		color.g = m_pHolder->_pullShort();
		color.b = m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (Direction& dir)
	{
		dir = (Direction)m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (BlendMode& blendMode)
	{
		blendMode = (BlendMode) m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (Axis& o)
	{
		o = (Axis)m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (PixelFormat& t)
	{
		t = (PixelFormat)m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (ScaleMode& m)
	{
		m = (ScaleMode)m_pHolder->_pullShort();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (const DataChunk& data)
	{
		m_pHolder->_pullBytes(data.bytes, (char*) data.pBuffer);
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (int mtx[2][2])
	{
		mtx[0][0] = m_pHolder->_pullChar();
		mtx[0][1] = m_pHolder->_pullChar();
		mtx[1][0] = m_pHolder->_pullChar();
		mtx[1][1] = m_pHolder->_pullChar();
		return *this;
	}

	CGfxInStream& CGfxInStream::operator>> (float mtx[2][2])
	{
		mtx[0][0] = m_pHolder->_pullFloat();
		mtx[0][1] = m_pHolder->_pullFloat();
		mtx[1][0] = m_pHolder->_pullFloat();
		mtx[1][1] = m_pHolder->_pullFloat();
		return *this;
	}


} // namespace wg
