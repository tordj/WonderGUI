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
#include <wg_dataset.h>

namespace wg
{

	const TypeInfo Widget::TYPEINFO = { "Widget", &Receiver::TYPEINFO };

	//____ constructor ____________________________________________________________

	Widget::Widget():m_id(0), m_pHolder(0), m_pSlot(0), m_pointerStyle(PointerStyle::Default),
						m_markOpacity( 1 ), m_bOpaque(false), m_bTabLock(false),
						 m_bPressed(false), m_bSelectable(true), m_size(256,256),
						m_bPickable(false), m_bDropTarget(false), m_pSkin(this), m_pickCategory(0)
	{
	}

	//____ Destructor _____________________________________________________________

	Widget::~Widget()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Widget::typeInfo(void) const
	{
		return TYPEINFO;
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
	/**
	 * @brief Releases widget from its parent.
	 *
	 * Removes the widget from its parent.
	 *
	 * @return Nothing.
	*/

	void Widget::releaseFromParent()
	{
		if (m_pHolder)
			m_pHolder->_releaseChild(m_pSlot);
	}


	//____ commonAncestor() ___________________________________________________
	/**
		@brief Finds closest common ancestor of widgets.

		Finds the closest common ancestor between this and pOtherWidget (if any).
		The result can be one of the specified widgets if one is an ancestor
		of the other.

		@param pOtherWidget	The widget this is checked against.

		@return The closest common ancestor, which can be one of the widgets specifed,
				or nullptr if they are not related.
	*/
	Widget_p Widget::commonAncestor(Widget* pOtherWidget)
	{
		if (pOtherWidget == nullptr)
			return nullptr;

		const int c_maxLevels = 128;

		Widget* list1[c_maxLevels];
		Widget* list2[c_maxLevels];

		list1[0] = this;
		list2[0] = pOtherWidget;

		int nb1 = _listAncestors(list1+1, c_maxLevels-1) + 1;
		int nb2 = pOtherWidget->_listAncestors(list2+1, c_maxLevels-1) + 1;

		if (nb1 == c_maxLevels-1 || nb2 == c_maxLevels-1)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::Internal, "Widget hierarchy too deep for operation.", this, Widget::TYPEINFO, __func__, __FILE__, __LINE__);
			return nullptr;
		}

		int maxSearchDepth = min(nb1, nb2);

		int i = 0;
		while (i < maxSearchDepth && list1[nb1-1 - i] == list2[nb2-1 - i])
			i++;

		if (i == 0 )
			return nullptr;

