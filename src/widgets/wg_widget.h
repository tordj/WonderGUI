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

#ifndef WG_WIDGET_DOT_H
#define WG_WIDGET_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_string.h>
#include <wg_skin.h>
#include <wg_receiver.h>
#include <wg_componentholder.h>
#include <wg_widgetholder.h>

namespace wg
{
	class GfxDevice;
	class Container;
	class Panel;
	class List;
	class Capsule;
	class Layer;
	class MsgRouter;
	class Patches;
	class WidgetHolder;

	class Widget;
	typedef	StrongPtr<Widget>	Widget_p;
	typedef	WeakPtr<Widget>		Widget_wp;

	class Container;
	typedef	StrongPtr<Container>	Container_p;
	typedef	WeakPtr<Container>	Container_wp;

	class Msg;
	typedef	StrongPtr<Msg>			Msg_p;
	typedef	WeakPtr<Msg>			Msg_wp;


	/**
	 * @brief Base class for widgets.
	 *
	 * Widget is the base class for all widgets, providing common functionality.
	 */

	class Widget : public Receiver, protected ComponentHolder
	{
		friend class MsgRouter;
		friend class InputHandler;

		friend class RootPanel;
		friend class FlexPanel;
		friend class ModalLayer;
		friend class ScrollPanel;
		friend class List;
		friend class PackList;
		friend class Capsule;
		friend class Layer;
		friend class StackPanel;
		friend class Container;
		friend class Panel;
		friend class PackPanel;
		friend class IStackPanelChildren;
		friend class ShaderCapsule;
		friend class PopupLayer;
		friend class ViewSlot;
		friend class LambdaPanel;
		friend class SplitPanel;
		friend class DragNDropLayer;
		friend class ShadowLayer;

		friend class Component;
		friend class Slot;

		template<class S, class H> friend class IChildrenSubclass;

	public:

		//.____ Identification _________________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static Widget_p		cast( Object * pObject );

		inline void			setId(int id);
		inline int			id() const;

		virtual bool		isContainer() const;

		//.____ Geometry _________________________________________________

		inline Coord		pos() const;
		inline Size			size() const;
		inline Rect			geo() const;
		inline Coord		globalPos() const;
		inline Rect			globalGeo() const;

		inline Coord		toGlobal( const Coord& coord ) const;
		inline Coord		toLocal( const Coord& coord ) const;

		inline QPix			matchingHeight( QPix width ) const;
		inline QPix			matchingWidth( QPix height ) const;

		inline Size			preferredSize() const;
		inline Size			minSize() const;
		inline Size			maxSize() const;


		//.____ Hierarchy _________________________________________________

		inline Widget_p		nextSibling() const;
		inline Widget_p		prevSibling() const;
		Container_p			parent() const;

		void				releaseFromParent();

		//.____ State _________________________________________________

		inline State		state() const;

		void				setEnabled(bool bEnabled);
		inline bool			isEnabled() const;

		void				setSelectable(bool bSelectable);
		inline bool			isSelectable() const;

		inline bool			grabFocus();
		inline bool			releaseFocus();
		inline bool			isFocused();


		//.____ Appearance _________________________________________________

		virtual void		setSkin( Skin * pSkin );
		Skin_p				skin( ) const	{ return m_pSkin; }

		inline void			setTooltip( const String& str ) { m_tooltip = str; }
		virtual String		tooltip() const { return m_tooltip; }

		void				setPointerStyle( PointerStyle style )	{ m_pointerStyle = style; }
		virtual PointerStyle	pointerStyle() const;


		//.____ Behavior _______________________________________________________

		void				setMarkOpacity( int opacity ) { m_markOpacity = opacity; }
		int					markOpacity() const { return m_markOpacity; }

		void				setTabLock( bool bLock ) { m_bTabLock = bLock; }
		bool				isTabLocked() const { return m_bTabLock; }

		void                setPickable( bool bPickable, int category = 0 );
		bool                isPickable() const { return m_bPickable; }
		int                 pickCategory() const { return m_pickCategory; }

