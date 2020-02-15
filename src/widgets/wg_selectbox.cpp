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



#include <wg_selectbox.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

namespace wg
{
	using namespace Util;

	const char SelectBox::CLASSNAME[] = {"SelectBox"};

	//____ Constructor ____________________________________________________________

	SelectBox::SelectBox() : text(this), entries(this)
	{
	}

	//____ Destructor _____________________________________________________________

	SelectBox::~SelectBox()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool SelectBox::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * SelectBox::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	SelectBox_p SelectBox::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SelectBox_p( static_cast<SelectBox*>(pObject) );

		return 0;
	}




	//____ _preferredSize() _____________________________________________________________

	SizeI SelectBox::_preferredSize() const
	{
	}

	//____ _cloneContent() _______________________________________________________

	void SelectBox::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		SelectBox * pOrg = (SelectBox*) _pOrg;

	}

	//____ _render() _____________________________________________________________

	void SelectBox::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectI canvas;
		if( m_pSkin )
			canvas = m_pSkin->_contentRect(_canvas, m_state);
		else
			canvas = _canvas;

		_text()._render(pDevice, canvas);
	}


} // namespace wg
