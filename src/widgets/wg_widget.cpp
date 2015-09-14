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

#include <wg_widget.h>
#include <wg_types.h>

#	include <wg_rootpanel.h>
#	include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char Widget::CLASSNAME[] = {"Widget"};
	
	
	//____ Constructor ____________________________________________________________
	
	Widget::Widget():m_id(0), m_pHook(0), m_pointerStyle(WG_POINTER_DEFAULT),
						m_markOpacity( 1 ), m_bOpaque(false),
						m_bTabLock(false), m_bPressed(false)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	Widget::~Widget()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Widget::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Receiver::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Widget::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Widget_p Widget::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Widget_p( static_cast<Widget*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ parent() _______________________________________________________________
	
	Container_p Widget::parent() const
	{ 
		if( m_pHook ) 
			return m_pHook->parent(); 
		return 0; 
	}
	
	//____ pointerStyle() ________________________________________
	
	WgPointerStyle Widget::pointerStyle() const
	{
		return m_pointerStyle;
	}
	
	//____ setEnabled() _______________________________________________________________
	
	void Widget::setEnabled( bool bEnabled )
	{
		if( m_state.isEnabled() != bEnabled || isContainer() )
		{
			State old = m_state;
			m_state.setEnabled(bEnabled);
			_onStateChanged(old);
		}
	}
	
	//____ markTest() _____________________________________________________________
	/**
	 * @brief Check if specified coordinate is inside or outside of widget.
	 *
	 * Check if specified coordinate is inside or outside of widget.
	 * 
	 * @param ofs	Coordinate to check in widgets own coordinate system.
	 * 
	 * This method first checks if the specified coordinate is inside the widgets
	 * box geometry. If it is, a second check is performed against the widgets
	 * alpha value (transparency) at the specified coordinate.
	 * If the alpha value is equal to or higher than the widgets MarkOpacity value, the
	 * test succeeds and MarkTest returns true.
	 * 
	 * MarkOpacity is by default set to 1, which means that all but totally transparent pixels
	 * will be marked. See setMarkOpacity() for more info. 
	 * 
	 * This method is mainly used to determine if the pointer hovers over the widget or not. 
	 * 
	 * @return True if alpha value of coordinate is equal to or higher than widgets MarkOpaciy.
	 */
	
	bool Widget::markTest( const Coord& ofs )
	{
		if( m_markOpacity <= 0 || ofs.x < 0 || ofs.y < 0 )
			return false;
			
		Size sz = size();
	
		if( ofs.x >= sz.w || ofs.y >= sz.h )
			return false;
			
		if( m_markOpacity >= 256 )
			return true;
			
		return _onAlphaTest(ofs,sz);
	}
	
	//____ setSkin() ______________________________________________________________
	
	void Widget::setSkin( const Skin_p& pSkin )
	{
		Skin_p pOldSkin = m_pSkin;
		m_pSkin = pSkin;
		_onSkinChanged( pOldSkin, m_pSkin );
	}
	
	
	
	//____ cloneContent() _________________________________________________________
	
	bool Widget::cloneContent( const Widget_p& _pOrg )
	{
		if( _pOrg->className() != className() )
			return false;
	
		Widget * pOrg = _pOrg.rawPtr();
	
		m_id			= pOrg->m_id;
	
		m_pointerStyle 	= pOrg->m_pointerStyle;
		m_pSkin			= pOrg->m_pSkin;
	
		m_tooltip		= pOrg->m_tooltip;
		m_markOpacity	= pOrg->m_markOpacity;
	
		m_bOpaque		= pOrg->m_bOpaque;
		m_bTabLock		= pOrg->m_bTabLock;
	
		// We do not clone state...
	
		_onCloneContent( pOrg );
		return true;
	}
	
	//____ _onNewHook() ___________________________________________________________
	
	void Widget::_onNewHook( Hook * pHook )
	{
		m_pHook = pHook;
	}
	
	//____ _onNewRoot() ___________________________________________________________
	
	void Widget::_onNewRoot( RootPanel * pRoot )
	{
	}
	
	//____ toGlobal() ____________________________________________________________
	/**
	 * @brief Convert coordinate from local to global coordinate system
	 *
	 * Convert coordinate from local to global coordinate system
	 *
	 * @param coord		Coordinate in widgets local coordinate system.
	 * 
	 * Please note that the widgets local coordinate system originates from the top-left
	 * corner of its box geometry and is NOT the same as the (parents) local coordinate 
	 * system in which it lives. 
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 * 
	 * @return Coordinate in gobal coordinate system
	 */
	 
	 Coord Widget::toGlobal( const Coord& coord ) const
	{
		Coord c = globalPos();
		c.x += coord.x;
		c.y += coord.y;
		return c;
	}
	
	//____ toLocal() ____________________________________________________________
	/**
	 * @brief Convert coordinate from local to global coordinate system
	 *
	 * Convert coordinate from local to global coordinate system
	 *
	 * @param coord		Coordinate in widgets local coordinate system.
	 * 
	 * Please note that the widgets local coordinate system originates from the top-left
	 * corner of its box geometry and is NOT the same as the (parents) local coordinate 
	 * system in which it lives. 
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 * 
	 * @return Coordinate in gobal coordinate system
	 */
	
	Coord Widget::toLocal( const Coord& coord ) const
	{
		Coord c = globalPos();
		return Coord( coord.x - c.x, coord.y - c.y );
	}
	
	//____ matchingHeight() _______________________________________________________
	/**
	 * @brief Get the widgets preferred height for the specified width.
	 *
	 * Get the widgets preferred height for the specified width.
	 *
	 * @param width		Width in pixels.
	 * 
	 * This method is used by containers to get the preferred height of a widget for which
	 * it has already decided the width.
	 * 
	 * @return The preferred height for the given width in pixels.
	 */
	
	int Widget::matchingHeight( int width ) const
	{
		return preferredSize().h;		// Default is to stick with best height no matter what width.
	}
	
	//____ matchingWidth() _______________________________________________________
	/**
	 * @brief Get the widgets preferred width for the specified height.
	 *
	 * Get the widgets preferred width for the specified height.
	 * 
	 * @param height	Height in pixels.
	 * 
	 * This method is used by containers to get the preferred width of a widget for which
	 * it has already decided the height.
	 * 
	 * @return The preferred width for the given height in pixels.
	 */
	
	int Widget::matchingWidth( int height ) const
	{
		return preferredSize().w;		// Default is to stick with best width no matter what height.
	}
	
	//____ preferredSize() ________________________________________________________
	/**
	 * @brief Get the widgets preferred size.
	 *
	 * Get the widgets preferred size.
	 *
	 * Each widget has its own preferred size, which is depending on things such as
	 * skinning, content and (in the case of containers) size and layout of children.
	 * 
	 * A container holding a widget will strive to give the widget its preferred size, given
	 * the constraints and limitations the container needs to work with. If a container can't
	 * give a widget its preferred size, it is likely to decide the closest width or height
	 * that it can provide and then make a second call to either matchingWidth() or matchingHeight()
	 * after which it will decide the size of the child and notify it.
	 * 
	 * @return The preferred size of the widget in pixels.
	 */
	
	Size Widget::preferredSize() const
	{
		if( m_pSkin )
			return m_pSkin->preferredSize();
		else
			return Size(0,0);
	}
	
	//____ minSize() ______________________________________________________________
	/**
	 * @brief Get the widgets recommended minimum size.
	 *
	 * Get the widgets recommended minimum size.
	 *
	 * Each widget has its own minimum size, which is depending on things such as
	 * skinning, content and (in the case of containers) size and layout of children.
	 * 
	 * The minimum size is only a hint for the container, which should strive to not
	 * make a child smaller than its minimum size, but is allowed to set the child to
	 * any size it decides, including 0.0.
	 *  
	 * @return The minimum size of the widget in pixels.
	 */
	
	Size Widget::minSize() const
	{
		if( m_pSkin )
			return m_pSkin->minSize();
		else
			return Size(0,0);
	}
	
	//____ onMsg() _______________________________________________________________
	
	void Widget::onMsg( const Msg_p& pMsg )
	{
		// SetRepost before _onMsg() so that subclasses can swallow the respost.
		
		switch( pMsg->type() )
		{
			case WG_MSG_MOUSE_MOVE:
			case WG_MSG_MOUSE_PRESS:
			case WG_MSG_MOUSE_REPEAT:
			case WG_MSG_MOUSE_DRAG:
			case WG_MSG_MOUSE_RELEASE:
			case WG_MSG_MOUSE_CLICK:
			case WG_MSG_MOUSE_DOUBLE_CLICK:
			case WG_MSG_KEY_PRESS:
			case WG_MSG_KEY_REPEAT:
			case WG_MSG_KEY_RELEASE:
			case WG_MSG_WHEEL_ROLL:
			{
				Widget_p pParent = parent();
				if( pParent )
					pMsg->setRepost(pParent,pParent);
				break;
			}
			default:
				break;
		}
		_onMsg( pMsg );
	}
	
	//____ maxSize() ______________________________________________________________
	/**
	 * @brief Get the widgets recommended maximum size.
	 *
	 * Get the widgets recommended maximum size.
	 *
	 * Each widget has its own maximum size, which is depending on things such as
	 * skinning, content and (in the case of containers) size and layout of children.
	 * 
	 * The maximum size is only a hint for the container, which should strive to not
	 * make a child larger than its maximum size, but is allowed to set the child to
	 * any reasonable size it decides.
	 *  
	 * @return The maximum size of the widget in pixels.
	 */
	
	Size Widget::maxSize() const
	{
		return Size(2<<24,2<<24);
	}
	
	//____ _getBlendMode() _________________________________________________________
	
	WgBlendMode Widget::_getBlendMode() const
	{
		Container * pParent = _parent();
		if( pParent )
			return pParent->_getBlendMode();
		else
			return WG_BLENDMODE_BLEND;		// We always start out with WG_BLENDMODE_BLEND.
	}
	
	//____ _renderPatches() ________________________________________________________
	
	void Widget::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches )
	{
		for( const Rect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
		{
			Rect clip( _window, *pRect );
			if( clip.w > 0 && clip.h > 0 )
				_onRender( pDevice, _canvas, _window, clip );
		}
	}
	
	//____ onCollectPatches()  ____________________________________________________
	
	void Widget::_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
			container.add( Rect( geo, clip ) );
	}
	
	//____ _onMaskPatches() _______________________________________________________
	
	void Widget::_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
		{
			patches.sub( Rect( geo, clip ) );
		}
	}
	
	//____ _onRender() ____________________________________________________________
	
	void Widget::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		if( m_pSkin )
			m_pSkin->render( pDevice, _canvas, m_state, _clip );
	}
	
	//____ _onNewSize() ___________________________________________________________
	
	void Widget::_onNewSize( const Size& size )
	{
		_requestRender();
	}
	
	//____ _onRefresh() ___________________________________________________________
	
	void Widget::_onRefresh()
	{
		if( m_pSkin && m_pSkin->isOpaque(m_state) )
			m_bOpaque = true;
		else
			m_bOpaque = false;
	
		_requestResize();
		_requestRender();
	}
	
	//____ _onSkinChanged() _______________________________________________________
	
	void Widget::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		if( !pOldSkin || !pNewSkin || pOldSkin->contentPadding() != pNewSkin->contentPadding() ||
			pOldSkin->preferredSize() != pNewSkin->preferredSize() ||
			pOldSkin->minSize() != pNewSkin->minSize() )
		{
			_requestResize();
		}
	
		if( pNewSkin && pNewSkin->isOpaque(m_state) )
			m_bOpaque = true;
		else
			m_bOpaque = false;
	
		_requestRender();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void Widget::_onStateChanged( State oldState )
	{
		if( m_pSkin && !m_pSkin->isStateIdentical(m_state, oldState) )
		{
			m_bOpaque = m_pSkin->isOpaque(m_state);
			_requestRender();
		}
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void Widget::_onMsg( const Msg_p& _pMsg )
	{
		State oldState = m_state;
	
		switch( _pMsg->type() )
		{
			case WG_MSG_MOUSE_ENTER:
				if( m_bPressed )
					m_state.setPressed(true);
				else
					m_state.setHovered(true);
				break;
			case WG_MSG_MOUSE_LEAVE:
				m_state.setHovered(false);			// Also clears any pressed flag.
				break;
			case WG_MSG_MOUSE_PRESS:
			{
				MousePressMsg_p pMsg = MousePressMsg::cast(_pMsg);
				if( pMsg->button() == WG_BUTTON_LEFT )
				{
					if( m_state.isHovered() )
						m_state.setPressed(true);
	
					m_bPressed = true;
				}
				break;
			}
			case WG_MSG_MOUSE_RELEASE:
			{
				MouseReleaseMsg_p pMsg = MouseReleaseMsg::cast(_pMsg);
				if( pMsg->button() == WG_BUTTON_LEFT )
				{
					if( m_state.isHovered() )
						m_state.setPressed(false);
	
					m_bPressed = false;
				}
				break;
			}
			case WG_MSG_FOCUS_GAINED:
				m_state.setFocused(true);
				break;
			case WG_MSG_FOCUS_LOST:
				m_state.setFocused(false);
				break;
		}
	
		if( m_state != oldState )
			_onStateChanged( oldState );
	}
	
	//____ _onAlphaTest() _________________________________________________________
	
	bool Widget::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		if( m_pSkin )
			return m_pSkin->markTest( ofs, Rect(0,0,sz), m_state, m_markOpacity );
	
		return false;
	}
	
	//____ _windowPadding() _______________________________________________________
	
	Size Widget::_windowPadding() const 
	{
		return Size(0,0);
	}

} // namespace wg
