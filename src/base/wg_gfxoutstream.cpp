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

#include <wg_gfxoutstream.h>
#include <assert.h>

namespace wg 
{

	//____ Constructor ________________________________________________________

	GfxOutStream::GfxOutStream(GfxOutStreamHolder * pHolder) : 
		m_pHolder(pHolder), 
		m_idCounter(1), 
		m_pFreeIdStack(nullptr),
		m_freeIdStackCapacity(0),
		m_freeIdStackSize(0)
	{
	}


	//____ operator<< _________________________________________________________

	GfxOutStream&  GfxOutStream::operator<< (GfxStream::Header header)
	{
		m_pHolder->_reserveStream(header.size + 4);
		m_pHolder->_pushShort((short)header.type);
		m_pHolder->_pushShort((short)header.size);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (int16_t int16)
	{
		m_pHolder->_pushShort(int16);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (uint16_t uint16)
	{
		m_pHolder->_pushShort(uint16);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (int32_t int32)
	{
		m_pHolder->_pushInt(int32);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (float f)
	{
		m_pHolder->_pushFloat(f);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (const Coord& c)
	{
		m_pHolder->_pushShort(c.x);
		m_pHolder->_pushShort(c.y);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (const Size& sz)
	{
		m_pHolder->_pushShort(sz.w);
		m_pHolder->_pushShort(sz.h);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (const Rect& rect)
	{
		m_pHolder->_pushShort(rect.x);
		m_pHolder->_pushShort(rect.y);
		m_pHolder->_pushShort(rect.w);
		m_pHolder->_pushShort(rect.h);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (const RectF& rect)
	{
		m_pHolder->_pushFloat(rect.x);
		m_pHolder->_pushFloat(rect.y);
		m_pHolder->_pushFloat(rect.w);
		m_pHolder->_pushFloat(rect.h);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (Direction d)
	{
		m_pHolder->_pushShort((short)d);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (BlendMode b)
	{
		m_pHolder->_pushShort((short) b);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (Orientation o)
	{
		m_pHolder->_pushShort((short)o);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (PixelFormat t)
	{
		m_pHolder->_pushShort((short)t);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (ScaleMode m)
	{
		m_pHolder->_pushShort((short)m);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (Color color)
	{
		m_pHolder->_pushInt(color.argb);
		return *this;
	}

	GfxOutStream& GfxOutStream::operator<< (const DataChunk& data)
	{
		m_pHolder->_pushBytes(data.bytes, (char*)data.pBuffer);
		return *this;
	}


	//____ allocObjectId() ____________________________________________________

	short GfxOutStream::allocObjectId()
	{
		if (m_freeIdStackSize > 0)
			return m_pFreeIdStack[--m_freeIdStackSize];

		return m_idCounter++;
	}

	//____ freeObjectId() _____________________________________________________

	void GfxOutStream::freeObjectId(short id)
	{
		if (m_freeIdStackSize == m_freeIdStackCapacity)
		{
			int capacity = min(16, m_freeIdStackCapacity * 2);
			short * pBuffer = new short[capacity];

			for (int i = 0; i < m_freeIdStackSize; i++)
				pBuffer[i] = m_pFreeIdStack[i];

			delete[] m_pFreeIdStack;

			m_pFreeIdStack = pBuffer;
			m_freeIdStackCapacity = capacity;
		}

		m_pFreeIdStack[m_freeIdStackSize++] = id;
	}



} // namespace wg
