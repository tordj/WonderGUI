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

#include <wg_checkbox.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_util.h>
#include <wg_rootpanel.h>
#include <wg_eventhandler.h>
#include <assert.h>

static const char	c_widgetType[] = {"Checkbox"};



//____ WgCheckBox() _________________________________________________________________

WgCheckBox::WgCheckBox()
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

WgCheckBox::~WgCheckBox()
{
}


//____ Type() _________________________________________________________________

const char * WgCheckBox::Type( void ) const
{
	return GetClass();
}

const char * WgCheckBox::GetClass( void )
{
	return c_widgetType;
}


//____ SetSource() ____________________________________________________________

bool WgCheckBox::SetSource( const WgBlocksetPtr& pUnchecked, const WgBlocksetPtr& pChecked )
{
	m_pBlockUnchecked	= pUnchecked;
	m_pBlockChecked		= pChecked;

	_onRefresh();
	return true;
}

//____ SetIcons() ______________________________________________________________

void WgCheckBox::SetIcons( const WgBlocksetPtr& pUnchecked, const WgBlocksetPtr& pChecked,
								const WgOrigo& origo, WgBorders borders, float scale,
								bool bPushText )
{
	m_pIconUnchecked	= pUnchecked;
	m_pIconChecked		= pChecked;
	m_iconOrigo	= origo;
	m_iconScale			= scale;
	m_bIconPushText		= bPushText;
	m_iconBorders		= borders;
	_onRefresh();
}

//____ SetIcons() ______________________________________________________________

void WgCheckBox::SetIcons( const WgBlocksetPtr& pUnchecked, const WgBlocksetPtr& pChecked )
{
	m_pIconUnchecked	= pUnchecked;
	m_pIconChecked		= pChecked;
	_onRefresh();
}

//____ SetCheckedIcon() _______________________________________________________

void WgCheckBox::SetCheckedIcon( const WgBlocksetPtr& pChecked )
{
	m_pIconChecked		= pChecked;
	_onRefresh();
}

//____ SetUncheckedIcon() _____________________________________________________

void WgCheckBox::SetUncheckedIcon( const WgBlocksetPtr& pUnchecked )
{
	m_pIconUnchecked	= pUnchecked;
	_onRefresh();
}



//____ SetState() _____________________________________________________________

bool WgCheckBox::SetState( bool _state )
{
	if( m_bChecked != _state )
	{
		m_bChecked = _state;

		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
		{
			if( _state )
				pHandler->QueueEvent( new WgEvent::CheckboxCheck( this ) );
			else
				pHandler->QueueEvent( new WgEvent::CheckboxUncheck( this ) );

			pHandler->QueueEvent( new WgEvent::CheckboxToggle(this, _state ) );
		}
		_requestRender();
	}

	return true;
}

//____ SetFlipOnRelease() _____________________________________________________

void WgCheckBox::SetFlipOnRelease( bool bFlipOnRelease )
{
	m_bFlipOnRelease = bFlipOnRelease;
}


//____ PreferredSize() __________________________________________________

WgSize WgCheckBox::PreferredSize() const
{
	WgSize iconPreferredSize;
	WgSize bgPreferredSize;
	WgSize textPreferredSize;

	if( m_text.nbChars() > 0 )
		textPreferredSize = m_text.unwrappedSize();

	if( m_pBlockUnchecked )
	{
		bgPreferredSize = m_pBlockUnchecked->Size();
		textPreferredSize += m_pBlockUnchecked->Padding();
	}

	if( m_pIconUnchecked )
	{
		iconPreferredSize = m_pIconUnchecked->Size() + m_iconBorders.Size();

		//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorders, iconScale and bgPreferredSize
	}

	WgSize preferredSize = WgSize::Max( WgSize::Max(iconPreferredSize,bgPreferredSize), textPreferredSize);

	return preferredSize;
}


//____ _onEnable() _________________________________________________
void WgCheckBox::_onEnable()
{
	_requestRender();
}

//____ _onDisable() _________________________________________________
void WgCheckBox::_onDisable()
{
	m_bOver = false;
	m_bPressed = false;

	_requestRender();
}

//____ _onEvent() _____________________________________________________________