		return list1[nb1 - i];
	}

	//____ pointerStyle() ________________________________________
	/**
	 * @brief Returns mouse pointer style.
	 *
	 * Returns the style the mouse pointer should have when hovering over this widget. This is typically
	 * the style previously set with setPointerStyle(), but some widgets might override and change this 
	 * dynamically depending on the exact position of the pointer and state of the widget. I.e. a widget might
	 * switch to PointerStyle::Ibeam when hovering over some editable text.
	 *
	 * @return PointerStyle for mouse pointer given its current position/status within the widget. If pointer 
	 * is outside widget, the style previously set with setPointerStyle() is returned.
	*/

	PointerStyle Widget::pointerStyle() const
	{
		return m_pointerStyle;
	}

	//____ setDropTarget() ____________________________________________________
	/**
	 * @brief Set widget to become a destination for drag-n-drop operations.
	 *
	 * Setting a widget as drop target makes it a potential destination for drag-n-drop operations.
	 * 
	 * Any widget that is a drop target receives a DropProbe-message when the mouse enters it 
	 * during a drag-n-drop operation. By accepting the DropProbe-message, the widget acknowledges 
	 * that it (probably) can accept the dragged content if dropped upon it and will start receiving other
	 * drop-related messages (DropEnter, DropMove, DropLeave and DropDeliver).
	 *
	 * @return Nothing.
	*/

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

	//____ setBaggage() _______________________________________________________

	void Widget::setBaggage(Object * pBaggage)
	{
		m_pBaggage = pBaggage;
	}

	//____ hasBaggage() _______________________________________________________

	bool Widget::hasBaggage() const
	{
		return m_pBaggage;
	}

	//____ baggage() __________________________________________________________

	Object_p Widget::baggage() const
	{
		return m_pBaggage;
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

	bool Widget::markTest(const Coord& ofs)
	{
		if (m_markOpacity >= 256 || ofs.x < 0 || ofs.y < 0 || ofs.x >= m_size.w || ofs.y >= m_size.h)
			return false;

		if (m_markOpacity <= 0)
			return true;

		return _alphaTest(ofs);
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
	 * @return The preferred height for the given width.
	 */

	MU Widget::matchingHeight( MU width ) const
	{
		return preferredSize().h;		// Default is to stick with preferred height no matter what width.
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
	 * @return The preferred width for the given height.
	 */
	 
	MU Widget::matchingWidth( MU height ) const
	{
		return preferredSize().w;		// Default is to stick with preferred width no matter what height.
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
	 * A container holding a widget will strive to provide the widget its preferred size, given
	 * the constraints and limitations the container needs to work with. If a container can't
	 * provide a widget its preferred size, it is likely to decide the closest width or height
	 * that it can provide and then make a second call to either matchingWidth() or matchingHeight()
	 * after which it will decide the size of the child.
	 *
	 * @return The preferred size of the widget.
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
	 * @return The minimum size of the widget.
	 */

	Size Widget::minSize() const
	{
		if( m_pSkin )
			return m_pSkin->minSize();
		else
			return Size(0,0);
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
	 * @return The maximum size of the widget.
	 */

	//____ maxSize() ______________________________________________________________

	Size Widget::maxSize() const
	{
		return Size(1 << 24, 1 << 24);
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

	void Widget::_setSlot(StaticSlot * pSlot)
	{
		m_pHolder = pSlot ? pSlot->_holder() : nullptr;
		m_pSlot = pSlot;
	}

	//____ _listAncestors() ________________________________________________________

	int Widget::_listAncestors(Widget* array[], int max)
	{
		int n = 0;
		Widget* p = _parent();
		while (p != nullptr && n < max)
		{
			array[n++] = p;
			p = p->_parent();
		}
		return n;
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

	void Widget::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
			container.add( Rect( geo, clip ) );
	}

	//____ _maskPatches() _______________________________________________________

	void Widget::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( Rect( geo, clip ) );
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

	void Widget::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		if( m_pSkin )
			m_pSkin->render( pDevice, _canvas, m_state );
	}

	//____ _resize() ___________________________________________________________

	void Widget::_resize( const Size& size )
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
		bool bRequestResize = ( !m_pSkin || !pSkin || m_pSkin->contentPaddingSize() != pSkin->contentPaddingSize() ||
								m_pSkin->preferredSize() != pSkin->preferredSize() || m_pSkin->minSize() != pSkin->minSize() );

		if( pSkin && pSkin->isOpaque(m_state) )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		m_pSkin = pSkin;
		_requestRender();

		if( bRequestResize )
			_requestResize();
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
				if (!pMsg->hasDataset())
				{
					pMsg->setDataset(BasicDataset::create());
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

	bool Widget::_alphaTest( const Coord& ofs )
	{
		if( m_pSkin )
			return m_pSkin->markTest( ofs, Rect(m_size), m_state, m_markOpacity );

		return false;
	}

	//____ _windowPadding() _______________________________________________________

	Size Widget::_windowPadding() const
	{
		return Size();
	}

	//____ _componentRequestRender() _________________________________________________________

	void Widget::_componentRequestRender( const GeoComponent * pComponent )
	{
		_requestRender( _componentGeo( pComponent ) );
	}

	void Widget::_componentRequestRender( const GeoComponent * pComponent, const Rect& rect )
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
		Rect r = _componentGeo( pComponent );
		_requestInView( r, r );
	}

	void Widget::_componentRequestInView( const GeoComponent * pComponent, const Rect& mustHave, const Rect& niceToHave )
	{
		Coord ofs = _componentPos( pComponent );
		_requestInView(mustHave + ofs, niceToHave + ofs );
	}


	//____ _componentPos() ______________________________________________________________

	Coord Widget::_componentPos( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_pSkin->contentOfs( m_state );
		else
			return Coord();
	}

	//____ _componentSize() ______________________________________________________________

	Size Widget::_componentSize( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_size - m_pSkin->contentPaddingSize();
		else
			return m_size;
	}

	//____ _componentGeo() ______________________________________________________________

	Rect Widget::_componentGeo( const GeoComponent * pComponent ) const
	{
		if( m_pSkin )
			return m_pSkin->contentRect( m_size, m_state );
		else
			return Rect( 0,0,m_size );
	}

	//____ _globalComponentPos() ________________________________________________________

	Coord Widget::_globalComponentPos( const GeoComponent * pComponent ) const
	{
		return _componentPos( pComponent ) + globalPos();
	}

	//____ _globalComponentGeo() ______________________________________________________________

	Rect Widget::_globalComponentGeo( const GeoComponent * pComponent ) const
	{
		return _componentGeo( pComponent ) + globalPos();
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

	//____ _skinRequestRender() _______________________________________________

	void Widget::_skinRequestRender(SkinSlot* pSlot)
	{
		_requestRender();
	}

	void Widget::_skinRequestRender(SkinSlot* pSlot, const Rect& rect)
	{
		_requestRender(rect);
	}


} // namespace wg
