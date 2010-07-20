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


#include <wg_item.h>
#include <wg_interface_itemholder.h>
#include <wg_emitter.h>
#include <wg_signals.h>


using namespace WgSignal;

//____ WgItem() _______________________________________________________________

WgItem::WgItem()
{
	m_width		= 0;
	m_height	= 0;
	m_pMyHolder	= 0;
	m_id		= 0;
	m_mode		= WG_MODE_NORMAL;
	m_bVisible	= true;
}

WgItem::WgItem( Sint64 id )
{
	m_width		= 0;
	m_height	= 0;
	m_pMyHolder	= 0;
	m_id		= id;
	m_mode		= WG_MODE_NORMAL;
	m_bVisible	= true;
}


//____ ~WgItem() ______________________________________________________________

WgItem::~WgItem()
{
	if( m_pMyHolder )
	{
		disconnect();
		m_pMyHolder->m_nItems--;
		m_pMyHolder->refreshItems();
	}
}

//____ SetVisible() _________________________________________________________
void WgItem::SetVisible(bool bVisible)
{
	if(m_bVisible != bVisible)
	{
		m_bVisible = bVisible;
		if( m_pMyHolder )
			m_pMyHolder->ItemModified(this, 0, 0 );

		if( m_mode == WG_MODE_SELECTED )
		{
			m_mode = WG_MODE_NORMAL;
			if( m_pMyHolder )
				m_pMyHolder->ItemMarkChanged(this, false);
		}
	}
}

//____ AdaptToWidth() _________________________________________________________
/**
	This callback gives the item a chance to change its requested width/height in response
	to a change in displayed width.

	A WrapText-item could for example change itself to get a width less or equal to
	displayed width and compensate with a higher height.
*/

void WgItem::AdaptToWidth( Uint32 width )
{
}

//____ AdaptToHeight() ________________________________________________________
/**
	This callback gives the item a chance to change its requested width/height in response
	to a change in displayed height.
*/

void WgItem::AdaptToHeight( Uint32 height )
{
}

//____ ActionRespond() ________________________________________________________

void WgItem::ActionRespond( WgEmitter * _pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int		btn = _button_key;

	switch( _action )
	{

		case WgInput::POINTER_ENTER:
			if(m_mode == WG_MODE_NORMAL)
				m_mode = WG_MODE_MARKED;
			_pEmitter->Emit( ItemEnter(), this );
			if(m_mode == WG_MODE_NORMAL)
				m_mode = WG_MODE_MARKED;
			break;

		case WgInput::POINTER_OVER:
			_pEmitter->Emit( ItemOver(), this );
			break;

		case WgInput::POINTER_EXIT:
			if(m_mode == WG_MODE_MARKED)
				m_mode = WG_MODE_NORMAL;
			_pEmitter->Emit( ItemExit(), this );
			if(m_mode == WG_MODE_MARKED)
				m_mode = WG_MODE_NORMAL;
			break;

		case WgInput::BUTTON_PRESS:
			_pEmitter->Emit( ItemPress(btn), this );
			break;

		case WgInput::BUTTON_DOWN:
			_pEmitter->Emit( ItemDown(btn), this );
			break;

		case WgInput::BUTTON_REPEAT:
			_pEmitter->Emit( ItemRepeat(btn), this );
			break;

		case WgInput::BUTTON_RELEASE:
			_pEmitter->Emit( ItemRelease(btn), this );
			break;

		case WgInput::BUTTON_CLICK:
			_pEmitter->Emit( ItemClick(btn), this );
			break;

		case WgInput::BUTTON_DOUBLECLICK:
			_pEmitter->Emit( ItemDoubleClick(btn), this );
			break;

        default:
            break;
	}
}

//____ Render() _______________________________________________________________

void WgItem::Render( const WgRect& _window, const WgRect& _clip )
{
}

//____ Clone() ________________________________________________________________

WgItem * WgItem::Clone( WgItem * _pClone )
{
//	if( _pClone == 0 )
//		_pClone = new WgItem();

	_pClone->m_width = m_width;
	_pClone->m_height = m_height;

	return _pClone;
}

//____ Refresh() ______________________________________________________________

void WgItem::Refresh( void )
{
}

//____ GetMarkedItem() ________________________________________________________

WgItem * WgItem::GetMarkedItem( Uint32 _x, Uint32 _y )
{
//	if( _x >= m_width && _y >= m_height )
//		return 0;

	return this;
}

//____ Enable() _______________________________________________________________

void WgItem::Enable( bool bEnable )
{
}

//____ Modified() _____________________________________________________________

void WgItem::Modified( Sint32 widthDiff, Sint32 heightDiff )
{
	if( m_pMyHolder )
		m_pMyHolder->ItemModified(this, widthDiff, heightDiff );
}

//____ Select() _______________________________________________________________

bool WgItem::Select()
{
	if( m_mode == WG_MODE_DISABLED )
		return false;

	if( m_mode != WG_MODE_SELECTED )
	{
		m_mode = WG_MODE_SELECTED;
		if( m_pMyHolder )
		{
			m_pMyHolder->ItemModified(this, 0, 0 );
			m_pMyHolder->ItemMarkChanged(this, true);
		}
	}
	return true;
}

//____ Unselect() _____________________________________________________________

void WgItem::Unselect()
{
	if( m_mode == WG_MODE_SELECTED )
	{
		m_mode = WG_MODE_NORMAL;
		if( m_pMyHolder )
		{
			m_pMyHolder->ItemModified(this, 0, 0 );
			m_pMyHolder->ItemMarkChanged(this, false);
		}
	}
}

//____ Enable() _______________________________________________________________

void WgItem::Enable()
{
	if( m_mode == WG_MODE_DISABLED )
	{
		m_mode = WG_MODE_NORMAL;
		if( m_pMyHolder )
			m_pMyHolder->ItemModified(this, 0, 0 );
	}
}

//____ Disable() ______________________________________________________________

void WgItem::Disable()
{
	if( m_mode != WG_MODE_DISABLED )
	{
		if( m_mode == WG_MODE_SELECTED && m_pMyHolder )
			m_pMyHolder->ItemMarkChanged(this, false);

		m_mode = WG_MODE_DISABLED;

		if( m_pMyHolder )
			m_pMyHolder->ItemModified(this, 0, 0 );
	}
}

//____ GrabInputFocus() _______________________________________________________

bool WgItem::GrabInputFocus() 
{ 
	if( m_pMyHolder ) 
		return m_pMyHolder->SetFocusedItem(this); 
	
	return false; 
}

//____ RemoveInputFocus() _____________________________________________________

bool WgItem::RemoveInputFocus() 
{ 
	if( m_pMyHolder ) 
		return m_pMyHolder->UnsetFocusedItem(this); 

	return false; 
}
