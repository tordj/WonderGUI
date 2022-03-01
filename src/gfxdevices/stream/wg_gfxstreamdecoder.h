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

#ifndef	WG_GFXSTREAMDECODER_DOT_H
#define	WG_GFXSTREAMDECODER_DOT_H
#pragma once

#include <wg_gfxstream.h>
#include <wg_types.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_cgfxinstream.h>

#include <cstring>

namespace wg
{
	class GfxStreamDecoder;
	typedef	StrongPtr<GfxStreamDecoder>	GfxStreamDecoder_p;
	typedef	WeakPtr<GfxStreamDecoder>	GfxStreamDecoder_wp;

	//____ GfxStreamDecoder ___________________________________________________

	class GfxStreamDecoder : public Object
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamDecoder_p	create() { return GfxStreamDecoder_p(new GfxStreamDecoder()); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void				setInput( const uint8_t * pBegin, const uint8_t * pEnd);

		bool				isEmpty();
		GfxStream::Header	peek();

		void				skip(int bytes);
		void				align();

		//.____ Operators _____________________________________________

		GfxStreamDecoder& operator>> (GfxStream::Header& header);

		GfxStreamDecoder& operator>> (int16_t&);
		GfxStreamDecoder& operator>> (uint16_t&);
		GfxStreamDecoder& operator>> (int&);
		GfxStreamDecoder& operator>> (float&);
		GfxStreamDecoder& operator>> (bool&);

		GfxStreamDecoder& operator>> (CoordI&);
		GfxStreamDecoder& operator>> (CoordF&);
		GfxStreamDecoder& operator>> (SizeI&);
		GfxStreamDecoder& operator>> (SizeF&);
		GfxStreamDecoder& operator>> (RectI&);
		GfxStreamDecoder& operator>> (RectF&);
		GfxStreamDecoder& operator>> (BorderI&);
		GfxStreamDecoder& operator>> (Border&);
		GfxStreamDecoder& operator>> (Gradient&);

		GfxStreamDecoder& operator>> (HiColor&);
		GfxStreamDecoder& operator>> (Direction&);
		GfxStreamDecoder& operator>> (BlendMode&);
		GfxStreamDecoder& operator>> (TintMode&);
		GfxStreamDecoder& operator>> (Axis&);
		GfxStreamDecoder& operator>> (PixelFormat&);
		GfxStreamDecoder& operator>> (SampleMethod&);
		GfxStreamDecoder& operator>> (GfxFlip&);
		GfxStreamDecoder& operator>> (XSections&);
		GfxStreamDecoder& operator>> (YSections&);
		GfxStreamDecoder& operator>> (CanvasRef&);

		GfxStreamDecoder& operator>> (const GfxStream::DataChunk&);

		GfxStreamDecoder& operator>> (int[2][2]);
		GfxStreamDecoder& operator>> (float[2][2]);


	protected:
		GfxStreamDecoder();
		~GfxStreamDecoder();

		inline bool		_hasChunk();
		inline GfxStream::Header	_peekChunk();	// Is only called if _hasChunk() has returned true.

		inline char		_pullChar();
		inline short	_pullShort();
		inline int		_pullInt();
		inline float	_pullFloat();
		inline void		_pullBytes(int nBytes, char* pBytes);
		inline void		_skipBytes(int nBytes);

		const uint8_t* m_pDataBegin = nullptr;
		const uint8_t* m_pDataEnd = nullptr;
		const uint8_t* m_pDataRead = nullptr;
	};

	//____ _hasChunk() ______________________________________________________

	bool GfxStreamDecoder::_hasChunk()
	{
		if (m_pDataRead != m_pDataEnd)
			return true;

		return false;
	}

	//____ _peekChunk() ______________________________________________________

	GfxStream::Header GfxStreamDecoder::_peekChunk()
	{
		return { (GfxChunkId)(*(uint16_t*)m_pDataRead), *(uint16_t*)(m_pDataRead + 2) };
	}


	//____ _pullChar() ______________________________________________________

	char GfxStreamDecoder::_pullChar()
	{
		char x = *(char*)m_pDataRead;
		m_pDataRead++;
		return x;
	}

	//____ _pullShort() ______________________________________________________

	short GfxStreamDecoder::_pullShort()
	{
		short x = *(short*)m_pDataRead;
		m_pDataRead += 2;
		return x;
	}

	//____ _pullInt() ______________________________________________________

	int GfxStreamDecoder::_pullInt()
	{
		int x = *(uint16_t*)m_pDataRead;
		m_pDataRead += 2;
		x += (*(uint16_t*)m_pDataRead << 16);
		m_pDataRead += 2;

		return x;
	}

	//____ _pullFloat() ______________________________________________________

	float GfxStreamDecoder::_pullFloat()
	{
		int x = *(uint16_t*)m_pDataRead;
		m_pDataRead += 2;
		x += (*(uint16_t*)m_pDataRead << 16);
		m_pDataRead += 2;

		return *((float*)&x);
	}

	//____ _pullBytes() ______________________________________________________

	void GfxStreamDecoder::_pullBytes(int nBytes, char* pBytes)
	{
		std::memcpy(pBytes, m_pDataRead, nBytes);
		m_pDataRead += nBytes;
	}

	//____ _skipBytes() ______________________________________________________

	void GfxStreamDecoder::_skipBytes(int nBytes)
	{
		m_pDataRead += nBytes;
	}




}



#endif // WG_GFXSTREAMDecoder_DOT_H
