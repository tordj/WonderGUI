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

	//____ operator<< _________________________________________________________

	GfxOutStream&  GfxOutStream::operator<< (GfxStream::Header header)
	{
		m_pHolder->_reserveStream(header.size + 4);
		m_pHolder->_pushShort((short)header.type);
		m_pHolder->_pushShort((short)header.size);
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
		m_pHolder->_pushInt(c.x);
		m_pHolder->_pushInt(c.y);
		return *this;
	}

	GfxOutStream&  GfxOutStream::operator<< (const Rect& rect)
	{
		m_pHolder->_pushInt(rect.x);
		m_pHolder->_pushInt(rect.y);
		m_pHolder->_pushInt(rect.w);
		m_pHolder->_pushInt(rect.h);
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

	GfxOutStream&  GfxOutStream::operator<< (BlendMode b)
	{
		m_pHolder->_pushShort((short) b);
		return *this;
	}


	GfxOutStream&  GfxOutStream::operator<< (Color color)
	{
		m_pHolder->_pushInt(color.argb);
		return *this;
	}




} // namespace wg