		void                setDropTarget( bool bDropTarget );
		bool                isDropTarget() const { return m_bDropTarget; }


		//.____ Misc _________________________________________________________________

		inline bool			markTest( const Coord& ofs );
		void 				receive( Msg * pMsg ) override;

		inline void			refresh() { _refresh(); }

		inline Widget_p		clone() const { return _clone(); }
		Widget_p			newOfMyType() const { return _newOfMyType(); } 					///< @brief Create and return a new widget of the same type.

		inline bool			isOpaque() const { return m_bOpaque; }

	protected:
		Widget();
		virtual ~Widget();

		inline void			_setHolder( WidgetHolder * pHolder, Slot * pSlot ) { m_pHolder = pHolder; m_pSlot = pSlot; }
		WidgetHolder *		_holder() const { return m_pHolder; }
		Slot *				_slot() const { return m_pSlot; }

		virtual BlendMode	_getBlendMode() const;

		virtual Widget* 	_newOfMyType() const = 0;

		virtual bool	_markTest(const CoordI& ofs);


		// Methods for geometry in quarterpixels

		inline CoordI	_globalPos() const { return m_pHolder ? m_pHolder->_childGlobalPos(m_pSlot) : CoordI(); }
		inline RectI	_globalGeo() const { return  m_pHolder ? RectI(m_pHolder->_childGlobalPos(m_pSlot), m_size) : RectI(0, 0, m_size); }

		CoordI			_toGlobal(const CoordI& coord) const;
		CoordI			_toLocal(const CoordI& coord) const;

		virtual int		_matchingHeight(int width) const;
		virtual int		_matchingWidth(int height) const;

		virtual SizeI	_preferredSize() const;
		virtual SizeI	_minSize() const;
		virtual SizeI	_maxSize() const;


		// Convenient calls to holder



		inline void		_requestRender() { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot ); }
		inline void		_requestRender( const RectI& rect ) { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot, rect ); }
		inline void		_requestResize() { if( m_pHolder ) m_pHolder->_childRequestResize( m_pSlot ); }
		inline void		_requestInView() const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot ); }
		inline void		_requestInView( const RectI& mustHaveArea, const RectI& niceToHaveArea ) const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot, mustHaveArea, niceToHaveArea ); }


		inline Widget *	_nextSibling() const { if( m_pHolder ) return m_pHolder->_nextChild( m_pSlot ); else return nullptr; }
		inline Widget *	_prevSibling() const { if( m_pHolder ) return m_pHolder->_prevChild( m_pSlot ); else return nullptr; }
		inline Container *	_parent() const { if( m_pHolder ) return m_pHolder->_childParent(); else return nullptr; }

		inline RectI	_windowSection() const { if( m_pHolder ) return m_pHolder->_childWindowSection( m_pSlot ); return RectI(); }

		// To be overloaded by Widget

		virtual void	_collectPatches( Patches& container, const RectI& geo, const RectI& clip );
		virtual void	_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode );

		Widget *		_clone() const;
		virtual void	_cloneContent( const Widget * _pOrg );
		virtual void	_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window );

		virtual void	_refresh();
		virtual void	_setSize( const SizeI& size );
		virtual void	_setSkin( Skin * pSkin );
		virtual void	_setState( State state );

		virtual void	_receive( Msg * pMsg );
		virtual	bool	_alphaTest( const CoordI& ofs );

		virtual SizeI	_windowPadding() const;	// Padding of window before we get to (scrollable) content.

		// Methods for components to access

		virtual Object * _object() override;
		virtual const Object * _object() const override;

		virtual CoordI	_componentPos( const Component * pComponent ) const override;
		virtual SizeI	_componentSize( const Component * pComponent ) const override;
		virtual RectI	_componentGeo( const Component * pComponent ) const override;
		virtual CoordI	_globalComponentPos( const Component * pComponent ) const override;
		virtual RectI	_globalComponentGeo( const Component * pComponent ) const override;

		virtual void	_componentRequestRender( const Component * pComponent ) override;
		virtual void	_componentRequestRender( const Component * pComponent, const RectI& rect ) override;
		virtual void	_componentRequestResize( const Component * pComponent ) override;

 		virtual void	_componentRequestFocus( const Component * pComponent ) override;
		virtual void	_componentRequestInView( const Component * pComponent ) override;
		virtual void	_componentRequestInView( const Component * pComponent, const RectI& mustHave, const RectI& niceToHave ) override;

		virtual void	_receiveComponentNotif( Component * pComponent, ComponentNotif notification, int value, void * pData ) override;

		//

		int				m_id;

		WidgetHolder *	m_pHolder;
		Slot *			m_pSlot;

		Skin_p			m_pSkin;
		PointerStyle	m_pointerStyle;

		String			m_tooltip;
		int				m_markOpacity;

		bool			m_bOpaque;
		bool			m_bTabLock;		// If set, the widget prevents focus shifting away from it with tab.
		bool			m_bSelectable;	// Set if widget is allowed to be selected.

		State			m_state;
		SizeI			m_size;

		bool            m_bPickable;        // Set if this widget accepts to be the source of drag-n-drop operations.
		uint8_t         m_pickCategory;     // Category of drag-n-drop operations. User defined.

		bool            m_bDropTarget;      // Set if this widget accepts to be the target of drag-n-drop operations.

