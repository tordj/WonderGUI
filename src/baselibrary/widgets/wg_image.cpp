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

#include <wg_image.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_surface.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Image::TYPEINFO = { "Image", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Image::Image()
	{
	}

	Image::Image(const Blueprint& bp)
	{
		_initFromBlueprint(bp);

		m_pSurface	= bp.image;
		m_rect = bp.imageRect;
		m_imageMarkAlpha = bp.imageMarkAlpha;
		
		if( m_rect.isEmpty() && m_pSurface )
			m_rect = m_pSurface->pointSize();
	}


	//____ Destructor _____________________________________________________________

	Image::~Image()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Image::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setImage() _____________________________________________________________

	void Image::setImage( Surface * pSurface, const Rect& rect )
	{
		if( pSurface != m_pSurface || rect != m_rect )
		{
			bool bResize = false;
			if( rect.w != m_rect.w || rect.h != m_rect.h )
				bResize = true;

			m_pSurface = pSurface;

			if( pSurface )
				m_rect = Rect::getIntersection( rect, Rect(pSurface->pointSize()) );
			else
				m_rect.clear();

			if( bResize )
				_requestResize();
			_requestRender();
		}
	}

	void Image::setImage( Surface * pSurface )
	{
		if( pSurface != m_pSurface )
		{
			bool bResize = false;
			if( !pSurface || !m_pSurface || pSurface->pointSize() != m_pSurface->pointSize() )
				bResize = true;

			m_pSurface = pSurface;

			if( pSurface )
				m_rect = pSurface->pointSize();
			else
				m_rect.clear();

			if( bResize )
				_requestResize();
			_requestRender();
		}
	}

	//____ _defaultSize() _____________________________________________________________

	SizeSPX Image::_defaultSize(int scale) const
	{

		if (m_pSurface)
		{
			scale = _fixScale(scale);
			return m_skin.sizeForContent( align(ptsToSpx(m_rect.size(),scale)), scale );
		}
		else
			return Widget::_defaultSize(scale);
	}

	//____ _render() _____________________________________________________________

	void Image::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		if( m_pSurface && !m_rect.isEmpty() )
		{
			RectSPX dest = m_skin.contentRect( _canvas, m_scale, state() );

			pDevice->setBlitSource(m_pSurface);
			pDevice->stretchBlit( dest, m_rect );		// Higher precision source coordinates than to use ptsToSpx().
		}
	}

	//____ _alphaTest() ___________________________________________________________

	bool Image::_alphaTest( const CoordSPX& ofs )
	{
		if( m_pSurface && !m_rect.isEmpty() )
		{
			RectSPX dest = m_skin.contentRect( m_size, m_scale, state() );

			if( Util::markTestStretchRect( ofs, m_pSurface, ptsToSpx(m_rect, m_pSurface->scale()), dest, m_imageMarkAlpha ) )
				return true;
		}

		return Widget::_alphaTest(ofs);
	}

} // namespace wg
