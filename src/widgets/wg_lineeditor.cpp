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

namespace wg
{
	using namespace Util;

	const char LineEditor::CLASSNAME[] = {"LineEditor"};



	//____ LineEditor() _________________________________________________________________

	LineEditor::LineEditor() : text(this), m_textScrollOfs(0)
	{
		_text().setMaxLines(1);
	}


	//____ Destructor _____________________________________________________________

	LineEditor::~LineEditor()
	{
	}


	//____ isInstanceOf() _________________________________________________________

	bool LineEditor::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * LineEditor::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	LineEditor_p LineEditor::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return LineEditor_p( static_cast<LineEditor*>(pObject) );

		return 0;
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI LineEditor::_preferredSize() const
	{
		//TODO: Use real text size as preferred size instead. They should use a SizeCapsule to limit the size if needed.

		SizeI	contentSize;

		TextStyle * pStyle = _text()._style();

		Font_p pFont = pStyle->font();

		if (pFont)
		{
			pFont->setSize( pStyle->size( StateEnum::Normal ) );
			contentSize.w = pFont->whitespaceAdvance() * 20;
			contentSize.h = pFont->maxAscend() + pFont->maxDescend();
		}
		else
		{
			contentSize.w = 100;
			contentSize.h = 16;
		}

		if( m_pSkin )
			return m_pSkin->_sizeForContent( contentSize );
		else
			return contentSize;
	}

	//____ _render() ________________________________________________________

	void LineEditor::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		RectI canvas = m_pSkin ? m_pSkin->_contentRect( _canvas, m_state ) : _canvas;

		//

		RectI textCanvas(canvas.x - m_textScrollOfs, canvas.y, _text().preferredSize());

		// We need to clip to canvas since textCanvas can go outside our canvas.

		auto pop = limitClipList(pDevice, rawToPixels(canvas) );
		_text().render(pDevice, textCanvas );
		popClipList(pDevice, pop);
	}

	//____ _refresh() _______________________________________________________

	void LineEditor::_refresh( void )
	{
		_text().refresh();
		Widget::_refresh();
	}

	//____ _setState() ______________________________________________________

	void LineEditor::_setState( State state )
	{
		Widget::_setState(state);

		_text().setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void LineEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		_text().receive( pMsg );
	}


	//____ _cloneContent() _______________________________________________________

	void LineEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const LineEditor * pOrg = static_cast<const LineEditor*>(_pOrg);

	}

	//____ _setSkin() _______________________________________________________

	void LineEditor::_setSkin( Skin * pSkin )
	{
		//TODO: Possibly notify text about new canvas size.

		Widget::_setSkin(pSkin);
	}

	//____ _resize() ________________________________________________

	void LineEditor::_resize( const SizeI& size )
	{
		Widget::_resize( size );

		if( m_pSkin )
			_text().setSize( SizeI(_text().preferredSize().w, size.h - m_pSkin->_contentPadding().h ) );
		else
			_text().setSize( SizeI( _text().preferredSize().w, size.h ) );
	}

	//____ _componentPos() __________________________________________________________

	CoordI LineEditor::_componentPos( const GeoComponent * pComponent ) const
	{
		CoordI c(-m_textScrollOfs, 0);

		if( m_pSkin )
			return m_pSkin->_contentOfs( m_state ) + c;
		else
			return c;
	}

	//____ _componentSize() _________________________________________________________

	SizeI LineEditor::_componentSize( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return SizeI( _text().preferredSize().w, m_size.h - m_pSkin->_contentPadding().h );
		else
			return SizeI( _text().preferredSize().w, m_size.h );
	}

	//____ _componentGeo() __________________________________________________________

	RectI LineEditor::_componentGeo( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
		{
			RectI r = m_pSkin->_contentRect( m_size, m_state );
			r.x -= m_textScrollOfs;
			r.w = _text().preferredSize().w;
			return r;
		}
		else
			return RectI( -m_textScrollOfs, 0, _text().preferredSize().w, m_size.h );
	}

	//____ _componentRequestRender() ______________________________________________

	void LineEditor::_componentRequestRender(const GeoComponent * pComponent)
	{
		RectI visible(0, 0, m_size);

		if (m_pSkin)
			visible = m_pSkin->_contentRect(m_size, m_state);

		_requestRender(visible);
	}

	void LineEditor::_componentRequestRender( const GeoComponent * pComponent, const RectI& rect )
	{
		RectI dirt = rect;
		dirt.x -= m_textScrollOfs;

		RectI visible(0,0,m_size);

		if( m_pSkin )
		{
			visible = m_pSkin->_contentRect( m_size, m_state );
			dirt += visible.pos();
		}

		dirt.intersection( dirt, visible );
		if( !dirt.isEmpty() )
			_requestRender( dirt );
	}

	//____ _componentRequestResize() ______________________________________________

	void LineEditor::_componentRequestResize( const GeoComponent * pComponent )
	{
		SizeI preferred = _text().preferredSize();

		int height = m_size.h;
		if( m_pSkin )
			height -= m_pSkin->_contentPadding().h;

		if( preferred.h != height )
			_requestResize();

		_text().setSize( SizeI(preferred.w, height ));	// Component gets the preferred width right away.
	}

	//____ _componentRequestInView() ____________________________________________

	void LineEditor::_componentRequestInView(const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave)
	{
		int scrollOfs = m_textScrollOfs;
		SizeI canvas = pComponent->_size();

		SizeI window = m_size;
		if( m_pSkin )
			window -= m_pSkin->_contentPadding();

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
