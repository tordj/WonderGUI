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

#include <wg_togglebutton.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_rootpanel.h>
#include <wg_msgrouter.h>
#include <wg_togglegroup.h>
#include <wg_internal.h>

#include <assert.h>

namespace wg
{

	const TypeInfo ToggleButton::TYPEINFO = { "ToggleButton", &Widget::TYPEINFO };



	//____ ToggleButton() _________________________________________________________________

	ToggleButton::ToggleButton() : text(this), icon(this)
	{
		m_bPressed			= false;
		m_bReturnPressed	= false;
		m_bFlipOnRelease	= false;

		m_clickArea			= DEFAULT;
	}

	//____ Destructor _____________________________________________________________

	ToggleButton::~ToggleButton()
	{
		if( m_pToggleGroup )
			m_pToggleGroup->_remove(this);
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& ToggleButton::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSelected() __________________________________________________________

	bool ToggleButton::setSelected( bool bSelected )
	{
		if( m_state.isSelected() != bSelected )
		{
			State state = m_state;
			state.setSelected(bSelected);
			_setState(state);
		}
		return true;
	}

	//____ setFlipOnRelease() _____________________________________________________

	void ToggleButton::setFlipOnRelease( bool bFlipOnRelease )
	{
		m_bFlipOnRelease = bFlipOnRelease;
	}

	//____ preferredSize() __________________________________________________

	Size ToggleButton::preferredSize() const
	{
		Size iconPreferredSize;
		Size textPreferredSize;

		if( !OO(text).isEmpty() )
			textPreferredSize = OO(text)._preferredSize();

		if( !_icon().isEmpty() )
		{
			iconPreferredSize = _icon().skin()->preferredSize() + _icon().padding().size();

			//TODO: Add magic for how icon influences textPreferredSize based on origo, iconBorder, iconScale and bgPreferredSize
		}

		// Apply the skin

		Size preferredSize = Size::max( iconPreferredSize, textPreferredSize );

		preferredSize = m_skin.sizeForContent( preferredSize );

		return preferredSize;
	}

	//____ _setToggleGroup() ________________________________________________________

	void ToggleButton::_setToggleGroup( ToggleGroup * pGroup )
	{
		if( m_pToggleGroup && pGroup )			// Remove us from current Toggle group if we are not just set to null (then we are being removed by Togglegroup itself).
			m_pToggleGroup->_remove(this);

		m_pToggleGroup = pGroup;
	}


	//____ _receive() _____________________________________________________________

	void ToggleButton::_receive( Msg * _pMsg )
	{
		State state = m_state;

		switch( _pMsg->type() )
		{
			case MsgType::KeyPress:
				if( static_cast<KeyMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = true;
					_pMsg->swallow();
				}
				break;

			case MsgType::KeyRepeat:
				if( static_cast<KeyMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
					_pMsg->swallow();
				break;

			case MsgType::KeyRelease:
				if( static_cast<KeyMsg*>(_pMsg)->translatedKeyCode() == Key::Return )
				{
					m_bReturnPressed = false;
					_pMsg->swallow();
				}
				break;

			case MsgType::MouseEnter:
				state.setHovered(true);
				break;
			case MsgType::MouseLeave:
				state.setHovered(false);
				break;
			case MsgType::MousePress:
				if( static_cast<MousePressMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = true;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseRelease:
				if( static_cast<MouseReleaseMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					m_bPressed = false;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseClick:
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
			case MsgType::MouseDrag:
				if( static_cast<MouseButtonMsg*>(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;

			case MsgType::FocusGained:
				state.setFocused(true);
				break;
			case MsgType::FocusLost:
				state.setFocused(false);
				m_bReturnPressed = false;
				m_bPressed = false;
				break;
			default:
				break;
		}

		// Set pressed if return or mouse button 1 is pressed

		if( m_bReturnPressed || (m_bPressed && state.isHovered()) )
			state.setPressed(true);
		else
			state.setPressed(false);

		// Possibly flip selected

		if( state.isPressed() != m_state.isPressed() )
		{
			if( state.isPressed() != m_bFlipOnRelease )
				state.setSelected( !state.isSelected() );
		}

		//

		if( state != m_state )
			_setState(state);
	}

	//____ _setState() ______________________________________________________

	void ToggleButton::_setState( State state )
	{
		// If state has changed from selected to unselected we need to check with Togglegroup

		if( !state.isSelected() && m_state.isSelected() && m_pToggleGroup && !m_pToggleGroup->_unselect(this) )
			state.setSelected(true);

		//

		State oldState = state;
		Widget::_setState(state);

		OO(text)._setState( state );

		if (!_icon().isEmpty())
		{
			//TODO: Remove once icon uses SkinSlot.
			_requestRender();
		}
		if( state.isSelected() != oldState.isSelected() )
		{
			Base::msgRouter()->post( ToggleMsg::create(this, state.isSelected() ) );

			if( m_pToggleGroup && state.isSelected() )
				m_pToggleGroup->_select(this);
		}
	}

	//____ setSkin() _______________________________________________________

	void ToggleButton::setSkin( Skin * pSkin )
	{
		//TODO: Set canvas size for the components

		Widget::setSkin(pSkin);
	}


	//____ _render() ________________________________________________________

	void ToggleButton::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		// Get the content rect and icon rect

		Rect contentRect	= m_skin.contentRect(_canvas, m_state );

		Rect iconRect		= _icon()._getIconRect( contentRect );

		// Blit icon

		if( _icon().isEmpty() && iconRect.w > 0 && iconRect.h > 0 )
			_icon().skin()->render( pDevice, iconRect, m_state );

		// Print text

	 	if( !OO(text).isEmpty() )
		{
			Rect	textRect = _icon()._getTextRect( contentRect, iconRect );
			OO(text)._render( pDevice, textRect );
		}
	}

	//____ _refresh() _______________________________________________________

	void ToggleButton::_refresh( void )
	{
		Widget::_refresh();

		//TODO: Handling of icon and text?
	}

	//____ _resize() _______________________________________________________

	void ToggleButton::_resize( const Size& size )
	{
		Widget::_resize( size );

		Rect contentRect	= m_skin.contentRect(size, m_state );

		OO(text)._setSize( _icon()._getTextRect( contentRect, _icon()._getIconRect( contentRect )) );
	}


	//____ _cloneContent() _______________________________________________________

	void ToggleButton::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		ToggleButton * pOrg = (ToggleButton *) _pOrg;

		m_bFlipOnRelease	= pOrg->m_bFlipOnRelease;
		m_clickArea			= pOrg->m_clickArea;

//		_icon().onCloneContent( &pOrg->m_icon );
	}

	//____ _markTestTextArea() ______________________________________________________

	bool ToggleButton::_markTestTextArea( const Coord& pos )
	{
		Rect contentRect = m_skin.contentRect(m_size, m_state );

		contentRect = _icon()._getTextRect( contentRect, _icon()._getIconRect( contentRect ) );

		if( OO(text)._charAtPos( pos - contentRect.pos() ) != -1 )
			return true;

		return false;
	}

	//____ _alphaTest() ______________________________________________________

	bool ToggleButton::_alphaTest( const Coord& ofs )
	{
		Size	bgSize		= m_size;

		Rect	contentRect = m_skin.contentRect( bgSize, m_state );

		Rect	iconRect	= _icon()._getIconRect( contentRect );

		switch( m_clickArea )
		{
			case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
			{
				// Extend iconRect so it connects with textArea before we compare

				Rect	textRect = _icon()._getTextRect( contentRect, iconRect);

				if( iconRect.x + iconRect.w < textRect.x )
					iconRect.w = textRect.x - iconRect.x;

				if( iconRect.x > textRect.right() )
				{
					iconRect.w += iconRect.x - textRect.right();
					iconRect.x = textRect.right();
				}

				if( iconRect.y + iconRect.h < textRect.y ) 
					iconRect.h = textRect.y - iconRect.y;

				if( iconRect.y > textRect.bottom() )
				{
					iconRect.h += iconRect.y - textRect.bottom();
					iconRect.y = textRect.bottom();
				}

				//

				if( Widget::_alphaTest( ofs ) || _markTestTextArea( ofs ) || iconRect.contains( ofs ) )
					return true;

				return false;
			}
			case ALPHA:			// Alpha test on background and icon.
			{
				if( Widget::_alphaTest( ofs ) ||
					( !_icon().isEmpty() && _icon().skin()->markTest( ofs, iconRect, m_state, m_markOpacity )) )
					return true;

				return false;
			}
			case GEO:			// Full geometry of Widget is clickable.
				return true;
			case ICON:			// Only the icon (alpha test) is clickable.
			{
				if( !_icon().isEmpty() && _icon().skin()->markTest( ofs, iconRect, m_state, m_markOpacity ) )
					return true;

				return false;
			}
			case TEXT:			// Only the text is clickable.
			{
				if( _markTestTextArea( ofs ) )
					return true;

				return false;
			}

			default:
				assert(false);			// Garbage in m_clickArea
				return false;
		};
	}

} // namespace wg
