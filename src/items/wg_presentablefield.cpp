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

#include <wg_presentablefield.h>

//____ Constructor _____________________________________________________________

WgPresentableField::WgPresentableField( WgPresentableHolder * pHolder ) : WgField( pHolder )
{
	_presenter()->AddField(this);
}

//____ Destructor ______________________________________________________________

WgPresentableField::~WgPresentableField()
{
	_presenter()->RemoveField(this);
}

//____ SetStyle() ______________________________________________________________

void WgPresentableField::SetStyle( const WgTextStylePtr& pStyle )
{
	m_pStyle = pStyle;
	_presenter()->OnStyleChange(this);
}

//____ ClearStyle() ____________________________________________________________

void WgPresentableField::ClearStyle()
{
	m_pStyle = 0;
	_presenter()->OnStyleChange(this);
}

//____ SetPresenter() __________________________________________________________

void WgPresentableField::SetPresenter( const WgTextPresenterPtr& pPresenter )
{
	if( pPresenter == m_pPresenter )
		return;
		
	_presenter()->RemoveField(this);
	m_pPresenter = pPresenter;
	_presenter()->AddField(this);
}

//____ ClearPresenter() ________________________________________________________

void WgPresentableField::ClearPresenter()
{
	if( !m_pPresenter )
		return;
		
	_presenter()->RemoveField(this);
	m_pPresenter = 0;
	_presenter()->AddField(this);
}

//____ SetState() ______________________________________________________________

void WgPresentableField::SetState( WgState state )
{
	if( state == m_state )
		return;

	WgState old = m_state;
	m_state = state;
	_presenter()->OnStateChange( this, state, old );
}

//____ PreferredSize() _________________________________________________________

WgSize WgPresentableField::PreferredSize() const
{
	return _presenter()->PreferredSize(this);
}

//____ MatchingWidth() _________________________________________________________

int WgPresentableField::MatchingWidth( int height ) const
{
	return _presenter()->MatchingWidth(this, height);
}

//____ MatchingHeight() ________________________________________________________

int WgPresentableField::MatchingHeight( int width ) const
{
	return _presenter()->MatchingHeight(this, width);
}

//____ CoordToChar() ___________________________________________________________

int WgPresentableField::CoordToChar( WgCoord pos ) const
{
	return _presenter()->CoordToChar(this,pos);
}

//____ CharToRect() ____________________________________________________________

WgRect WgPresentableField::CharToRect( int charOfs ) const
{
	return _presenter()->CharToRect(this, charOfs);
}

//____ OnRefresh() _____________________________________________________________

void WgPresentableField::OnRefresh()
{
	_presenter()->OnRefresh(this);
}

//____ OnNewSize() _____________________________________________________________

void WgPresentableField::OnNewSize( const WgSize& size )
{
	if( size == m_size )
		return;

	m_size = size;
	_presenter()->OnFieldResize(this,size);
}

//_____ OnRender() _____________________________________________________________

void  WgPresentableField::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
{
	_presenter()->RenderField(this, pDevice, _canvas, _clip);
}

//____ RectForRange() __________________________________________________________

WgRect  WgPresentableField::RectForRange( int ofs, int length ) const
{
	_presenter()->RectForRange(this, ofs, length);
}

//____ Tooltip() _______________________________________________________________

WgString WgPresentableField::Tooltip() const
{
	return _presenter()->Tooltip(this);
}

//____ GetString() ___________________________________________________________________

WgString WgPresentableField::GetString() const
{
	return WgString(&m_charBuffer);
}
