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
#include <wg_gfxtypes.h>
#include <wg_geo.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_gfxstreamsource.h>

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
		inline int			chunkSize();
		

		void				skip(int bytes);
		void				align();

		//.____ Operators _____________________________________________

		inline GfxStreamDecoder& operator>> (GfxStream::Header& header);

		inline GfxStreamDecoder& operator>> (uint8_t&);
		inline GfxStreamDecoder& operator>> (int16_t&);
		inline GfxStreamDecoder& operator>> (uint16_t&);
		inline GfxStreamDecoder& operator>> (int&);
		inline GfxStreamDecoder& operator>> (float&);
		inline GfxStreamDecoder& operator>> (bool&);

		inline GfxStreamDecoder& operator>> (GfxStream::SPX&);
		inline GfxStreamDecoder& operator>> (CoordI&);
		inline GfxStreamDecoder& operator>> (CoordF&);
		inline GfxStreamDecoder& operator>> (SizeI&);
		inline GfxStreamDecoder& operator>> (SizeF&);
		inline GfxStreamDecoder& operator>> (RectI&);
		inline GfxStreamDecoder& operator>> (RectF&);
		inline GfxStreamDecoder& operator>> (BorderI&);
		inline GfxStreamDecoder& operator>> (Border&);
		inline GfxStreamDecoder& operator>> (Gradient&);

		inline GfxStreamDecoder& operator>> (HiColor&);
		inline GfxStreamDecoder& operator>> (Direction&);
		inline GfxStreamDecoder& operator>> (BlendMode&);
		inline GfxStreamDecoder& operator>> (TintMode&);
		inline GfxStreamDecoder& operator>> (Axis&);
		inline GfxStreamDecoder& operator>> (PixelFormat&);
		inline GfxStreamDecoder& operator>> (SampleMethod&);
		inline GfxStreamDecoder& operator>> (GfxFlip&);
		inline GfxStreamDecoder& operator>> (XSections&);
		inline GfxStreamDecoder& operator>> (YSections&);
		inline GfxStreamDecoder& operator>> (CanvasRef&);
		
		inline GfxStreamDecoder& operator>> (const GfxStream::DataChunk&);

		inline GfxStreamDecoder& operator>> (int[2][2]);
		inline GfxStreamDecoder& operator>> (float[2][2]);


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

		int				m_spxFormat;

		typedef	const uint8_t* (*SpxOp_p)(const uint8_t* pStream, spx& output);
		typedef	const uint8_t*(*CoordOp_p)(const uint8_t* pStream, CoordI& output);
		typedef	const uint8_t*(*SizeOp_p)(const uint8_t* pStream, SizeI& output);
		typedef	const uint8_t*(*RectOp_p)(const uint8_t* pStream, RectI& output);

		static const SpxOp_p	s_spxOps[4];
		static const CoordOp_p	s_coordOps[4];
		static const SizeOp_p	s_sizeOps[4];
		static const RectOp_p	s_rectOps[4];

		const uint8_t* m_pDataBegin = nullptr;
		const uint8_t* m_pDataEnd = nullptr;
		const uint8_t* m_pDataRead = nullptr;
	};

	GfxStreamDecoder& GfxStreamDecoder::operator>> (GfxStream::Header& header)
	{
		if (_hasChunk())
		{
			header.type = (GfxChunkId)_pullChar();
			uint8_t sizeEtc = _pullChar();
			header.spxFormat = sizeEtc >> 5;
			m_spxFormat = sizeEtc >> 5;

			sizeEtc &= 0x1F;
			if (sizeEtc <= 30)
				header.size = sizeEtc;
			else
				header.size = (uint16_t)_pullShort();
		}
		else
		{
			header.type = GfxChunkId::OutOfData;
			header.spxFormat = 0;
			header.size = 0;
		}

		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (uint8_t& i)
	{
		i = _pullChar();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (int16_t& i)
	{
		i = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (uint16_t& i)
	{
		i = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (int& i)
	{
		i = _pullInt();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (float& f)
	{
		f = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (bool& b)
	{
		int16_t myBool = _pullShort();
		b = (bool)myBool;
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (GfxStream::SPX& value)
	{
		if(m_spxFormat == 0 )
			value.value = _pullInt();
		else if(m_spxFormat == 1 )
			value.value = uint16_t(_pullShort());			// Value is unsigned.
		else if(m_spxFormat == 2 )
			value.value = int(_pullShort()) << 6;
		else if(m_spxFormat == 3 )
			value.value = int(uint8_t(_pullChar())) << 6;	// Make sure value is unsigned.
		return *this;
	}


	GfxStreamDecoder& GfxStreamDecoder::operator>> (CoordI& coord)
	{
		m_pDataRead = s_coordOps[m_spxFormat](m_pDataRead, coord);
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (CoordF& coord)
	{
		coord.x = _pullFloat();
		coord.y = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SizeI& sz)
	{
		m_pDataRead = s_sizeOps[m_spxFormat](m_pDataRead, sz);
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SizeF& sz)
	{
		sz.w = _pullFloat();
		sz.h = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (RectI& rect)
	{
		m_pDataRead = s_rectOps[m_spxFormat](m_pDataRead, rect);
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (RectF& rect)
	{
		rect.x = _pullFloat();
		rect.y = _pullFloat();
		rect.w = _pullFloat();
		rect.h = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (BorderI& border)
	{
		border.top = _pullShort();
		border.right = _pullShort();
		border.bottom = _pullShort();
		border.left = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (Border& border)
	{
		border.top = _pullFloat();
		border.right = _pullFloat();
		border.bottom = _pullFloat();
		border.left = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (Gradient& gradient)
	{
		*this >> gradient.topLeft;
		*this >> gradient.topRight;
		*this >> gradient.bottomRight;
		*this >> gradient.bottomLeft;
		*this >> gradient.isValid;
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (HiColor& color)
	{
		color.b = _pullShort();
		color.g = _pullShort();
		color.r = _pullShort();
		color.a = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (Direction& dir)
	{
		dir = (Direction)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (BlendMode& blendMode)
	{
		blendMode = (BlendMode)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (TintMode& tintMode)
	{
		tintMode = (TintMode)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (Axis& o)
	{
		o = (Axis)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (PixelFormat& t)
	{
		t = (PixelFormat)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SampleMethod& m)
	{
		m = (SampleMethod)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (GfxFlip& f)
	{
		f = (GfxFlip)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (XSections& x)
	{
		x = (XSections)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (YSections& y)
	{
		y = (YSections)_pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (CanvasRef& r)
	{
		r = (CanvasRef)_pullChar();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (const GfxStream::DataChunk& data)
	{
		_pullBytes(data.bytes, (char*)data.pBuffer);
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (int mtx[2][2])
	{
		mtx[0][0] = _pullChar();
		mtx[0][1] = _pullChar();
		mtx[1][0] = _pullChar();
		mtx[1][1] = _pullChar();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (float mtx[2][2])
	{
		mtx[0][0] = _pullFloat();
		mtx[0][1] = _pullFloat();
		mtx[1][0] = _pullFloat();
		mtx[1][1] = _pullFloat();
		return *this;
	}


	//____ chunkSize() ______________________________________________________

	int GfxStreamDecoder::chunkSize()
	{
		uint8_t sizeEtc = m_pDataRead[1] & 0x1F;
		
		if (sizeEtc <= 30)
			return sizeEtc + 2;
		else
			return (* (uint16_t*) &m_pDataRead[2]) + 4;
	}

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
		GfxStream::Header header;

		header.type = (GfxChunkId) m_pDataRead[0];
		uint8_t sizeEtc = m_pDataRead[1];
		header.spxFormat = sizeEtc >> 5;
		
		sizeEtc &= 0x1F;
		if (sizeEtc <= 30)
			header.size = sizeEtc;
		else
			header.size = * (uint16_t*) &m_pDataRead[2];

		return header;
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
