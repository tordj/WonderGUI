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

#include <wg_gizmo_checkbox.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_util.h>
#ifdef WG_TNG
#	include <wg_root.h>
#	include <wg_eventhandler.h>
#endif
#include <assert.h>

using namespace WgSignal;

static const char	c_gizmoType[] = {"CheckBox"};



//____ WgGizmoCheckbox() _________________________________________________________________

WgGizmoCheckbox::WgGizmoCheckbox()
{
	m_bChecked			= false;
	m_bOver				= false;
	m_bPressed			= false;
	m_bFlipOnRelease	= false;

	m_pText				= &m_text;
	m_text.setHolder( this );
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgGizmoCheckbox::~WgGizmoCheckbox()
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoCheckbox::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoCheckbox::GetMyType( void )
{
	return c_gizmoType;
}


//____ SetSource() ____________________________________________________________

bool WgGizmoCheckbox::SetSource( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked )
{
	m_pBlockUnchecked	= pUnchecked;
	m_pBlockChecked		= pChecked;

	_onRefresh();
	return true;
}

//____ SetIcons() ______________________________________________________________

void WgGizmoCheckbox::SetIcons( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked,
								const WgOrientation& orientation, WgBorders borders, float scale,
								bool bPushText )
{
	m_pIconUnchecked	= pUnchecked;
	m_pIconChecked		= pChecked;
	m_iconOrientation	= orientation;
	m_iconScale			= scale;
	m_bIconPushText		= bPushText;
	m_iconBorders		= borders;
	_onRefresh();
}

//____ SetIcons() ______________________________________________________________

void WgGizmoCheckbox::SetIcons( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked )
{
	m_pIconUnchecked	= pUnchecked;
	m_pIconChecked		= pChecked;
	_onRefresh();
}

//____ SetCheckedIcon() _______________________________________________________

void WgGizmoCheckbox::SetCheckedIcon( const WgBlockSetPtr& pChecked )
{
	m_pIconChecked		= pChecked;
	_onRefresh();
}

//____ SetUncheckedIcon() _____________________________________________________

void WgGizmoCheckbox::SetUncheckedIcon( const WgBlockSetPtr& pUnchecked )
{
	m_pIconUnchecked	= pUnchecked;
	_onRefresh();
}



//____ SetState() _____________________________________________________________

bool WgGizmoCheckbox::SetState( bool _state )
{
	if( m_bChecked != _state )
	{
		m_bChecked = _state;

		if( _state )
			Emit( Set() );
		else
			Emit( Unset() );

		Emit( Flipped(), m_bChecked );
#ifdef WG_TNG
		WgEventHandler * pHandler = EventHandler();
		if( pHandler )
		{
			if( _state )
				pHandler->QueueEvent( new WgEvent::CheckboxCheck( this ) );
			else
				pHandler->QueueEvent( new WgEvent::CheckboxUncheck( this ) );

			pHandler->QueueEvent( new WgEvent::CheckboxToggle(this, _state ) );
		}
#endif
		_requestRender();
	}

	return true;
}

//____ SetFlipOnRelease() _____________________________________________________

void WgGizmoCheckbox::SetFlipOnRelease( bool bFlipOnRelease )
{
	m_bFlipOnRelease = bFlipOnRelease;
}


//____ DefaultSize() __________________________________________________

WgSize WgGizmoCheckbox::DefaultSize() const
{
	WgSize iconBestSize;
	WgSize bgBestSize;
	WgSize textBestSize;

	if( m_text.nbChars() > 0 )
		textBestSize = m_text.unwrappedSize();

	if( m_pBlockUnchecked )
	{
		bgBestSize = m_pBlockUnchecked->Size();
		textBestSize += m_pBlockUnchecked->ContentBorders();
	}

	if( m_pIconUnchecked )
	{
		iconBestSize = m_pIconUnchecked->Size() + m_iconBorders.Size();

		//TODO: Add magic for how icon influences textBestSize based on origo, iconBorders, iconScale and bgBestSize
	}

	WgSize bestSize = WgSize::Max( WgSize::Max(iconBestSize,bgBestSize), textBestSize);

	return bestSize;
}


//____ _onEnable() _________________________________________________
void WgGizmoCheckbox::_onEnable()
{
	_requestRender();
}

//____ _onDisable() _________________________________________________
void WgGizmoCheckbox::_onDisable()
{
	m_bOver = false;
	m_bPressed = false;

	_requestRender();
}

//____ _onEvent() _____________________________________________________________

#ifdef WG_TNG
void WgGizmoCheckbox::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSE_ENTER:
			if( !m_bOver )
			{
				m_bOver = true;
				_requestRender();
			}
			break;

		case WG_EVENT_MOUSE_LEAVE:
			if( m_bOver )
			{
				m_bOver = false;
				_requestRender();
			}
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 && !m_bPressed )
			{
				if( !m_bFlipOnRelease )
					SetState( !m_bChecked );
				m_bPressed = true;
				_requestRender();
			}
			break;
		}

		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 && m_bPressed )
			{
				m_bPressed = false;
				if( m_bFlipOnRelease )
					SetState( !m_bChecked );
				_requestRender();
			}
			break;
		}

        default:
            break;
	}
}
#endif

