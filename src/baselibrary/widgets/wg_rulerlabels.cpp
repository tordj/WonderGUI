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

#include <wg_rulerlabels.h>
#include <wg_gfxdevice.h>
#include <wg_internal.h>

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
		OO(pLabel->text).set(text);
		OO(pLabel->text).setStyle(pStyle);
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
				Size sz = OO(pLabel->text)._preferredSize();
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
				Size sz = OO(pLabel->text)._preferredSize();
				preferred.w += sz.w;

				if( sz.h > preferred.h )
					preferred.h = sz.h;

				pLabel = pLabel->next();
			}
		}

		return OO(skin)._sizeForContent(preferred);
	}


	//____ _render() _____________________________________________________________________

	void RulerLabels::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		Rect canvas = OO(skin)._contentRect(_canvas,m_state);

		if( m_direction == Direction::Up || m_direction == Direction::Down )
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				MU height = OO(pLabel->text)._size().h;
				MU ofs = canvas.h * pLabel->offset;
				if( m_direction == Direction::Up )
					ofs = canvas.h - ofs;

	/*
	TODO: Reinstate!!!

				switch( OO(pLabel->text).placement() )
				{
					case Placement::NorthWest:
					case Placement::North:
					case Placement::NorthEast:
						break;
					case Placement::SouthEast:
					case Placement::South:
					case Placement::SouthWest:
						ofs -= height;
						break;
					case Placement::East:
					case Placement::West:
					case Placement::Center:
						ofs -= height/2;
						break;
				}
	*/
				ofs = ofs.aligned();
				OO(pLabel->text)._render(pDevice, Rect( canvas.x, canvas.y + ofs, canvas.w, height ) );
				pLabel = pLabel->next();
			}
		}
		else
		{
			Label * pLabel = m_labels.first();
			while( pLabel )
			{
				MU width = OO(pLabel->text)._size().w;
				MU ofs = (canvas.w * pLabel->offset).aligned();
				if( m_direction == Direction::Left )
					ofs = canvas.w - ofs;
	/*
	TODO: Reinstate!

				switch( OO(pLabel->text).placement() )
				{
					case Placement::NorthWest:
					case Placement::SouthWest:
					case Placement::West:
						break;
					case Placement::SouthEast:
					case Placement::NorthEast:
					case Placement::East:
						ofs -= width;
						break;
					case Placement::North:
					case Placement::Center:
					case Placement::South:
						ofs -= width/2;
						break;
				}
	*/
				OO(pLabel->text)._render( pDevice, Rect( canvas.x + ofs, canvas.y, width, canvas.h ) );
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
			OO(p->text)._setState(state);
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
