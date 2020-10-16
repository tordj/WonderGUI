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
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Canvas::TYPEINFO = { "Canvas", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Canvas::Canvas() : canvas(this)
	{
	}

	//____ Destructor _____________________________________________________________

	Canvas::~Canvas()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Canvas::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ matchingHeight() ___________________________________________________

	MU Canvas::matchingHeight(MU width) const
	{
		//TODO: Take skin padding into account

		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		Size sz = OO(canvas)._preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16*4;

		return width * sz.h / sz.w;
	}

	//____ matchingWidth() ____________________________________________________

	MU Canvas::matchingWidth(MU height) const
	{
		//TODO: Take skin padding into account

		// Try to maintain aspect ratio of preferred size. If we can't get an aspect
		// ratio we simply stick to 16 pixels.

		Size sz = OO(canvas)._preferredSize();

		if (sz.h == 0 || sz.w == 0)
			return 16*4;

		return height * sz.w / sz.h;
	}

	//____ preferredSize() ____________________________________________________

	Size Canvas::preferredSize() const
	{
		Size sz = OO(canvas)._preferredSize();
		if (sz.w == sz.h == 0)
			sz = { 16*4,16*4 };

		sz += m_skin.contentPaddingSize();

		return sz;
	}

	//____ _cloneContent() ____________________________________________________

	void Canvas::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
	}

	//____ _resize() _________________________________________________________

	void Canvas::_resize(const Size& size)
	{
		Widget::_resize(size);
		OO(canvas)._setComponentSize(size);
	}

	//____ setSkin() ____________________________________________________________

	void Canvas::setSkin(Skin * pSkin)
	{
		Size oldPadding = m_skin.contentPaddingSize();
		Widget::setSkin(pSkin);
		Size newPadding = m_skin.contentPaddingSize();

		if (newPadding != oldPadding)
			OO(canvas)._setComponentSize(m_size - newPadding);
	}

	//____ _render() __________________________________________________________

	void Canvas::_render(GfxDevice * pDevice, const Rect& canvas, const Rect& window)
	{
		//TODO: Support bitmap being of different surface kind than destination.

		Widget::_render(pDevice, canvas, window);

		Rect componentCanvas = m_skin.contentRect(canvas, m_state);

		OO(Canvas::canvas)._render(pDevice, componentCanvas); // , RectI(_clip, componentCanvas)); //TODO: Needs to clip against componentCanvas!!!
	}

	//____ _alphaTest() _______________________________________________________

	bool Canvas::_alphaTest(const Coord& _ofs)
	{
		if (Widget::_alphaTest(_ofs))
			return true;

		Coord ofs = _ofs - m_skin.contentOfs(m_state);

		return OO(canvas)._alphaTest(ofs, m_markOpacity);
	}

} // namespace wg
