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



#include <wg_canvas.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	const char Canvas::CLASSNAME[] = {"Canvas"};
	
	//____ Constructor ____________________________________________________________
	
	Canvas::Canvas() : canvas(&m_canvas), m_canvas(this,&canvas)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	Canvas::~Canvas()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Canvas::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ________________________________________________________
	
	const char * Canvas::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _____________________________________________________________
	
	Canvas_p Canvas::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Canvas_p( static_cast<Canvas*>(pObject) );
	
		return 0;
	}
	
	//____ matchingHeight() ___________________________________________________

	int Canvas::matchingHeight(int width) const
	{
		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		Size sz = m_canvas.preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16;

		return width * sz.h / sz.w;
	}

	//____ matchingWidth() ____________________________________________________

	int Canvas::matchingWidth(int height) const
	{
		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		Size sz = m_canvas.preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16;

		return height * sz.w / sz.h;
	}

	//____ preferredSize() ____________________________________________________

	Size Canvas::preferredSize() const
	{
		Size sz = m_canvas.preferredSize();
		if (sz.w == sz.h == 0)
			sz = { 16,16 };

		if (m_pSkin)
			sz += m_pSkin->contentPadding();

		return sz;
	}

	//____ _cloneContent() ____________________________________________________
	
	void Canvas::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Canvas * pOrg = (Canvas*) _pOrg;
	
	}

	//____ _setSize() _________________________________________________________

	void Canvas::_setSize(const Size& size)
	{
		Widget::_setSize(size);
		m_canvas.setItemSize(size);
	}

	//____ _setSkin() ____________________________________________________________

	void Canvas::_setSkin(Skin * pSkin)
	{
		Size oldPadding = m_pSkin ? m_pSkin->contentPadding() : Size();

		Widget::_setSkin(pSkin);

		Size newPadding = m_pSkin ? m_pSkin->contentPadding() : Size();
		if (newPadding != oldPadding)
			m_canvas.setItemSize(m_size - newPadding);
	}

	//____ _render() __________________________________________________________

	void Canvas::_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip)
	{
		//TODO: Support bitmap being of different surface kind than destination.

		Widget::_render(pDevice, _canvas, _window, _clip);

		Rect itemCanvas = m_pSkin ? m_pSkin->contentRect(_canvas, m_state) : _canvas;

		m_canvas.render(pDevice, itemCanvas, Rect(_clip,itemCanvas));
	}

	//____ _alphaTest() _______________________________________________________

	bool Canvas::_alphaTest(const Coord& ofs)
	{
		if (Widget::_alphaTest(ofs))
			return true;

		Coord itemOfs = ofs; 
		if( m_pSkin )
			itemOfs -= m_pSkin->contentOfs(m_state);

		return m_canvas.alphaTest(itemOfs, m_markOpacity);
	}



} // namespace wg