void WgCheckBox::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
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
			if( button == 1 )
			{
				if( !m_bPressed )
				{
					if( !m_bFlipOnRelease )
						SetState( !m_bChecked );
					m_bPressed = true;
					_requestRender();
				}
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_CLICK:
		case WG_EVENT_MOUSEBUTTON_DOUBLE_CLICK:
		case WG_EVENT_MOUSEBUTTON_REPEAT:
		case WG_EVENT_MOUSEBUTTON_DRAG:
		{
			int button = static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button();
			if( button != 1 )
				pHandler->ForwardEvent( pEvent );
			break;
		}

		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
			{
				if( m_bPressed )
				{
					m_bPressed = false;
					if( m_bFlipOnRelease )
						SetState( !m_bChecked );
					_requestRender();
				}
			}
			else
				pHandler->ForwardEvent( pEvent );
			break;
		}

        default:
			pHandler->ForwardEvent( pEvent );
            break;
	}
}

//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgCheckBox::GetTextAreaWidth()
{
	WgSize widgetSize = Size();

	WgSize	iconSize;

	if( m_pIconUnchecked )
		iconSize = m_pIconUnchecked->Size();
	else if( m_pIconChecked )
		iconSize = m_pIconChecked->Size();

	return _getTextRect( widgetSize, _getIconRect( WgRect(0,0,widgetSize), iconSize ) ).w;
}


//____ _onRender() ________________________________________________________

void WgCheckBox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
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

	WgBlocksetPtr	pBgBlockset, pIconBlockset;

	if( m_bChecked )
	{
		pBgBlockset = m_pBlockChecked;
		pIconBlockset = m_pIconChecked;
	}
	else
	{
		pBgBlockset = m_pBlockUnchecked;
		pIconBlockset = m_pIconUnchecked;
	}

	// Get block for background

	WgBlock			bgBlock;

	if( pBgBlockset )
		bgBlock = pBgBlockset->GetBlock(mode,_canvas);

	// Blit background

	pDevice->ClipBlitBlock( _clip, bgBlock, _canvas );

	// Get the content rect and icon rect

	WgRect contentRect	= bgBlock.ContentRect(_canvas);
	WgRect iconRect		= _getIconRect( contentRect, pIconBlockset );

	// Get block for icon

	WgBlock		iconBlock;

	if( pIconBlockset )
		iconBlock = pIconBlockset->GetBlock(mode,iconRect);

	// Blit icon

	if( iconRect.w > 0 && iconRect.h > 0 )
		pDevice->ClipBlitBlock( _clip, iconBlock, iconRect );

	// Print text

 	if( m_text.nbLines()!= 0 )
	{
		WgRect	textRect = _getTextRect( contentRect, iconRect );
		m_pText->setMode( mode );

		if( pBgBlockset )
			m_text.SetBgBlockColors( pBgBlockset->TextColors() );


		pDevice->PrintText( _clip, m_pText, textRect );
	}
}

//____ _onRefresh() _______________________________________________________

void WgCheckBox::_onRefresh( void )
{
	if( m_pBlockChecked && m_pBlockUnchecked && m_pBlockChecked->IsOpaque() &&
		m_pBlockUnchecked->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	_requestRender();
}

//____ _onNewSize() _______________________________________________________

void WgCheckBox::_onNewSize( const WgSize& size )
{
	m_text.setLineWidth( GetTextAreaWidth() );
}


//____ _onCloneContent() _______________________________________________________

void WgCheckBox::_onCloneContent( const WgWidget * _pOrg )
{
	WgCheckBox * pOrg = (WgCheckBox *) _pOrg;

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

void WgCheckBox::_textModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}

//____ _iconModified() ________________________________________________________

void WgCheckBox::_iconModified()
{
	//TODO: Need to do more here, like possibly request resize.

	_requestRender();
}



//____ _markTestTextArea() ______________________________________________________

bool WgCheckBox::_markTestTextArea( int _x, int _y )
{
	WgBlocksetPtr	pIconBlockset = m_bChecked?m_pIconChecked:m_pIconUnchecked;

	WgRect	contentRect = _getTextRect( Size(), _getIconRect( Size(), pIconBlockset ) );

	if( m_text.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgCheckBox::_onAlphaTest( const WgCoord& ofs )
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

	WgBlocksetPtr	pBgBlockset, pIconBlockset;

	if( m_bChecked )
	{
		pBgBlockset = m_pBlockChecked;
		pIconBlockset = m_pIconChecked;
	}
	else
	{
		pBgBlockset = m_pBlockUnchecked;
		pIconBlockset = m_pIconUnchecked;
	}


	WgBlock bgBlock;
	WgBlock iconBlock;

	WgSize	bgSize		= Size();
	WgRect	iconRect	= _getIconRect( bgSize, pIconBlockset );

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

			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize), m_markOpacity )) ||
				_markTestTextArea( ofs.x, ofs.y ) ||
				iconRect.Contains( ofs.x, ofs.y ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize), m_markOpacity)) ||
				(iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect, m_markOpacity )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Widget is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect, m_markOpacity ) )
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
