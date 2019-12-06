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
#include <wg_container.h>
#include <wg_rootpanel.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_payload.h>

namespace wg
{

	const char Widget::CLASSNAME[] = {"Widget"};


	//____ Constructor ____________________________________________________________

	Widget::Widget():m_id(0), m_pHolder(0), m_pSlot(0), m_pointerStyle(PointerStyle::Default),
						m_markOpacity( 1 ), m_bOpaque(false), m_bTabLock(false),
						 m_bPressed(false), m_bSelectable(true), m_size(256,256),
						m_bPickable(false), m_bDropTarget(false)
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

	Widget_p Widget::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Widget_p( static_cast<Widget*>(pObject) );

		return 0;
	}


	//____ isContainer() ______________________________________________________
	/**
	 * @brief Check if this widget is a container.
	 *
	 *  Check if widget is a container.
	 *
	 *  This method is a quicker way to check if the widget
	 *  is a container than calling isInstanceOf(Container::CLASS).
	 *
	 *  @return True if the widget is a subclass of Container.
	 */

	bool Widget::isContainer() const
	{
		return false;
	}


	//____ parent() _______________________________________________________________
	/**
	 * @brief Returns the parent of the widget.
	 *
	 * Returns the parent of the widget.
	 *
	 * @return Parent of the widget or null if widget has no parent.
	*/

	Container_p Widget::parent() const
	{
		if( m_pHolder )
			return m_pHolder->_container();
		return 0;
	}


	//____ releaseFromParent() ________________________________________________

	void Widget::releaseFromParent()
	{
		if (m_pHolder)
			m_pHolder->_releaseChild(m_pSlot);
	}


	//____ pointerStyle() ________________________________________

	PointerStyle Widget::pointerStyle() const
	{
		return m_pointerStyle;
	}

	//____ setDropTarget() ____________________________________________________

	void Widget::setDropTarget(bool bDropTarget)
	{
		m_bDropTarget = bDropTarget;
	}


	//____ setEnabled() _______________________________________________________________
	/**
	 * @brief Enables or disables the widget.
	 *
	 * Enables or disables the widget and all child widgets if the widget is a container.
	 *
	 * @param bEnabled	True to enable Widget, false to disable.Coordinate to check in widgets own coordinate system.
	 *
	 * A Widget that is disabled does not react on mouse or keyboard events or updates its content.
	 * A disabled Widget is rendered in disabled state, how that looks depends on the skin.
	 *
	 * This method never fails.
	 */

	void Widget::setEnabled( bool bEnabled )
	{
		if( m_state.isEnabled() != bEnabled || isContainer() )
		{
			State s = m_state;
			s.setEnabled(bEnabled);
			_setState(s);
		}
	}

	//____ setSelectable() ____________________________________________________
	/**
	 * @brief Ḿake widget (un)selectable.
	 *
	 * Sets the selectable flag of the widget. Default is true, except for some
	 * special widgets, like PopupOpener, which should perform an action instead
	 * of being selected when clicked.
	 *
	 * Parent widgets such as List that allows selection/unselection of its
	 * children should take the selectable flag into account and ignore selection
	 * of widgets that are not selectable.
	 *
	 * A typical use of this flag is to make widgets that should just be separators
	 * in lists and menus unselectable.
	 *
	 */

	void Widget::setSelectable(bool bSelectable)
	{
		m_bSelectable = bSelectable;
	}

	bool Widget::_markTest( const CoordI& ofs )
	{
		if( m_markOpacity >= 256  || ofs.x < 0 || ofs.y < 0 || ofs.x >= m_size.w || ofs.y >= m_size.h)
			return false;

		if( m_markOpacity <= 0 )
			return true;

		return _alphaTest(ofs);
	}

	//____ setSkin() ______________________________________________________________
	/**
	 * @brief Sets the skin of this widget.
	 *
	 * @param pSkin 	Pointer to the skin to be used.
	 *
	 * Specifies the skin to be used by this widget. The skin will cover the
	 * whole widget and provide the background for any components placed
	 * on the widget (if any).
	 *
	 * A skin typically has different looks depending on the widgets state (normal,
	 * disabled, mouse inside, pressed, selected etc) and can also include padding.
	 *
	 * Some widgets have more than one skin, but this is always the background skin for
	 * the whole widget.
	 *
	 */

	void Widget::setSkin( Skin * pSkin )
	{
		_setSkin( pSkin );
	}


	//____ clone() _________________________________________________________

	Widget * Widget::_clone() const
	{
		Widget * p = _newOfMyType();
		p->_cloneContent( this );
		return p;
	}

	//____ _cloneContent() _____________________________________________________

	void Widget::_cloneContent( const Widget * pOrg )
	{
		m_id			= pOrg->m_id;

		m_pointerStyle 	= pOrg->m_pointerStyle;
		m_pSkin			= pOrg->m_pSkin;

		m_tooltip		= pOrg->m_tooltip;
		m_markOpacity	= pOrg->m_markOpacity;

		m_bOpaque		= pOrg->m_bOpaque;
		m_bTabLock		= pOrg->m_bTabLock;

		m_size			= pOrg->m_size;

		//TODO: We should clone stateflags selectively, like disabled.

	}


	//____ _toGlobal() __________________________

	 CoordI Widget::_toGlobal( const CoordI& coord ) const
	{
		 return _globalPos() + coord;
	}

	//____ _toLocal() ____________________________________________________________

	CoordI Widget::_toLocal( const CoordI& coord ) const
	{
		return coord - _globalPos();
	}

	//____ _matchingHeight() _______________________________________________________

	int Widget::_matchingHeight( int width ) const
	{
		return _preferredSize().h;		// Default is to stick with preferred height no matter what width.
	}

	//____ _matchingWidth() _______________________________________________________

	int Widget::_matchingWidth( int height ) const
	{
		return _preferredSize().w;		// Default is to stick with preferred width no matter what height.
	}

	//____ _preferredSize() ________________________________________________________

	SizeI Widget::_preferredSize() const
	{
		if( m_pSkin )
			return m_pSkin->_preferredSize();
		else
			return SizeI(0,0);
	}

	//____ _minSize() ______________________________________________________________

	SizeI Widget::_minSize() const
	{
		if( m_pSkin )
			return m_pSkin->_minSize();
		else
			return SizeI(0,0);
	}

	//____ _maxSize() ______________________________________________________________

	SizeI Widget::_maxSize() const
	{
		return SizeI(1 << 28, 1 << 28);
	}

	//____ receive() _______________________________________________________________

	void Widget::receive( Msg * pMsg )
	{
		// SetRepost before _receive() so that subclasses can swallow the respost.

		switch( pMsg->type() )
		{
			case MsgType::MouseMove:
			case MsgType::MousePress:
			case MsgType::MouseRepeat:
			case MsgType::MouseDrag:
			case MsgType::MouseRelease:
			case MsgType::MouseClick:
			case MsgType::MouseDoubleClick:
			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			case MsgType::KeyRelease:
			case MsgType::WheelRoll:
			{
				Widget_p pParent = parent();
				if( pParent )
					pMsg->setRepost(pParent,pParent);
				break;
			}
			default:
				break;
		}
		_receive( pMsg );
	}

	//____ setPickable() ____________________________________________________________

	void Widget::setPickable( bool bPickable, int category )
	{
		m_bPickable = bPickable;
		m_pickCategory = category;
	}

	//____ _setSlot() ______________________________________________________________

	void Widget::_setSlot(BasicSlot * pSlot) 
	{ 
		m_pHolder = pSlot ? pSlot->_holder() : nullptr; 
		m_pSlot = pSlot;
	}

	//____ _getBlendMode() _________________________________________________________

	BlendMode Widget::_getBlendMode() const
	{
		Container * pParent = _parent();
		if( pParent )
			return pParent->_getBlendMode();
		else
			return BlendMode::Blend;		// We always start out with BlendMode::Blend.
	}

	//____ _collectPatches()  ____________________________________________________

	void Widget::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
			container.add( RectI( geo, clip ) );
	}

	//____ _maskPatches() _______________________________________________________

	void Widget::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( RectI( geo, clip ) );
		}
	}

    //____ _requestPreRenderCall() ___________________________________________________
    
    bool Widget::_requestPreRenderCall()
    {
        if( !_holder() )
            return false;
        
        RootPanel * pRoot = _holder()->_root();
        if( !pRoot )
            return false;
        
        pRoot->_addPreRenderCall(this);
        return true;
    }

    
    //____ _preRender() ____________________________________________________________
    
    void Widget::_preRender()
    {
        // By default we do nothing.
    }

    
	//____ _render() ____________________________________________________________

	void Widget::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		if( m_pSkin )
			m_pSkin->_render( pDevice, _canvas, m_state );
	}

	//____ _resize() ___________________________________________________________

	void Widget::_resize( const SizeI& size )
	{
		m_size = size;
//		_requestRender();		Do NOT request render here, it is the responsibility of ancestor initiating the series of events.
	}

	//____ _refresh() ___________________________________________________________

	void Widget::_refresh()
	{
		if( m_pSkin && m_pSkin->isOpaque(m_state) )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		_requestResize();
		_requestRender();
	}

	//____ _setSkin() _______________________________________________________

	void Widget::_setSkin( Skin * pSkin )
	{
		if( !m_pSkin || !pSkin || m_pSkin->_contentPadding() != pSkin->_contentPadding() ||
			m_pSkin->_preferredSize() != pSkin->_preferredSize() ||
			m_pSkin->_minSize() != pSkin->_minSize() )
		{
			_requestResize();
		}


		if( pSkin && pSkin->isOpaque(m_state) )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		m_pSkin = pSkin;
		_requestRender();
	}

	//____ _setState() _________________________________________________________

	void Widget::_setState( State state )
	{
		if( m_pSkin && !m_pSkin->isStateIdentical(state, m_state) )
		{
			m_bOpaque = m_pSkin->isOpaque(state);
			_requestRender();
		}

		m_state = state;
	}

	//____ _receive() _____________________________________________________________

	void Widget::_receive( Msg * _pMsg )
	{
		State state = m_state;

		switch( _pMsg->type() )
		{
			case MsgType::MouseEnter:
				if( m_bPressed )
					state.setPressed(true);
				else
					state.setHovered(true);
				break;
			case MsgType::MouseLeave:
				state.setHovered(false);			// Also clears any pressed flag.
				break;
			case MsgType::MousePress:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);
				if( pMsg->button() == MouseButton::Left )
				{
					if( state.isHovered() )
						state.setPressed(true);

					m_bPressed = true;
				}
				break;
			}
			case MsgType::MouseRelease:
			{
				auto pMsg = static_cast<MouseReleaseMsg*>(_pMsg);
				if( pMsg->button() == MouseButton::Left )
				{
					if( state.isHovered() )
						state.setPressed(false);

					m_bPressed = false;
				}
				break;
			}
			case MsgType::FocusGained:
				state.setFocused(true);
				break;
			case MsgType::FocusLost:
				state.setFocused(false);
				break;
			case MsgType::DropPick:
			{
				auto pMsg = static_cast<DropPickMsg*>(_pMsg);
				if (!pMsg->hasPayload())
				{
					pMsg->setPayload(Payload::create());
				}
				break;
			}
			case MsgType::DropEnter:
				state.setTargeted(true);
				break;
			case MsgType::DropLeave:
				state.setTargeted(false);
				break;
			default:
				break;
		}

		if( state != m_state )
			_setState( state );
	}

	//____ _alphaTest() _________________________________________________________

	bool Widget::_alphaTest( const CoordI& ofs )
	{
		if( m_pSkin )
			return m_pSkin->_markTest( ofs, RectI(0,0,m_size), m_state, m_markOpacity );

		return false;
	}

	//____ _windowPadding() _______________________________________________________

	SizeI Widget::_windowPadding() const
	{
		return SizeI(0,0);
	}

	//____ _componentRequestRender() _________________________________________________________

	void Widget::_componentRequestRender( const GeoComponent * pComponent )
	{
		_requestRender( _componentGeo( pComponent ) );
	}

	void Widget::_componentRequestRender( const GeoComponent * pComponent, const RectI& rect )
	{
		_requestRender( rect + _componentPos( pComponent ) );
	}

	//____ _componentRequestResize() ________________________________________________________

	void Widget::_componentRequestResize( const GeoComponent * pComponent )
	{
		_requestResize();
		_requestRender();
	}

	//____ _componentRequestFocus() ___________________________________________________

	void Widget::_componentRequestFocus( const GeoComponent * pComponent )
	{
		grabFocus();
	}

	//____ _componentRequestInView() ______________________________________________

	void Widget::_componentRequestInView( const GeoComponent * pComponent )
	{
		RectI r = _componentGeo( pComponent );
		_requestInView( r, r );
	}

	void Widget::_componentRequestInView( const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave )
	{
		CoordI ofs = _componentPos( pComponent );
		_requestInView(mustHave + ofs, niceToHave + ofs );
	}


	//____ _componentPos() ______________________________________________________________

	CoordI Widget::_componentPos( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_pSkin->_contentOfs( m_state );
		else
			return CoordI();
	}

	//____ _componentSize() ______________________________________________________________

	SizeI Widget::_componentSize( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_size - m_pSkin->_contentPadding();
		else
			return m_size;
	}

	//____ _componentGeo() ______________________________________________________________

	RectI Widget::_componentGeo( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_pSkin->_contentRect( m_size, m_state );
		else
			return RectI( 0,0,m_size );
	}

	//____ _globalComponentPos() ________________________________________________________

	CoordI Widget::_globalComponentPos( const GeoComponent * pComponent ) const
	{
		return _componentPos( pComponent ) + _globalPos();
	}

	//____ _globalComponentGeo() ______________________________________________________________

	RectI Widget::_globalComponentGeo( const GeoComponent * pComponent ) const
	{
		return _componentGeo( pComponent ) + _globalPos();
	}

	//____ _object() ______________________________________________________

	Object * Widget::_object()
	{
		return this;
	}

	const Object * Widget::_object() const
	{
		return this;
	}


	//____ _receiveComponentNotif() __________________________________________________

	void Widget::_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData )
	{
		// By default we do nothing
	}



} // namespace wg
