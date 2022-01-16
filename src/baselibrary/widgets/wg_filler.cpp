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

#include <wg_filler.h>
#include <wg_gfxdevice.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Filler::TYPEINFO = { "Filler", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Filler::Filler()
	{
		m_preferredSize = Size(-1,-1);
	}

	//____ Destructor _____________________________________________________________

	Filler::~Filler()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Filler::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPreferredSize() _______________________________________________________

	void Filler::setPreferredSize( const Size& size )
	{
		if( size != m_preferredSize )
		{
			m_preferredSize = size;
			_requestResize();
		}
	}

	//____ preferredSize() __________________________________________________________

	SizeSPX Filler::_preferredSize(int _scale) const
	{
		if (m_preferredSize.w >= 0 && m_preferredSize.h >= 0)
		{
			int scale = _fixScale(_scale);
			return SizeSPX(m_preferredSize*scale);
		}
		else
			return Widget::_preferredSize(_scale);
	}

} // namespace wg