//____ _onAction() _________________________________________________

void WgGizmoCheckbox::_onAction( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	switch( _action )
	{
		case WgInput::POINTER_ENTER:
			if( !m_bOver )
			{
				m_bOver = true;
				_requestRender();
			}
			break;

		case WgInput::POINTER_EXIT:
			if( m_bOver )
			{
				m_bOver = false;
				_requestRender();
			}
			break;

		case WgInput::BUTTON_PRESS:
			if( _button_key == 1 && !m_bPressed )
			{
				m_bPressed = true;
				if( !m_bFlipOnRelease )
					SetState( !m_bChecked );
				_requestRender();
			}
			break;

		case WgInput::BUTTON_RELEASE:
		case WgInput::BUTTON_RELEASE_OUTSIDE:
			if( _button_key == 1 && m_bPressed )
			{
				m_bPressed = false;
				if( m_bFlipOnRelease )
					SetState( !m_bChecked );
				_requestRender();
			}
			break;

        default:
            break;

	}
}

//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgGizmoCheckbox::GetTextAreaWidth()
{
	WgSize gizmoSize = Size();

	WgSize	iconSize;

	if( m_pIconUnchecked )
		iconSize = m_pIconUnchecked->Size();
	else if( m_pIconChecked )
		iconSize = m_pIconChecked->Size();

	return _getTextRect( gizmoSize, _getIconRect( WgRect(0,0,gizmoSize), iconSize ) ).w;
}


//____ _onRender() ________________________________________________________

void WgGizmoCheckbox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Get correct mode

	WgMode	mode = WG_MODE_NORMAL;
	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;
	else if( m_bOver )
	{
		mode = WG_MODE_MARKED;
		if( m_bPressed )
			mode = WG_MODE_SELECTED;
	}

	// Get blocksets for background and icon

	WgBlockSetPtr	pBgBlockSet, pIconBlockSet;

	if( m_bChecked )
	{
		pBgBlockSet = m_pBlockChecked;
		pIconBlockSet = m_pIconChecked;
	}
	else
	{
		pBgBlockSet = m_pBlockUnchecked;
		pIconBlockSet = m_pIconUnchecked;
	}

	// Get block for background

	WgBlock			bgBlock;

	if( pBgBlockSet )
		bgBlock = pBgBlockSet->GetBlock(mode,_canvas);

	// Blit background

	pDevice->ClipBlitBlock( _clip, bgBlock, _canvas );

	// Get the content rect and icon rect

	WgRect contentRect	= bgBlock.ContentRect(_canvas);
	WgRect iconRect		= _getIconRect( contentRect, pIconBlockSet );

	// Get block for icon

	WgBlock		iconBlock;

	if( pIconBlockSet )
		iconBlock = pIconBlockSet->GetBlock(mode,iconRect);

	// Blit icon

	if( iconRect.w > 0 && iconRect.h > 0 )
		pDevice->ClipBlitBlock( _clip, iconBlock, iconRect );

	// Print text

 	if( m_text.nbLines()!= 0 )
	{
		WgRect	textRect = _getTextRect( contentRect, iconRect );
		m_pText->setMode( mode );

		if( pBgBlockSet )
			m_text.SetBgBlockColors( pBgBlockSet->TextColors() );


		pDevice->PrintText( _clip, m_pText, textRect );
	}
}

//____ _onRefresh() _______________________________________________________

