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

#include <wg_cgfxoutstream.h>
#include <assert.h>

namespace wg
{

	const TypeInfo CGfxOutStream::TYPEINFO = { "CGfxOutStream", &Component::TYPEINFO };

	//____ constructor ________________________________________________________

	CGfxOutStream::CGfxOutStream(CGfxOutStream::Holder * pHolder) :
		m_pHolder(pHolder),
		m_idCounter(1),
		m_pFreeIdStack(nullptr),
		m_freeIdStackCapacity(0),
		m_freeIdStackSize(0)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CGfxOutStream::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ operator<< _________________________________________________________

	CGfxOutStream&  CGfxOutStream::operator<< (GfxStream::Header header)
	{
		m_pHolder->_reserveStream(header.size + 4);
		m_pHolder->_pushShort((short)header.type);
		m_pHolder->_pushShort((short)header.size);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (int16_t int16)
	{
		m_pHolder->_pushShort(int16);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (uint16_t uint16)
	{
		m_pHolder->_pushShort(uint16);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (int32_t int32)
	{
		m_pHolder->_pushInt(int32);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (float f)
	{
		m_pHolder->_pushFloat(f);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const CoordI& c)
	{
		m_pHolder->_pushShort(c.x);
		m_pHolder->_pushShort(c.y);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const CoordF& c)
	{
		m_pHolder->_pushFloat(c.x);
		m_pHolder->_pushFloat(c.y);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const SizeI& sz)
	{
		m_pHolder->_pushShort(sz.w);
		m_pHolder->_pushShort(sz.h);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const SizeF& sz)
	{
		m_pHolder->_pushFloat(sz.w);
		m_pHolder->_pushFloat(sz.h);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const RectI& rect)
	{
		m_pHolder->_pushShort(rect.x);
		m_pHolder->_pushShort(rect.y);
		m_pHolder->_pushShort(rect.w);
		m_pHolder->_pushShort(rect.h);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (const RectF& rect)
	{
		m_pHolder->_pushFloat(rect.x);
		m_pHolder->_pushFloat(rect.y);
		m_pHolder->_pushFloat(rect.w);
		m_pHolder->_pushFloat(rect.h);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (Direction d)
	{
		m_pHolder->_pushShort((short)d);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (BlendMode b)
	{
		m_pHolder->_pushShort((short) b);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (Orientation o)
	{
		m_pHolder->_pushShort((short)o);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (PixelFormat t)
	{
		m_pHolder->_pushShort((short)t);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (ScaleMode m)
	{
		m_pHolder->_pushShort((short)m);
		return *this;
	}

	CGfxOutStream&  CGfxOutStream::operator<< (Color color)
	{
		m_pHolder->_pushInt(color.argb);
		return *this;
	}

	CGfxOutStream& CGfxOutStream::operator<< (const DataChunk& data)
	{
		m_pHolder->_pushBytes(data.bytes, (char*)data.pBuffer);
		return *this;
	}

	CGfxOutStream& CGfxOutStream::operator<< (const int mtx[2][2])
	{
		m_pHolder->_pushChar((char)(mtx[0][0]));
		m_pHolder->_pushChar((char)(mtx[0][1]));
		m_pHolder->_pushChar((char)(mtx[1][0]));
		m_pHolder->_pushChar((char)(mtx[1][1]));
		return *this;
	}

	CGfxOutStream& CGfxOutStream::operator<< (const float mtx[2][2])
	{
		m_pHolder->_pushFloat(mtx[0][0]);
		m_pHolder->_pushFloat(mtx[0][1]);
		m_pHolder->_pushFloat(mtx[1][0]);
		m_pHolder->_pushFloat(mtx[1][1]);
		return *this;
	}


	//____ allocObjectId() ____________________________________________________

	short CGfxOutStream::allocObjectId()
	{
		if (m_freeIdStackSize > 0)
			return m_pFreeIdStack[--m_freeIdStackSize];

		return m_idCounter++;
	}

	//____ freeObjectId() _____________________________________________________

	void CGfxOutStream::freeObjectId(short id)
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
