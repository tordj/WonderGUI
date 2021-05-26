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

#include <wg_lineeditor.h>
#include <wg_key.h>
#include <wg_gfxdevice.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo LineEditor::TYPEINFO = { "LineEditor", &Widget::TYPEINFO };



	//____ LineEditor() _________________________________________________________________

	LineEditor::LineEditor() : text(this), m_textScrollOfs(0)
	{
		text.setMaxLines(1);
	}


	//____ Destructor _____________________________________________________________

	LineEditor::~LineEditor()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& LineEditor::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _preferredSize() _____________________________________________________________

	SizeSPX LineEditor::_preferredSize(int scale) const
	{
		scale = _fixScale(scale);

		//TODO: Use real text size as preferred size instead. They should use a SizeCapsule to limit the size if needed.

		SizeSPX	contentSize;

		TextStyle * pStyle = OO(text)._style();

		Font_p pFont = pStyle->font();

		if (pFont)
		{
			pFont->setSize( pStyle->size( StateEnum::Normal ) );
			contentSize.w = pFont->whitespaceAdvance() * 20;
			contentSize.h = pFont->maxAscend() + pFont->maxDescend();
		}
		else
		{
			contentSize.w = 100*64;
			contentSize.h = 16*64;
		}

		contentSize = alignUp(contentSize);

		return OO(skin)._sizeForContent( contentSize, scale );
	}

	//____ _render() ________________________________________________________

	void LineEditor::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectSPX canvas = OO(skin)._contentRect(_canvas, m_scale, m_state);

		//

		RectSPX textCanvas(canvas.x - m_textScrollOfs, canvas.y, OO(text)._preferredSize(m_scale));

		// We need to clip to canvas since textCanvas can go outside our canvas.

		auto pop = limitClipList(pDevice, canvas );
		OO(text)._render(pDevice, textCanvas );
		popClipList(pDevice, pop);
	}

	//____ _refresh() _______________________________________________________

	void LineEditor::_refresh( void )
	{
		OO(text)._refresh();
		Widget::_refresh();
	}

	//____ _setState() ______________________________________________________

	void LineEditor::_setState( State state )
	{
		Widget::_setState(state);

		OO(text)._setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void LineEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		OO(text)._receive( pMsg );
	}


	//____ _cloneContent() _______________________________________________________

	void LineEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

//		const LineEditor * pOrg = static_cast<const LineEditor*>(_pOrg);

	}

	//____ _resize() ________________________________________________

	void LineEditor::_resize( const SizeSPX& size, int scale )
	{
		Widget::_resize( size, scale );

		OO(text)._setSize( SizeSPX( OO(text)._preferredSize(m_scale).w, size.h - OO(skin)._contentPaddingSize(m_scale).h ), m_scale );
	}

	//____ _componentPos() __________________________________________________________

	CoordSPX LineEditor::_componentPos( const GeoComponent * pComponent ) const
	{
		if (pComponent != &text)
			return CoordSPX();

		CoordSPX c(-m_textScrollOfs, 0);

		return OO(skin)._contentOfs( m_scale, m_state ) + c;
	}

	//____ _componentSize() _________________________________________________________

	SizeSPX LineEditor::_componentSize( const GeoComponent * pComponent ) const
	{
		if (pComponent != &text)
			return m_size;

		return SizeSPX( OO(text)._preferredSize(m_scale).w, m_size.h - OO(skin)._contentPaddingSize(m_scale).h );
	}

	//____ _componentGeo() __________________________________________________________

	RectSPX LineEditor::_componentGeo( const GeoComponent * pComponent ) const
	{
		if (pComponent != &text)
			return m_size;

		RectSPX r = OO(skin)._contentRect( m_size, m_scale, m_state );
		r.x -= m_textScrollOfs;
		r.w = OO(text)._preferredSize(m_scale).w;
		return r;
	}

	//____ _componentRequestRender() ______________________________________________

	void LineEditor::_componentRequestRender(const GeoComponent * pComponent)
	{
		if (pComponent != &text)
			return _requestRender();

		_requestRender(OO(skin)._contentRect(m_size, m_scale, m_state));
	}

	void LineEditor::_componentRequestRender( const GeoComponent * pComponent, const RectSPX& rect )
	{
		if (pComponent != &text)
			return _requestRender(rect);

		RectSPX dirt = rect;
		dirt.x -= m_textScrollOfs;

		RectSPX visible = OO(skin)._contentRect( m_size, m_scale, m_state );
		dirt += visible.pos();

		dirt.intersection( dirt, visible );
		if( !dirt.isEmpty() )
			_requestRender( dirt );
	}

	//____ _componentRequestResize() ______________________________________________

	void LineEditor::_componentRequestResize( const GeoComponent * pComponent )
	{
		if (pComponent != &text)
			return Widget::_componentRequestResize(pComponent);

		SizeSPX preferred = OO(text)._preferredSize(m_scale);

		spx height = m_size.h - OO(skin)._contentPaddingSize(m_scale).h;

		if( preferred.h != height )
			_requestResize();

		OO(text)._setSize( SizeSPX(preferred.w, height ),m_scale);	// Component gets the preferred width right away.
	}

	//____ _componentRequestInView() ____________________________________________

	void LineEditor::_componentRequestInView(const GeoComponent * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave)
	{
		spx scrollOfs = m_textScrollOfs;
		SizeSPX canvas = _componentSize(pComponent);

		SizeSPX window = m_size - OO(skin)._contentPaddingSize(m_scale);

		if (scrollOfs > 0 && canvas.w - scrollOfs < window.w)
			scrollOfs = canvas.w < window.w ? 0 : canvas.w - window.w;

		if( mustHave.x < scrollOfs )
			scrollOfs = mustHave.x;

		if( mustHave.x + mustHave.w > scrollOfs + window.w )
			scrollOfs = mustHave.x + mustHave.w - window.w;

		if (scrollOfs != m_textScrollOfs)
		{
			m_textScrollOfs = scrollOfs;
			_requestRender();
		}
	}


} // namespace wg