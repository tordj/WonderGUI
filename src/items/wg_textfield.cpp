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

#include <wg_textfield.h>

//____ Constructor _____________________________________________________________

WgTextField::WgTextField( WgTextHolder * pHolder ) : WgField( pHolder )
{
	_presenter()->InitTextField(this);
}

//____ Destructor ______________________________________________________________

WgTextField::~WgTextField()
{
	_presenter()->ExitTextField(this);
}


//____ SetStyle() ______________________________________________________________

void WgTextField::SetStyle( const WgTextStylePtr& pStyle )
{
	m_pStyle = pStyle;
	_presenter()->OnStyleChange(this);
}

//____ ClearStyle() ____________________________________________________________

void WgTextField::ClearStyle()
{
	m_pStyle = 0;
	_presenter()->OnStyleChange(this);
}

//____ SetPresenter() __________________________________________________________

void WgTextField::SetPresenter( const WgTextPresenterPtr& pPresenter )
{
	if( pPresenter == m_pPresenter )
		return;
		
	_presenter()->ExitTextField(this);
	m_pPresenter = pPresenter;
	_presenter()->InitTextField(this);
}

//____ ClearPresenter() ________________________________________________________

void WgTextField::ClearPresenter()
{
	if( !m_pPresenter )
		return;
		
	_presenter()->ExitTextField(this);
	m_pPresenter = 0;
	_presenter()->InitTextField(this);
}

//____ Clear() _________________________________________________________________

void WgTextField::Clear()
{
	int removed = m_text.Length();
	m_text.Clear();
	_presenter()->OnTextModified(this, 0, removed, 0 );
}

//___ Set() ____________________________________________________________________

void WgTextField::Set( const WgCharSeq& seq )
{
	int removed = m_text.Length();
	m_text = seq;
	_presenter()->OnTextModified(this, 0, removed, m_text.Length() );
}

void WgTextField::Set( const WgCharBuffer * buffer )
{
	int removed = m_text.Length();
	m_text = * buffer;
	_presenter()->OnTextModified(this, 0, removed, m_text.Length() );
}

void WgTextField::Set( const WgString& str )
{
	int removed = m_text.Length();
	m_text = str;
	_presenter()->OnTextModified(this, 0, removed, m_text.Length() );
}

//____ Append() ________________________________________________________________

int WgTextField::Append( const WgCharSeq& seq )
{
	int ofs = m_text.Length();
	int len = m_text.PushBack(seq);
	_presenter()->OnTextModified(this, ofs, 0, len );
}

//____ Insert() ________________________________________________________________

int WgTextField::Insert( int ofs, const WgCharSeq& seq )
{
	m_text.Insert(ofs,seq);
	_presenter()->OnTextModified(this, ofs, 0, seq.Length() );
}

//____ Replace() _______________________________________________________________

int WgTextField::Replace( int ofs, int nDelete, const WgCharSeq& seq )
{
	m_text.Replace(ofs,nDelete,seq);
	_presenter()->OnTextModified(this, ofs, nDelete, seq.Length() );
}

//____ Delete() ________________________________________________________________

int WgTextField::Delete( int ofs, int len )
{
	m_text.Delete(ofs,len);
	_presenter()->OnTextModified(this, ofs, len, 0 );	
}

//____ SetState() ______________________________________________________________

void WgTextField::SetState( WgState state )
{
	if( state == m_state )
		return;

	WgState old = m_state;
	m_state = state;
	_presenter()->OnStateChange( this, state, old );
}

//____ PreferredSize() _________________________________________________________

WgSize WgTextField::PreferredSize() const
{
	return _presenter()->PreferredSize(this);
}

//____ MatchingWidth() _________________________________________________________

int WgTextField::MatchingWidth( int height ) const
{
	return _presenter()->MatchingWidth(this, height);
}

//____ MatchingHeight() ________________________________________________________

int WgTextField::MatchingHeight( int width ) const
{
	return _presenter()->MatchingHeight(this, width);
}

//____ CoordToChar() ___________________________________________________________

int WgTextField::CoordToChar( WgCoord pos ) const
{
	return _presenter()->CoordToChar(this,pos);
}

//____ CharToRect() ____________________________________________________________

WgRect WgTextField::CharToRect( int charOfs ) const
{
	return _presenter()->CharToRect(this, charOfs);
}

//____ OnRefresh() _____________________________________________________________

void WgTextField::OnRefresh()
{
	return _presenter()->OnRefresh(this);
}

//____ OnNewSize() _____________________________________________________________

void WgTextField::OnNewSize( const WgSize& size )
{
	if( size == m_size )
		return;

	m_size = size;
	_presenter()->OnFieldResize(this,size);
}

//____ OnEvent() _______________________________________________________________

bool WgTextField::OnEvent( const WgEventPtr& pEvent, WgEventHandler * pEventHandler, const WgRect& container )
{
	
}

//_____ OnRender() _____________________________________________________________

void  WgTextField::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
{
	_presenter()->RenderTextField(this, pDevice, _canvas, _clip);
}

//____ Get() ___________________________________________________________________

WgString WgTextField::Get() const
{
	return WgString(&m_text);
}

//____ RectForRange() __________________________________________________________

WgRect  WgTextField::RectForRange( int ofs, int length ) const
{
	_presenter()->RectForRange(this, ofs, length);
}
