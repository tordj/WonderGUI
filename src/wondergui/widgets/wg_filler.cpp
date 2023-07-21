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
		m_defaultSize = Size(-1,-1);
	}

	Filler::Filler( const Blueprint& bp )
	{
		_initFromBlueprint(bp); 
		m_defaultSize = bp.defaultSize;
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

	//____ setDefaultSize() _______________________________________________________

	void Filler::setDefaultSize( const Size& size )
	{
		if( size != m_defaultSize )
		{
			m_defaultSize = size;
			_requestResize();
		}
	}

	//____ defaultSize() __________________________________________________________

	SizeSPX Filler::_defaultSize(int scale) const
	{
		if (m_defaultSize.w >= 0 && m_defaultSize.h >= 0)
		{
			return SizeSPX(m_defaultSize*scale);
		}
		else
			return Widget::_defaultSize(scale);
	}

} // namespace wg
