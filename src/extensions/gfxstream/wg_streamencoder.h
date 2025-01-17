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

#ifndef	WG_STREAMENCODER_DOT_H
#define	WG_STREAMENCODER_DOT_H
#pragma once

#include <cstring>

#include <wg_streamsink.h>
#include <wg_componentptr.h>
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>

namespace wg
{
	class StreamEncoder;
	typedef	StrongPtr<StreamEncoder>	StreamEncoder_p;
	typedef	WeakPtr<StreamEncoder>	StreamEncoder_wp;

	//____ StreamEncoder ___________________________________________________

	class StreamEncoder : public Object
	{
	public:

		//.____ Blueprint __________________________________________________

		struct Blueprint
		{
			int				bufferBytes		= GfxStream::c_maxBlockSize * 2;
			Finalizer_p		finalizer;
			uint16_t		objectIdStart	= 0;
			PixelFormat		pixelFormat		= PixelFormat::BGRA_8;
			SampleMethod	sampleMethod	= SampleMethod::Nearest;
			StreamSink_p	sink;			
		};

		//.____ Creation __________________________________________

		static StreamEncoder_p	create(const StreamSink_p& pStream, int bufferBytes = GfxStream::c_maxBlockSize * 2) { return StreamEncoder_p(new StreamEncoder(pStream, bufferBytes)); }
		static StreamEncoder_p	create(const Blueprint& blueprint) { return StreamEncoder_p(new StreamEncoder(blueprint)); }


		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void			setStream(const StreamSink_p& pStream);

		void			setDefaultPixelFormat(PixelFormat pixelFormat);
		inline PixelFormat defaultPixelFormat() const { return m_defaultPixelFormat; }

		void			setDefaultSampleMethod(SampleMethod sampleMethod);
		inline SampleMethod defaultSampleMethod() const { return m_defaultSampleMethod; }

		void			flush();
		inline void		align();

		uint16_t		allocObjectId();
		void			freeObjectId(uint16_t id);


		//.____ Operators _____________________________________________________

		inline StreamEncoder& operator<< (GfxStream::Header);
		inline StreamEncoder& operator<< (const GfxStream::DataInfo&);
		inline StreamEncoder& operator<< (uint8_t);
		inline StreamEncoder& operator<< (int16_t);
		inline StreamEncoder& operator<< (uint16_t);
		inline StreamEncoder& operator<< (int32_t);
		inline StreamEncoder& operator<< (float);
		inline StreamEncoder& operator<< (bool);

		inline StreamEncoder& operator<< (const SizeI&);
		inline StreamEncoder& operator<< (const SizeF&);
		inline StreamEncoder& operator<< (const RectI&);
		inline StreamEncoder& operator<< (const RectF&);
		inline StreamEncoder& operator<< (const BorderI&);
		inline StreamEncoder& operator<< (const Border&);

		inline StreamEncoder& operator<< (HiColor);
		inline StreamEncoder& operator<< (PixelFormat);
		inline StreamEncoder& operator<< (SampleMethod);
		inline StreamEncoder& operator<< (CanvasRef);

		inline StreamEncoder& operator<< (const GfxStream::WriteBytes&);


	protected:
		StreamEncoder(const StreamSink_p& pStream, int bufferBytes);
		template<class BP> StreamEncoder( const BP& bp )
		{
			m_pBuffer = new uint8_t[bp.bufferBytes];
			m_capacity = bp.bufferBytes;
			m_pWriteData = m_pBuffer;

			m_pStream = bp.sink;
			
			if( bp.finalizer )
				setFinalizer(bp.finalizer);
			
			m_defaultPixelFormat = bp.pixelFormat;
			m_defaultSampleMethod = bp.sampleMethod;
			m_idCounter = bp.objectIdStart;
		}
		
		~StreamEncoder();

		void _beginChunk(GfxStream::Header header);
		
		inline void	_pushChar(char c);
		inline void	_pushShort(short s);
		inline void	_pushInt(int i);
		inline void	_pushFloat(float f);
		inline void	_pushBytes(int nBytes, const void* pBytes);
		
		short		m_idCounter = 1;

		PixelFormat	m_defaultPixelFormat = PixelFormat::BGRA_8;
		SampleMethod m_defaultSampleMethod = SampleMethod::Nearest;

