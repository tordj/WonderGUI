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



#include <wg_knob.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char Knob::CLASSNAME[] = {"Knob"};
	
	//____ Constructor ____________________________________________________________
	
	Knob::Knob()
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	Knob::~Knob()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Knob::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Knob::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Knob_p Knob::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Knob_p( static_cast<Knob*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setValue() _____________________________________________________________
	
	void Knob::setValue( float value )
	{
	    //TODO: Implement!
	}
	
	//____ preferredSize() __________________________________________________________
	
	Size Knob::preferredSize() const
	{
		return Size(40,40);
	}
	
	
	//____ _onCloneContent() _______________________________________________________
	
	void Knob::_onCloneContent( const Widget * _pOrg )
	{
		const Knob * pOrg = static_cast<const Knob*>(_pOrg);
	}
	
	//____ _onRender() _____________________________________________________________
	
	void Knob::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice, _canvas, _window, _clip);
	
		int sz = WgMin( _canvas.w,_canvas.h );
	
		if( sz > 1 )
		{
			pDevice->clipDrawElipse( _clip, Rect(_canvas.pos(),sz,sz), Color::pink );
		}
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool Knob::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		return true;
	}
	
	

} // namespace wg
