
#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>

namespace wg
{

	const TypeInfo RulerLabels::TYPEINFO = { "RulerLabels", &Widget::TYPEINFO };


	//____ constructor ____________________________________________________________

	RulerLabels::RulerLabels()
	{
		m_direction = Direction::Right;
	}

	//____ Destructor _____________________________________________________________

	RulerLabels::~RulerLabels()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& RulerLabels::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ addLabel() ____________________________________________________________

	void RulerLabels::addLabel( const CharSeq& text, TextStyle * pStyle, float offset )
	{
		Label * pLabel = new Label(this);
		pLabel->_text().set(text);
		pLabel->_text().setStyle(pStyle);
		pLabel->offset = offset;

		m_labels.pushBack(pLabel);
		_requestResize();
		_requestRender();
	}

	//____ setDirection() __________________________________________________________

	void RulerLabels::setDirection( Direction direction )
	{
		m_direction = direction;
		_requestResize();
		_requestRender();
	}

	//____ getLabel() ________________________________________________________________

	CTextDisplay_p	RulerLabels::getLabel(int index)
	{
		if( index >= m_labels.size() )
			return CTextDisplay_p();

		return CTextDisplay_p(&m_labels.get(index)->text);
	}



	//____ preferredSize() ________________________________________________________________

	Size RulerLabels::preferredSize() const
	{
		//TODO: calculation of length is not good.

		Size preferred;

		if( m_direction == Direction::Up || m_direction == Direction::Down )
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				Size sz = pLabel->_text()._preferredSize();
				if( sz.w > preferred.w )
					preferred.w = sz.w;

				preferred.h += sz.h;
				pLabel = pLabel->next();
			}
		}
		else
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				Size sz = pLabel->_text()._preferredSize();
				preferred.w += sz.w;

				if( sz.h > preferred.h )
					preferred.h = sz.h;

				pLabel = pLabel->next();
			}
		}

		if( m_pSkin )
			return m_pSkin->sizeForContent(preferred);
		else
			return preferred;
	}


	//____ _render() _____________________________________________________________________

	void RulerLabels::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas,m_state);
		else
			canvas = _canvas;

		if( m_direction == Direction::Up || m_direction == Direction::Down )
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				MU height = pLabel->_text()._size().h;
				MU ofs = canvas.h * pLabel->offset;
				if( m_direction == Direction::Up )
					ofs = canvas.h - ofs;

	/*
	TODO: Reinstate!!!

				switch( pLabel->_text().alignment() )
				{
					case Origo::NorthWest:
					case Origo::North:
					case Origo::NorthEast:
						break;
					case Origo::SouthEast:
					case Origo::South:
					case Origo::SouthWest:
						ofs -= height;
						break;
					case Origo::East:
					case Origo::West:
					case Origo::Center:
						ofs -= height/2;
						break;
				}
	*/
				ofs = ofs.aligned();
				pLabel->_text()._render(pDevice, Rect( canvas.x, canvas.y + ofs, canvas.w, height ) );
				pLabel = pLabel->next();
			}
		}
		else
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				MU width = pLabel->_text()._size().w;
				MU ofs = (canvas.w * pLabel->offset).aligned();
				if( m_direction == Direction::Left )
					ofs = canvas.w - ofs;
	/*
	TODO: Reinstate!

				switch( pLabel->_text().alignment() )
				{
					case Origo::NorthWest:
					case Origo::SouthWest:
					case Origo::West:
						break;
					case Origo::SouthEast:
					case Origo::NorthEast:
					case Origo::East:
						ofs -= width;
						break;
					case Origo::North:
					case Origo::Center:
					case Origo::South:
						ofs -= width/2;
						break;
				}
	*/
				pLabel->_text()._render( pDevice, Rect( canvas.x + ofs, canvas.y, width, canvas.h ) );
				pLabel = pLabel->next();
			}
		}

	}

	//____ _cloneContent() _________________________________________________________________

	void RulerLabels::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );
	}

	//____ _alphaTest() ____________________________________________________________________

	bool RulerLabels::_alphaTest( const Coord& ofs )
	{
		return Widget::_alphaTest(ofs);
	}


	//____ _setState() ______________________________________________________

	void RulerLabels::_setState( State state )
	{
		Widget::_setState(state);

		Label * p = m_labels.first();
		while( p )
		{
			p->_text()._setState(state);
			p = p->next();
		}
	}


	//____ _componentPos() __________________________________________________________

	Coord RulerLabels::_componentPos( const GeoComponent * pComponent ) const
	{
		//TODO: Implement!!!
		return Coord();
	}

	//____ _componentSize() _________________________________________________________

	Size RulerLabels::_componentSize( const GeoComponent * pComponent ) const
	{
		//TODO: Implement!!!
		return Size();
	}

	//____ _componentGeo() __________________________________________________________

	Rect RulerLabels::_componentGeo( const GeoComponent * pComponent ) const
	{
		//TODO: Implement!!!
		return Rect();
	}

	//____ _componentRequestResize() __________________________________________________

	void RulerLabels::_componentRequestResize( const GeoComponent * pComponent )
	{
		//TODO: Implement!!!
		_requestResize();
	}

} // namespace wg
