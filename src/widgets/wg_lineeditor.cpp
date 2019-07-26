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

	const char LineEditor::CLASSNAME[] = {"LineEditor"};



	//____ LineEditor() _________________________________________________________________

	LineEditor::LineEditor() : m_text(this), text(&m_text), m_textScrollOfs(0)
	{
		m_text.setMaxLines(1);
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

		TextStyle * pStyle = m_text._style();

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

		RectI canvas;
		if( m_pSkin )
			canvas = m_pSkin->_contentRect( _canvas, m_state );
		else
			canvas = _canvas;
/*
		//TODO: Move this somewhere else where it belongs.

		if( m_text.editMode() != TextEditMode::Static  )
		{
			if( m_text.hasSelection() || m_text.editMode() == TextEditMode::Editable )
			{
				int selBeg = m_text.selectionBegin();
				int selEnd = m_text.selectionEnd();

				int beg = selBeg;
				int end = selEnd;

				if( beg > end )
					std::swap( beg, end );

				if( beg > 0 )
					beg--;

				if( end < m_text.length() )
					end++;

				RectI r = m_text.rectForRange( beg, end - beg );

				int prio1 = r.x;
				int prio2 = r.x + r.w;

				if( selBeg < selEnd )
					std::swap( prio1, prio2 );

				if( prio2 < m_textScrollOfs )
					m_textScrollOfs = prio2;

				if (prio2 > m_textScrollOfs + canvas.w)
					m_textScrollOfs = prio2 - canvas.w;

				if (prio1 < m_textScrollOfs)
					m_textScrollOfs = prio1;

				if (prio1 > m_textScrollOfs + canvas.w)
					m_textScrollOfs = prio1 - canvas.w;
			}
		}
		else
			m_textScrollOfs = 0;
*/
		//

		RectI textCanvas(canvas.x - m_textScrollOfs, canvas.y, m_text.preferredSize());

//		RectI textClip(_clip, canvas);		//TODO: Text should not be rendered outside widgets contentRect. Maybe do this inside m_text.render?

		m_text.render(pDevice, textCanvas );
	}

	//____ _refresh() _______________________________________________________

	void LineEditor::_refresh( void )
	{
		m_text.refresh();
		Widget::_refresh();
	}

	//____ _setState() ______________________________________________________

	void LineEditor::_setState( State state )
	{
		Widget::_setState(state);

		m_text.setState(state);
		_requestRender(); //TODO: Only requestRender if skin or text appearance has changed.
	}


	//____ _receive() ______________________________________________________________

	void LineEditor::_receive( Msg * pMsg )
	{
		Widget::_receive( pMsg );
		m_text.receive( pMsg );
	}


	//____ _cloneContent() _______________________________________________________

	void LineEditor::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const LineEditor * pOrg = static_cast<const LineEditor*>(_pOrg);

		m_text = pOrg->m_text;
	}

	//____ _setSkin() _______________________________________________________

	void LineEditor::_setSkin( Skin * pSkin )
	{
		//TODO: Possibly notify text about new canvas size.

		Widget::_setSkin(pSkin);
	}

	//____ _setSize() ________________________________________________

	void LineEditor::_setSize( const SizeI& size )
	{
		Widget::_setSize( size );

		if( m_pSkin )
			m_text.setSize( SizeI(m_text.preferredSize().w, size.h - m_pSkin->_contentPadding().h ) );
		else
			m_text.setSize( SizeI( m_text.preferredSize().w, size.h ) );
	}

	//____ _componentPos() __________________________________________________________

	CoordI LineEditor::_componentPos( const Component * pComponent ) const
	{
		CoordI c(-m_textScrollOfs, 0);

		if( m_pSkin )
			return m_pSkin->_contentOfs( m_state ) + c;
		else
			return c;
	}

	//____ _componentSize() _________________________________________________________

	SizeI LineEditor::_componentSize( const Component * pComponent ) const
	{
		if( m_pSkin )
			return SizeI( m_text.preferredSize().w, m_size.h - m_pSkin->_contentPadding().h );
		else
			return SizeI( m_text.preferredSize().w, m_size.h );
	}

	//____ _componentGeo() __________________________________________________________

	RectI LineEditor::_componentGeo( const Component * pComponent ) const
	{
		if( m_pSkin )
		{
			RectI r = m_pSkin->_contentRect( m_size, m_state );
			r.x -= m_textScrollOfs;
			r.w = m_text.preferredSize().w;
			return r;
		}
		else
			return RectI( -m_textScrollOfs, 0, m_text.preferredSize().w, m_size.h );
	}

	//____ _componentRequestRender() ______________________________________________

	void LineEditor::_componentRequestRender(const Component * pComponent)
	{
		RectI visible(0, 0, m_size);

		if (m_pSkin)
			visible = m_pSkin->_contentRect(m_size, m_state);

		_requestRender(visible);
	}

	void LineEditor::_componentRequestRender( const Component * pComponent, const RectI& rect )
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

	void LineEditor::_componentRequestResize( const Component * pComponent )
	{
		SizeI preferred = m_text.preferredSize();

		int height = m_size.h;
		if( m_pSkin )
			height -= m_pSkin->_contentPadding().h;

		if( preferred.h != height )
			_requestResize();

		m_text.setSize( SizeI(preferred.w, height ));	// Component gets the preferred width right away.
	}

	//____ _componentRequestInView() ____________________________________________

	void LineEditor::_componentRequestInView(const Component * pComponent, const RectI& preferred, const RectI& prio)
	{
		int scrollOfs = m_textScrollOfs;
		SizeI canvas = pComponent->_size();

		SizeI window = m_size;
		if( m_pSkin )
			window -= m_pSkin->_contentPadding();

		if (scrollOfs > 0 && canvas.w - scrollOfs < window.w)
			scrollOfs = canvas.w < window.w ? 0 : canvas.w - window.w;

		if( prio.x < scrollOfs )
			scrollOfs = prio.x;

		if( prio.x + prio.w > scrollOfs + window.w )
			scrollOfs = prio.x + prio.w - window.w;

		if (scrollOfs != m_textScrollOfs)
		{
			m_textScrollOfs = scrollOfs;
			_requestRender();
		}
	}


} // namespace wg
