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

#include <wg_cgfxstreamsink.h>
#include <wg_types.h>
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

		//.____ Creation __________________________________________

		static GfxStreamEncoder_p	create( CGfxStreamSink* pStream, int bufferBytes = GfxStream::c_maxBlockSize*2 ) { return GfxStreamEncoder_p(new GfxStreamEncoder(pStream, bufferBytes)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void		setStream(CGfxStreamSink* pStream);

		void		setDefaultPixelFormat(PixelFormat pixelFormat);
		inline PixelFormat defaultPixelFormat() const { return m_defaultPixelFormat; }

		void		setDefaultSampleMethod(SampleMethod sampleMethod);
		inline SampleMethod defaultSampleMethod() const { return m_defaultSampleMethod; }

		void		flush();
		void		align();

		uint16_t	allocObjectId();
		void		freeObjectId(uint16_t id);


		//.____ Operators _____________________________________________________

		GfxStreamEncoder& operator<< (GfxStream::Header);
		GfxStreamEncoder& operator<< (int16_t);
		GfxStreamEncoder& operator<< (uint16_t);
		GfxStreamEncoder& operator<< (int32_t);
		GfxStreamEncoder& operator<< (float);
		GfxStreamEncoder& operator<< (bool);

		GfxStreamEncoder& operator<< (const CoordS&);
		GfxStreamEncoder& operator<< (const CoordI&);
		GfxStreamEncoder& operator<< (const CoordF&);
		GfxStreamEncoder& operator<< (const SizeS&);
		GfxStreamEncoder& operator<< (const SizeI&);
		GfxStreamEncoder& operator<< (const SizeF&);
		GfxStreamEncoder& operator<< (const RectS&);
		GfxStreamEncoder& operator<< (const RectI&);
		GfxStreamEncoder& operator<< (const RectF&);
		GfxStreamEncoder& operator<< (const BorderS&);
		GfxStreamEncoder& operator<< (const BorderI&);
		GfxStreamEncoder& operator<< (const Border&);
		GfxStreamEncoder& operator<< (const Gradient&);

		GfxStreamEncoder& operator<< (HiColor);
		GfxStreamEncoder& operator<< (Direction);
		GfxStreamEncoder& operator<< (BlendMode);
		GfxStreamEncoder& operator<< (TintMode);
		GfxStreamEncoder& operator<< (Axis);
		GfxStreamEncoder& operator<< (PixelFormat);
		GfxStreamEncoder& operator<< (SampleMethod);
		GfxStreamEncoder& operator<< (GfxFlip);
		GfxStreamEncoder& operator<< (XSections);
		GfxStreamEncoder& operator<< (YSections);
		GfxStreamEncoder& operator<< (CanvasRef);

		GfxStreamEncoder& operator<< (const GfxStream::DataChunk&);

		GfxStreamEncoder& operator<< (const int[2][2]);
		GfxStreamEncoder& operator<< (const float[2][2]);


	protected:
		GfxStreamEncoder( CGfxStreamSink* pStream, int bufferBytes );
		~GfxStreamEncoder();

		inline void	_reserveStream(int bytes);

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

		uint8_t*	m_pBuffer = nullptr;
		int			m_capacity = 0;
		int			m_size = 0;

		CGfxStreamSink_p	m_pStream;
	};

	//____ _reserveStream() ________________________________________________________

	void GfxStreamEncoder::_reserveStream(int bytes)
	{
		if (m_capacity - m_size < bytes)
			flush();
	}


	//____ _pushChar() ________________________________________________________

	void GfxStreamEncoder::_pushChar(char c)
	{
		m_pBuffer[m_size++] = c;
	}

	//____ _pushShort() ________________________________________________________

	void GfxStreamEncoder::_pushShort(short s)
	{
		*(short*)(&m_pBuffer[m_size]) = s;
		m_size += 2;
	}

	//____ _pushInt() _________________________________________________________

	void GfxStreamEncoder::_pushInt(int i)
	{
		//		*(int*)(&m_pBuffer[m_size]) = i;

		*(short*)(&m_pBuffer[m_size]) = (short)i;
		*(short*)(&m_pBuffer[m_size + 2]) = (short)(i >> 16);
		m_size += 4;
	}

	//____ _pushFloat() _______________________________________________________

	void GfxStreamEncoder::_pushFloat(float f)
	{
		//		*(float*)(&m_pBuffer[m_size]) = f;

		int i = *((int*)&f);

		*(short*)(&m_pBuffer[m_size]) = (short)i;
		*(short*)(&m_pBuffer[m_size + 2]) = (short)(i >> 16);
		m_size += 4;
	}

	//____ _pushBytes() _________________________________________________________

	void GfxStreamEncoder::_pushBytes(int nBytes, char* pBytes)
	{
		std::memcpy(&m_pBuffer[m_size], pBytes, nBytes);
		m_size += nBytes;
	}



}



#endif // WG_GFXSTREAMENCODER_DOT_H