		uint16_t*	m_pFreeIdStack = nullptr;
		int			m_freeIdStackCapacity = 0;
		int			m_freeIdStackSize = 0;
		
		ComponentPtr<StreamSink>	m_pStream;
		
		uint8_t*	m_pBuffer = nullptr;
		int			m_capacity = 0;
		uint8_t*	m_pWriteData = nullptr;

	};

	//____ align() _______________________________________________________________

	void StreamEncoder::align()
	{
		if( intptr_t(m_pWriteData) & 0x1 )
			_pushChar(0);
	}

	//____ _pushChar() ________________________________________________________

	void StreamEncoder::_pushChar(char c)
	{
		* m_pWriteData++ = c;
	}

	//____ _pushShort() ________________________________________________________

	void StreamEncoder::_pushShort(short s)
	{
		*(short*)m_pWriteData = s;
		m_pWriteData += 2;
	}

	//____ _pushInt() _________________________________________________________

	void StreamEncoder::_pushInt(int i)
	{
		*(short*)m_pWriteData = (short)i;
		m_pWriteData += 2;
		*(short*)m_pWriteData = (short)(i >> 16);
		m_pWriteData += 2;
	}

	//____ _pushFloat() _______________________________________________________

	void StreamEncoder::_pushFloat(float f)
	{
		int i = *((int*)&f);

		*(short*)m_pWriteData = (short)i;
		m_pWriteData += 2;
		*(short*)m_pWriteData = (short)(i >> 16);
		m_pWriteData += 2;
	}

	//____ _pushBytes() _________________________________________________________

	void StreamEncoder::_pushBytes(int nBytes, const void* pBytes)
	{
		std::memcpy(m_pWriteData, pBytes, nBytes);
		m_pWriteData += nBytes;
	}

	//____ operator<< ____________________________________________________________

	StreamEncoder& StreamEncoder::operator<< (GfxStream::Header header)
	{
		_beginChunk(header);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const GfxStream::DataInfo& info)
	{
		_pushInt(info.totalSize);
		_pushInt(info.chunkOffset);
		_pushShort((int(info.bFirstChunk) << 8) | (int(info.bLastChunk) << 9) | (int(info.bPadded) << 10)| int(info.compression));
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (uint8_t uint8)
	{
		_pushChar(uint8);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (int16_t int16)
	{
		_pushShort(int16);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (uint16_t uint16)
	{
		_pushShort(uint16);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (int32_t int32)
	{
		_pushInt(int32);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (float f)
	{
		_pushFloat(f);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (bool b)
	{
		int16_t	myBool = (int16_t)b;
		_pushShort(myBool);
		return *this;
	}



	StreamEncoder& StreamEncoder::operator<< (const SizeI& sz)
	{
		_pushInt(sz.w);
		_pushInt(sz.h);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const SizeF& sz)
	{
		_pushFloat(sz.w);
		_pushFloat(sz.h);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const RectI& rect)
	{
		_pushInt(rect.x);
		_pushInt(rect.y);
		_pushInt(rect.w);
		_pushInt(rect.h);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const RectF& rect)
	{
		_pushFloat(rect.x);
		_pushFloat(rect.y);
		_pushFloat(rect.w);
		_pushFloat(rect.h);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const BorderI& border)
	{
		_pushShort(border.top);
		_pushShort(border.right);
		_pushShort(border.bottom);
		_pushShort(border.left);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const Border& border)
	{
		_pushFloat(border.top);
		_pushFloat(border.right);
		_pushFloat(border.bottom);
		_pushFloat(border.left);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (PixelFormat t)
	{
		_pushShort((short)t);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (SampleMethod m)
	{
		_pushShort((short)m);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (CanvasRef r)
	{
		_pushChar((char)r);
		return *this;
	}


	StreamEncoder& StreamEncoder::operator<< (HiColor color)
	{
		_pushShort(color.b);
		_pushShort(color.g);
		_pushShort(color.r);
		_pushShort(color.a);
		return *this;
	}

	StreamEncoder& StreamEncoder::operator<< (const GfxStream::WriteBytes& data)
	{
		_pushBytes(data.bytes, (char*)data.pBuffer);
		return *this;
	}


}



#endif // WG_STREAMENCODER_DOT_H
