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

#include <wg_gfxstreamencoder.h>

namespace wg
{
	const TypeInfo GfxStreamEncoder::TYPEINFO = { "GfxStreamEncoder", &Object::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamEncoder::GfxStreamEncoder(CGfxOutStream* pStream, int bufferBytes)
	{
		m_pStream = pStream;

		m_pBuffer = new uint8_t[bufferBytes];
		m_capacity = bufferBytes;
		m_size = 0;
	}

	//____ destructor ________________________________________________________

	GfxStreamEncoder::~GfxStreamEncoder()
	{
		delete[] m_pBuffer;
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamEncoder::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setStream() ________________________________________________________

	void GfxStreamEncoder::setStream(CGfxOutStream* pStream)
	{
		m_pStream = pStream;
	}

	//____ setDefaultPixelFormat() ____________________________________________

	void GfxStreamEncoder::setDefaultPixelFormat(PixelFormat pixelFormat)
	{
		m_defaultPixelFormat = pixelFormat;
	}

	//____ setDefaultSampleMethod() ___________________________________________

	void GfxStreamEncoder::setDefaultSampleMethod(SampleMethod sampleMethod)
	{
		m_defaultSampleMethod = sampleMethod;
	}


	//____ flush() ____________________________________________________________

	void GfxStreamEncoder::flush()
	{
		if (m_pStream)
			m_pStream->processChunks( m_pBuffer, m_pBuffer + m_size);

		m_size = 0;
	}

	//____ allocObjectId() ____________________________________________________

	uint16_t GfxStreamEncoder::allocObjectId()
	{
		if (m_freeIdStackSize > 0)
			return m_pFreeIdStack[--m_freeIdStackSize];

		return m_idCounter++;
	}

	//____ freeObjectId() _____________________________________________________

	void GfxStreamEncoder::freeObjectId(uint16_t id)
	{
		if (m_freeIdStackSize == m_freeIdStackCapacity)
		{
			int capacity = std::max(16, m_freeIdStackCapacity * 2);
			uint16_t* pBuffer = new uint16_t[capacity];

			for (int i = 0; i < m_freeIdStackSize; i++)
				pBuffer[i] = m_pFreeIdStack[i];

			delete[] m_pFreeIdStack;

			m_pFreeIdStack = pBuffer;
			m_freeIdStackCapacity = capacity;
		}

		m_pFreeIdStack[m_freeIdStackSize++] = id;
	}

	//____ operator<< _________________________________________________________

	GfxStreamEncoder& GfxStreamEncoder::operator<< (GfxStream::Header header)
	{
		_reserveStream(header.size + 4);
		_pushShort((short)header.type);
		_pushShort((short)header.size);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (int16_t int16)
	{
		_pushShort(int16);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (uint16_t uint16)
	{
		_pushShort(uint16);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (int32_t int32)
	{
		_pushInt(int32);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (float f)
	{
		_pushFloat(f);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (bool b)
	{
		int16_t	myBool = (int16_t)b;
		_pushShort(myBool);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const CoordI& c)
	{
		_pushInt(c.x);
		_pushInt(c.y);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const CoordF& c)
	{
		_pushFloat(c.x);
		_pushFloat(c.y);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const SizeI& sz)
	{
		_pushInt(sz.w);
		_pushInt(sz.h);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const SizeF& sz)
	{
		_pushFloat(sz.w);
		_pushFloat(sz.h);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const RectI& rect)
	{
		_pushInt(rect.x);
		_pushInt(rect.y);
		_pushInt(rect.w);
		_pushInt(rect.h);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const RectF& rect)
	{
		_pushFloat(rect.x);
		_pushFloat(rect.y);
		_pushFloat(rect.w);
		_pushFloat(rect.h);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const BorderI& border)
	{
		_pushShort(border.top);
		_pushShort(border.right);
		_pushShort(border.bottom);
		_pushShort(border.left);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const Border& border)
	{
		_pushFloat(border.top);
		_pushFloat(border.right);
		_pushFloat(border.bottom);
		_pushFloat(border.left);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (Direction d)
	{
		_pushShort((short)d);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (BlendMode b)
	{
		_pushShort((short)b);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (TintMode t)
	{
		_pushShort((short)t);
		return *this;
	}


	GfxStreamEncoder& GfxStreamEncoder::operator<< (Axis o)
	{
		_pushShort((short)o);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (PixelFormat t)
	{
		_pushShort((short)t);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (SampleMethod m)
	{
		_pushShort((short)m);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (GfxFlip f)
	{
		_pushShort((short)f);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (XSections x)
	{
		_pushShort((short)x);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (YSections y)
	{
		_pushShort((short)y);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (CanvasRef r)
	{
		_pushShort((short)r);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (HiColor color)
	{
		_pushShort(color.b);
		_pushShort(color.g);
		_pushShort(color.r);
		_pushShort(color.a);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const GfxStream::DataChunk& data)
	{
		_pushBytes(data.bytes, (char*)data.pBuffer);
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const int mtx[2][2])
	{
		_pushChar((char)(mtx[0][0]));
		_pushChar((char)(mtx[0][1]));
		_pushChar((char)(mtx[1][0]));
		_pushChar((char)(mtx[1][1]));
		return *this;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const float mtx[2][2])
	{
		_pushFloat(mtx[0][0]);
		_pushFloat(mtx[0][1]);
		_pushFloat(mtx[1][0]);
		_pushFloat(mtx[1][1]);
		return *this;
	}

}
