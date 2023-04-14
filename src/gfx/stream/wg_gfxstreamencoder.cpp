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

#include <algorithm>

namespace wg
{
	const TypeInfo GfxStreamEncoder::TYPEINFO = { "GfxStreamEncoder", &Object::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamEncoder::GfxStreamEncoder(const GfxStreamSink_p& pStream)
	{
		m_pStream = pStream;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamEncoder::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setStream() ________________________________________________________

	void GfxStreamEncoder::setStream(const GfxStreamSink_p& pStream)
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

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const GfxStream::WriteSpxArray& array)
	{
		switch( array.spxFormat )
		{
			case GfxStream::SpxFormat::Int32_dec:
				_pushBytes(array.size*4, array.pData);
				break;
			
			case GfxStream::SpxFormat::Int16_int:
			{
				auto p = array.pData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushShort(* p++ >> 6);
				break;
			}

			case GfxStream::SpxFormat::Uint16_dec:
			{
				auto p = array.pData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushShort(* p++);
				break;
			}
				
			case GfxStream::SpxFormat::Uint8_int:
			{
				auto p = array.pData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushChar(* p++ >> 6);
				break;
			}

			case GfxStream::SpxFormat::Delta16_dec:
			{
				auto p1 = array.pData;
				auto p2 = array.pCompareData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushShort((* p1++) - (* p2++));
				break;
			}

			case GfxStream::SpxFormat::Delta16_int:
			{
				auto p1 = array.pData;
				auto p2 = array.pCompareData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushShort( ((* p1++) - (* p2++)) << 6);
				break;
			}

			case GfxStream::SpxFormat::Delta8_dec:
			{
				auto p1 = array.pData;
				auto p2 = array.pCompareData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushChar((* p1++) - (* p2++));
				break;
			}

			case GfxStream::SpxFormat::Delta8_int:
			{
				auto p1 = array.pData;
				auto p2 = array.pCompareData;
				for( int i = 0 ; i < array.size ; i++ )
					_pushChar( ((* p1++) - (* p2++)) << 6);
				break;
			}
		}
		return *this;
	}


}
