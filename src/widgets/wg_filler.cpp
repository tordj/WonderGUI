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
		m_preferredSize = SizeI(-1,-1);
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

	Size Filler::preferredSize() const
	{
		if( m_preferredSize.w >= 0 && m_preferredSize.h >= 0 )
			return m_preferredSize;
		else
			return Widget::preferredSize();
	}


	//____ _cloneContent() _______________________________________________________

	void Filler::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Filler * pOrg = (Filler*) _pOrg;

		m_preferredSize = pOrg->m_preferredSize;
	}

} // namespace wg
