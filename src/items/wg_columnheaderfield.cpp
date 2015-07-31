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

#include <wg_columnheaderfield.h>

namespace wg 
{
	
	WgColumnHeaderField::WgColumnHeaderField(WgColumnHeaderHolder * pHolder) : WgField(pHolder), icon(this), arrow(this), label(this)
	{
		m_height = 0;
		m_width = 0;
		m_bPressed = false;
	}
	
	//____ setSkin() ______________________________________________________
	
	void WgColumnHeaderField::setSkin( const WgSkin_p& pSkin )
	{
		if( pSkin != m_pSkin )
		{
			m_pSkin = pSkin;
			_onResize();
		}
	}
	
	//____ _onFieldDirty() _________________________________________________________
	
	void WgColumnHeaderField::_onFieldDirty( WgField * pField )
	{
		_onDirty();
	}
	
	//____ _onFieldResize() ________________________________________________________
	
	void WgColumnHeaderField::_onFieldResize( WgField * pField )
	{
		_onResize();
	}

} // namespace wg
