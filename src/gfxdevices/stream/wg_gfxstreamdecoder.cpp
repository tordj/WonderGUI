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

#include <wg_gfxstreamdecoder.h>

#include <tuple>

namespace wg
{
	const TypeInfo GfxStreamDecoder::TYPEINFO = { "GfxStreamDecoder", &Object::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamDecoder::GfxStreamDecoder()
	{
	}

	//____ destructor ________________________________________________________

	GfxStreamDecoder::~GfxStreamDecoder()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamDecoder::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setInput() ________________________________________________________

	void GfxStreamDecoder::setInput(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		m_pDataBegin = pBegin;
		m_pDataEnd = pEnd;
		m_pDataRead = pBegin;
	}

	//____ isEmpty() __________________________________________________________

	bool GfxStreamDecoder::isEmpty()
	{
		return !_hasChunk();
	}

	//____ peek() _____________________________________________________________

	GfxStream::Header GfxStreamDecoder::peek()
	{
		if (_hasChunk())
			return _peekChunk();

		return { GfxChunkId::OutOfData, 0 };
	}

	//____ skip() _____________________________________________________________

	void GfxStreamDecoder::skip(int bytes)
	{
		_skipBytes(bytes);
	}

	//____ skip() _____________________________________________________________

	void GfxStreamDecoder::align()
	{
		if( ( std::intptr_t(m_pDataRead) & 0x1) == 1 )
			_skipBytes(1);
	}


	//____ operator>> _________________________________________________________

	GfxStreamDecoder& GfxStreamDecoder::operator>> (GfxStream::Header& header)
	{
		if (_hasChunk())
		{
			header.type = (GfxChunkId)_pullShort();
			header.size = (uint16_t)_pullShort();
		}
		else
		{
			header.type = GfxChunkId::OutOfData;
			header.size = 0;
		}

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

	GfxStreamDecoder& GfxStreamDecoder::operator>> (CoordS& coord)
	{
		coord.x = _pullShort();
		coord.y = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (CoordI& coord)
	{
		coord.x = _pullInt();
		coord.y = _pullInt();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (CoordF& coord)
	{
		coord.x = _pullFloat();
		coord.y = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SizeS& sz)
	{
		sz.w = _pullShort();
		sz.h = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SizeI& sz)
	{
		sz.w = _pullInt();
		sz.h = _pullInt();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (SizeF& sz)
	{
		sz.w = _pullFloat();
		sz.h = _pullFloat();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (RectS& rect)
	{
		rect.x = _pullShort();
		rect.y = _pullShort();
		rect.w = _pullShort();
		rect.h = _pullShort();
		return *this;
	}

	GfxStreamDecoder& GfxStreamDecoder::operator>> (RectI& rect)
	{
		rect.x = _pullInt();
		rect.y = _pullInt();
		rect.w = _pullInt();
		rect.h = _pullInt();
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

	GfxStreamDecoder& GfxStreamDecoder::operator>> (BorderS& border)
	{
		border.top = _pullShort();
		border.right = _pullShort();
		border.bottom = _pullShort();
		border.left = _pullShort();
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
		* this >> gradient.topLeft;
		* this >> gradient.topRight;
		* this >> gradient.bottomRight;
		* this >> gradient.bottomLeft;
		* this >> gradient.isValid;
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
		r = (CanvasRef)_pullShort();
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


}