void WgGizmoCheckbox::_onRefresh( void )
{
	if( m_pBlockChecked && m_pBlockUnchecked && m_pBlockChecked->IsOpaque() &&
		m_pBlockUnchecked->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
}

//____ _onNewSize() _______________________________________________________

void WgGizmoCheckbox::_onNewSize( const WgSize& size )
{
	m_text.setLineWidth( GetTextAreaWidth() );
}


//____ _onCloneContent() _______________________________________________________

void WgGizmoCheckbox::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoCheckbox * pOrg = (WgGizmoCheckbox *) _pOrg;

	m_bChecked			= pOrg->m_bChecked;
	m_bOver				= false;
	m_bPressed			= false;

	m_pBlockUnchecked	= pOrg->m_pBlockUnchecked;
	m_pBlockChecked		= pOrg->m_pBlockChecked;

	m_pIconUnchecked	= pOrg->m_pIconUnchecked;
	m_pIconChecked		= pOrg->m_pIconChecked;

	m_text				= pOrg->m_text;
	m_clickArea			= pOrg->m_clickArea;

	Wg_Interface_TextHolder::_onCloneContent( pOrg );
	WgIconHolder::_onCloneContent( pOrg );
}

//____ _textModified() _________________________________________________________

void WgGizmoCheckbox::_textModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}

//____ _iconModified() ________________________________________________________

void WgGizmoCheckbox::_iconModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}



//____ _markTestTextArea() ______________________________________________________

bool WgGizmoCheckbox::_markTestTextArea( int _x, int _y )
{
	WgBlockSetPtr	pIconBlockSet = m_bChecked?m_pIconChecked:m_pIconUnchecked;

	WgRect	contentRect = _getTextRect( Size(), _getIconRect( Size(), pIconBlockSet ) );

	if( m_text.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgGizmoCheckbox::_onAlphaTest( const WgCoord& ofs )
{
	WgMode mode = WG_MODE_NORMAL;

	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;
	else if( m_bOver )
	{
		mode = WG_MODE_MARKED;
		if( m_bPressed )
			mode = WG_MODE_SELECTED;
	}

	// Get blocksets for background and icon

	WgBlockSetPtr	pBgBlockSet, pIconBlockSet;

	if( m_bChecked )
	{
		pBgBlockSet = m_pBlockChecked;
		pIconBlockSet = m_pIconChecked;
	}
	else
	{
		pBgBlockSet = m_pBlockUnchecked;
		pIconBlockSet = m_pIconUnchecked;
	}


	WgBlock bgBlock;
	WgBlock iconBlock;

	WgSize	bgSize		= Size();
	WgRect	iconRect	= _getIconRect( bgSize, pIconBlockSet );

	if( m_bChecked )
	{
		if( m_pBlockChecked )
			bgBlock = m_pBlockChecked->GetBlock(mode,bgSize);

		if( m_pIconChecked )
			iconBlock = m_pIconChecked->GetBlock(mode,iconRect);
	}
	else
	{
		if( m_pBlockUnchecked )
			bgBlock = m_pBlockUnchecked->GetBlock(mode,bgSize);

		if( m_pIconUnchecked )
			iconBlock = m_pIconUnchecked->GetBlock(mode,iconRect);
	}


	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	textRect = _getTextRect( bgSize, iconRect);

			if( iconRect.x + iconRect.w < textRect.x )
				iconRect.w = textRect.x - iconRect.x;

			if( iconRect.x > textRect.Right() )
			{
				iconRect.w += iconRect.x - textRect.Right();
				iconRect.x = textRect.Right();
			}

			if( iconRect.y + iconRect.h < textRect.y )
				iconRect.h = textRect.y - iconRect.y;

			if( iconRect.y > textRect.Bottom() )
			{
				iconRect.h += iconRect.y - textRect.Bottom();
				iconRect.y = textRect.Bottom();
			}

			//

			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize) )) ||
				_markTestTextArea( ofs.x, ofs.y ) ||
				iconRect.Contains( ofs.x, ofs.y ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize) )) ||
				(iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Gizmo is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect ) )
				return true;

			return false;
		}
		case TEXT:			// Only the text is clickable.
		{
			if( _markTestTextArea( ofs.x, ofs.y ) )
				return true;

			return false;
		}

		default:
			assert(false);			// Garbage in m_clickArea
			return false;
	};
}
