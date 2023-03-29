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

#ifndef	WG_GFXSTREAMENCODER_DOT_H
#define	WG_GFXSTREAMENCODER_DOT_H
#pragma once

#include <cstring>

#include <wg_gfxstreamsink.h>
#include <wg_componentptr.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>

namespace wg
{
	class GfxStreamEncoder;
	typedef	StrongPtr<GfxStreamEncoder>	GfxStreamEncoder_p;
	typedef	WeakPtr<GfxStreamEncoder>	GfxStreamEncoder_wp;

	//____ GfxStreamEncoder ___________________________________________________

	class GfxStreamEncoder : public Object
	{
	public:

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void			setStream(const GfxStreamSink_p& pStream);

		void			setDefaultPixelFormat(PixelFormat pixelFormat);
		inline PixelFormat defaultPixelFormat() const { return m_defaultPixelFormat; }

		void			setDefaultSampleMethod(SampleMethod sampleMethod);
		inline SampleMethod defaultSampleMethod() const { return m_defaultSampleMethod; }

		virtual void	flush() = 0;
		inline void		align();

		uint16_t		allocObjectId();
		void			freeObjectId(uint16_t id);


		//.____ Operators _____________________________________________________

		virtual GfxStreamEncoder& operator<< (GfxStream::Header) = 0;
		inline GfxStreamEncoder& operator<< (uint8_t);
		inline GfxStreamEncoder& operator<< (int16_t);
		inline GfxStreamEncoder& operator<< (uint16_t);
		inline GfxStreamEncoder& operator<< (int32_t);
		inline GfxStreamEncoder& operator<< (float);
		inline GfxStreamEncoder& operator<< (bool);

		inline GfxStreamEncoder& operator<< (const CoordI&);
		inline GfxStreamEncoder& operator<< (const CoordF&);
		inline GfxStreamEncoder& operator<< (const SizeI&);
		inline GfxStreamEncoder& operator<< (const SizeF&);
		inline GfxStreamEncoder& operator<< (const RectI&);
		inline GfxStreamEncoder& operator<< (const RectF&);
		inline GfxStreamEncoder& operator<< (const BorderI&);
		inline GfxStreamEncoder& operator<< (const Border&);
		inline GfxStreamEncoder& operator<< (const Gradient&);

		inline GfxStreamEncoder& operator<< (HiColor);
		inline GfxStreamEncoder& operator<< (Direction);
		inline GfxStreamEncoder& operator<< (BlendMode);
		inline GfxStreamEncoder& operator<< (TintMode);
		inline GfxStreamEncoder& operator<< (Axis);
		inline GfxStreamEncoder& operator<< (PixelFormat);
		inline GfxStreamEncoder& operator<< (SampleMethod);
		inline GfxStreamEncoder& operator<< (GfxFlip);
		inline GfxStreamEncoder& operator<< (XSections);
		inline GfxStreamEncoder& operator<< (YSections);
		inline GfxStreamEncoder& operator<< (CanvasRef);

		inline GfxStreamEncoder& operator<< (const GfxStream::DataChunk&);

		inline GfxStreamEncoder& operator<< (const int[2][2]);
		inline GfxStreamEncoder& operator<< (const float[2][2]);


	protected:
		GfxStreamEncoder( const GfxStreamSink_p& pStream );
		~GfxStreamEncoder() {};

		inline void	_pushChar(char c);
		inline void	_pushShort(short s);
		inline void	_pushInt(int i);
		inline void	_pushFloat(float f);
		inline void	_pushBytes(int nBytes, char* pBytes);
		
		short		m_idCounter = 1;

		PixelFormat	m_defaultPixelFormat = PixelFormat::BGRA_8;
		SampleMethod m_defaultSampleMethod = SampleMethod::Nearest;

		uint16_t*	m_pFreeIdStack = nullptr;
		int			m_freeIdStackCapacity = 0;
		int			m_freeIdStackSize = 0;

		ComponentPtr<GfxStreamSink>	m_pStream;
		
		uint8_t*	m_pWriteData = nullptr;

	};

	//____ align() _______________________________________________________________

	void GfxStreamEncoder::align()
	{
		if( intptr_t(m_pWriteData) & 0x1 )
			_pushChar(0);
	}

	//____ _pushChar() ________________________________________________________

	void GfxStreamEncoder::_pushChar(char c)
	{
		* m_pWriteData++ = c;
	}

	//____ _pushShort() ________________________________________________________

	void GfxStreamEncoder::_pushShort(short s)
	{
		*(short*)m_pWriteData = s;
		m_pWriteData += 2;
	}

	//____ _pushInt() _________________________________________________________

	void GfxStreamEncoder::_pushInt(int i)
	{
		*(short*)m_pWriteData = (short)i;
		m_pWriteData += 2;
		*(short*)m_pWriteData = (short)(i >> 16);
		m_pWriteData += 2;
	}

	//____ _pushFloat() _______________________________________________________

	void GfxStreamEncoder::_pushFloat(float f)
	{
		int i = *((int*)&f);

		*(short*)m_pWriteData = (short)i;
		m_pWriteData += 2;
		*(short*)m_pWriteData = (short)(i >> 16);
		m_pWriteData += 2;
	}

	//____ _pushBytes() _________________________________________________________

	void GfxStreamEncoder::_pushBytes(int nBytes, char* pBytes)
	{
		std::memcpy(m_pWriteData, pBytes, nBytes);
		m_pWriteData += nBytes;
	}

	GfxStreamEncoder& GfxStreamEncoder::operator<< (uint8_t uint8)
	{
		_pushChar(uint8);
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

	GfxStreamEncoder& GfxStreamEncoder::operator<< (const Gradient& gradient)
	{
		* this << gradient.topLeft;
		* this << gradient.topRight;
		* this << gradient.bottomRight;
		* this << gradient.bottomLeft;
		* this << gradient.isValid;
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
		_pushChar((char)r);
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



#endif // WG_GFXSTREAMENCODER_DOT_H