//	private:
		bool			m_bPressed;		// Keeps track of pressed button when mouse leaves/re-enters widget.

	};



	//____ Inline methods __________________________________________________________
	/**
	* @brief	Set a user specified ID for this widget.
	*
	* Sets a user specified ID, which can be read with id() and used to identify this
	* widget. It has no other purpose, is not used internally and does not need to be unique.
	*
	* Default ID is 0.
	*/

	void Widget::setId(int id)
	{
		m_id = id;
	}

	/**
	* @brief	Read the ID set through setId().
	*
	* Reads the user specified ID.
	*
	* @return	The ID earlier specified with setId() or 0.
	*/

	int Widget::id() const
	{
		return m_id;
	}



	/**
	 * @brief	Get the local position of widget.
	 *
	 * Get the local position of the widget, which is the position relative to parent.
	 *
	 * @return Local position in pixels.
	 */
	Coord Widget::pos() const
	{
		if( m_pHolder )
			return Util::rawToQpix(m_pHolder->_childPos( m_pSlot ));
		return Coord(0,0);
	}


	/**
	 * @brief	Get the width and height of widget.
	 *
	 * Get the size (width and height) of widget.
	 *
	 * @return Width and height in pixels.
	 */
	Size Widget::size() const
	{
		return Util::rawToQpix(m_size);
	}

	/**
	 * @brief	Get the local geometry of widget.
	 *
	 * Get the local geometry of the widget, eg a rectangle with the size of the widget and
	 * positon relative to the parent. If the widget doesn't have a parent, it's position
	 * defaults to (0,0).
	 *
	 * @return Local geometry in pixels.
	 */
	Rect Widget::geo() const
	{
		if( m_pHolder )
			return Rect(Util::rawToQpix(m_pHolder->_childPos( m_pSlot )),Util::rawToQpix(m_size));
		return Rect(0,0,m_size);
	}

	/**
	 * @brief	Get the global position of widget.
	 *
	 * Get the global position of widget, which is the position relative to RootPanel.
	 *
	 * @return Global position in pixels.
	 */
	Coord Widget::globalPos() const
	{
		return Util::rawToQpix(_globalPos());
	}

	/**
	 * @brief	Get the global geometry of widget.
	 *
	 * Get the global geometry of the widget, eg a rectangle with the size of the widget and
	 * positon relative to RootPanel. If the widget doesn't have a parent, it's position
	 * defaults to (0,0).
	 *
	 * @return Global geometry in pixels.
	 */
	Rect Widget::globalGeo() const
	{
		return Util::rawToQpix(_globalGeo());
	}

	//____ toGlobal() __________________________
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

	Coord Widget::toGlobal(const Coord& coord) const
	{
		return Util::rawToQpix(_toGlobal(Util::qpixToRaw(coord)) );
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

	Coord Widget::toLocal(const Coord& coord) const
	{
		return Util::rawToQpix(_toLocal(Util::qpixToRaw(coord)));
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

	QPix Widget::matchingHeight(QPix width) const
	{
		return QPix::fromRaw(_matchingHeight(width.raw));
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

	QPix Widget::matchingWidth(QPix height) const
	{
		return QPix::fromRaw(_matchingWidth(height.raw));
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
		return Util::rawToQpix(_preferredSize());
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
		return Util::rawToQpix(_minSize());
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
		return Util::rawToQpix(_maxSize());
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
		return _markTest(Util::qpixToRaw(ofs));
	}


	/**
	 * @brief	Return the state of widget.
	 *
	 * Returns a copy of the widgets internal state object, containing basic state information
	 * such as if it is disabled, has mouse pointer inside, is pressed or selected.
	 *
	 * This is just for information. WonderGUI doesn't allow the state to be manipulated except
	 * as allowed by specific API:s like setEnabled().
	 *
	 * @return State object representing the current state of the widget.
	 */

	State Widget::state() const
	{
		return m_state;
	}

	/**
	 * @brief	Get the enabled/disabled state of widget.
	 *
	 * @return 	True if widget is enabled, otherwise false.
	 */

	bool Widget::isEnabled() const
	{
		return m_state.isEnabled();
	}

	/**
	 * @brief	Get the selectable flag of widget.
	 *
	 * Get the selectable flag of widget, which determines if widget should be considered
	 * selectable when placed in lists and such.
	 *
	 * @return 	True if widget is selectable, otherwise false.
	 */


	bool Widget::isSelectable() const
	{
		return m_bSelectable;
	}



	/** @brief Get keyboard focus.
	*
	* Attempt to get keyboard focus. This can be blocked by parent or further up the hierarchy.
	* Widget needs to descend from a RootPanel in order to be able to get keyboard focus.
	*
	* @return True if widget has focus now, no matter if it already had focus or not before the call.
	*/

	bool Widget::grabFocus()
	{
		if (m_state.isFocused())
			return true;

		if( m_pHolder )
			return m_pHolder->_childRequestFocus( m_pSlot, this );
		return false;
	}

	/** @brief Release keyboard focus.
	*
	* Attempt to release keyboard focus, returning it to previous widget being focused.
	*
	* @return True if widget no longer has focus, no matter if it had focus or not before the call.
	*/

	bool Widget::releaseFocus()
	{
		if (!m_state.isFocused())
			return true;

		if( m_pHolder )
			return m_pHolder->_childReleaseFocus( m_pSlot, this );
		return false;
	}

	/** @brief Check if widget has keyboard focus.
	 *
	 * Check if widget has keyboard focus.
	 *
	 * @return True if widget has focus, otherwise false.
	 */

	bool Widget::isFocused()
	{
		return m_state.isFocused();
	}

	/** @brief Get next sibling.
	 *
	 * Get the widgets next sibling. Order of siblings is determined by parent
	 * and generally left to right or top to bottom for containers where children can't overlap.
	 * For containers where children can overlap the order is generally background to foreground.
	 *
	 * @return Pointer to next sibling or null if none.
	 */

	Widget_p Widget::nextSibling() const
	{
		if( m_pHolder )
			return m_pHolder->_nextChild( m_pSlot );
		return nullptr;
	}

	/** @brief Get previous sibling.
	 *
	 * Get the widgets previous sibling. Order of siblings is determined by parent
	 * and generally left to right or top to bottom for containers where children can't overlap.
	 * For containers where children can overlap the order is generally background to foreground.
	 *
	 * @return Pointer to previous sibling or null if none.
	 */

	Widget_p Widget::prevSibling() const
	{
		if( m_pHolder )
			return m_pHolder->_prevChild( m_pSlot );
		return 0;
	}



} // namespace wg
#endif
