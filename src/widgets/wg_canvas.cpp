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
	using namespace Util;

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

	//____ _matchingHeight() ___________________________________________________

	int Canvas::_matchingHeight(int width) const
	{
		//TODO: Take skin padding into account

		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		SizeI sz = m_canvas.preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16*4;

		return width * sz.h / sz.w;
	}

	//____ _matchingWidth() ____________________________________________________

	int Canvas::_matchingWidth(int height) const
	{
		//TODO: Take skin padding into account

		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		SizeI sz = m_canvas.preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16*4;

		return height * sz.w / sz.h;
	}

	//____ _preferredSize() ____________________________________________________

	SizeI Canvas::_preferredSize() const
	{
		SizeI sz = m_canvas.preferredSize();
		if (sz.w == sz.h == 0)
			sz = { 16*4,16*4 };

		if (m_pSkin)
			sz += m_pSkin->_contentPadding();

		return sz;
	}

	//____ _cloneContent() ____________________________________________________

	void Canvas::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
	}

	//____ _resize() _________________________________________________________

	void Canvas::_resize(const SizeI& size)
	{
		Widget::_resize(size);
		m_canvas.setComponentSize(size);
	}

	//____ _setSkin() ____________________________________________________________

	void Canvas::_setSkin(Skin * pSkin)
	{
		SizeI oldPadding = m_pSkin ? m_pSkin->_contentPadding() : SizeI();

		Widget::_setSkin(pSkin);

		SizeI newPadding = m_pSkin ? m_pSkin->_contentPadding() : SizeI();
		if (newPadding != oldPadding)
			m_canvas.setComponentSize(m_size - newPadding);
	}

	//____ _render() __________________________________________________________

	void Canvas::_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window)
	{
		//TODO: Support bitmap being of different surface kind than destination.

		Widget::_render(pDevice, _canvas, _window);

		RectI componentCanvas = m_pSkin ? m_pSkin->_contentRect(_canvas, m_state) : _canvas;

		m_canvas.render(pDevice, componentCanvas); // , RectI(_clip, componentCanvas)); //TODO: Needs to clip against componentCanvas!!!
	}

	//____ _alphaTest() _______________________________________________________

	bool Canvas::_alphaTest(const CoordI& _ofs)
	{
		if (Widget::_alphaTest(_ofs))
			return true;

		CoordI ofs;
		if( m_pSkin )
			ofs -= m_pSkin->_contentOfs(m_state);

		return m_canvas.alphaTest(ofs, m_markOpacity);
	}



} // namespace wg
