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



#include <wg_switch.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char Switch::CLASSNAME[] = {"Switch"};
	
	//____ Constructor ____________________________________________________________
	
	Switch::Switch()
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	Switch::~Switch()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Switch::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Switch::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Switch_p Switch::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Switch_p( static_cast<Switch*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setValue() _____________________________________________________________
	
	void Switch::setValue( int value )
	{
	    //TODO: Implement!
	}
	
	//____ preferredSize() __________________________________________________________
	
	Size Switch::preferredSize() const
	{
		Size contentSize(40,40);
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void Switch::_onCloneContent( const Widget * _pOrg )
	{
		const Switch * pOrg = static_cast<const Switch*>(_pOrg);
	}
	
	//____ _onRender() _____________________________________________________________
	
	void Switch::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
		else
			canvas = _canvas;
	
		int sz = wg::min( canvas.w,canvas.h );
	
		if( sz > 12 )
		{
			int itemSize = sz/4;
			int stepping = itemSize/2;
	
			int y = canvas.y;
			int x = canvas.x + (sz - itemSize) / 2;
			for( int i = 0 ; i < 3 ; i++ )
			{
				pDevice->clipDrawElipse( _clip, Rect(x,y,itemSize,itemSize), Color::White );
				y += itemSize + stepping;
			}
		}	
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool Switch::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		return true;
	}
	
	
	
	
	

} // namespace wg
