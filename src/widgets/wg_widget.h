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
#include <wg_geocomponent.h>
#include <wg_slotholder.h>

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
	class SlotHolder;

	class Widget;
	typedef	StrongPtr<Widget>	Widget_p;
	typedef	WeakPtr<Widget>		Widget_wp;

	class Container;
	typedef	StrongPtr<Container>	Container_p;
	typedef	WeakPtr<Container>	Container_wp;

	class Msg;
	typedef	StrongPtr<Msg>			Msg_p;
	typedef	WeakPtr<Msg>			Msg_wp;

	class BasicPayload;
	typedef	StrongPtr<BasicPayload>	BasicPayload_p;
	typedef	WeakPtr<BasicPayload>	BasicPayload_wp;


	/**
	 * @brief Base class for widgets.
	 *
	 * Widget is the base class for all widgets, providing common functionality.
	 */

	class Widget : public Receiver, protected GeoComponent::Holder
	{
		friend class Container;
		friend class GeoComponent;
		friend class StaticSlot;
		friend class DynamicSlot;

		template<class S> friend class CStaticSlotVector;
		template<class S> friend class CDynamicSlotVector;
		template<class S, int X> friend class CSlotArray;

	public:

		//.____ Identification _________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

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

		inline MU			matchingHeight( MU width ) const;
		inline MU			matchingWidth( MU height ) const;

		inline Size			preferredSize() const;
		inline Size			minSize() const;
		inline Size			maxSize() const;


		//.____ Hierarchy _________________________________________________

		inline Widget_p		nextSibling() const;
		inline Widget_p		prevSibling() const;
		Container_p			parent() const;

		void				releaseFromParent();

		//.____ State _________________________________________________

		inline const State&		state() const;

		void				setEnabled(bool bEnabled);
		inline bool			isEnabled() const;

		void				setSelectable(bool bSelectable);
		inline bool			isSelectable() const;

		inline bool			grabFocus();
		inline bool			releaseFocus();
		inline bool			isFocused();


		//.____ Appearance _________________________________________________

		virtual void		setSkin( Skin * pSkin );
		inline Skin_p		skin() const;

		inline void			setTooltip(const String& str);
		inline virtual String tooltip() const;

		inline void			setPointerStyle(PointerStyle style);
		virtual PointerStyle pointerStyle() const;


		//.____ Behavior _______________________________________________________

		inline void			setMarkOpacity(int opacity);
		inline int			markOpacity() const;

		inline void			setTabLock(bool bLock);
		inline bool			isTabLocked() const;

		void                setPickable( bool bPickable, int category = 0 );
		inline bool         isPickable() const;
		inline int          pickCategory() const;

		void                setDropTarget( bool bDropTarget );
		inline bool         isDropTarget() const;


		//.____ Misc _________________________________________________________________

		inline bool			markTest( const Coord& ofs );
		void 				receive( Msg * pMsg ) override;

		inline void			refresh();

		inline Widget_p		clone() const;
		inline Widget_p		newOfMyType() const;

		inline bool			isOpaque() const;

		void				setPayload(BasicPayload * pPayload);
		bool				hasPayload() const;
		BasicPayload_p		payload() const;

	protected:
		Widget();
		virtual ~Widget();

		void				_setSlot(StaticSlot * pSlot);
		SlotHolder *		_holder() { return m_pHolder; }
		const SlotHolder *	_holder() const { return m_pHolder; }
		StaticSlot *		_slot() const { return m_pSlot; }

		virtual BlendMode	_getBlendMode() const;

		virtual Widget* 	_newOfMyType() const = 0;

		virtual bool		_markTest(const CoordI& ofs);

		bool            	_requestPreRenderCall();

		// Methods for geometry in quarterpixels

		inline CoordI		_globalPos() const { return m_pHolder ? m_pHolder->_childGlobalPos(m_pSlot) : CoordI(); }
		inline RectI		_globalGeo() const { return  m_pHolder ? RectI(m_pHolder->_childGlobalPos(m_pSlot), m_size) : RectI(0, 0, m_size); }

		CoordI				_toGlobal(const CoordI& coord) const;
		CoordI				_toLocal(const CoordI& coord) const;

		virtual int			_matchingHeight(int width) const;
		virtual int			_matchingWidth(int height) const;

		virtual SizeI		_preferredSize() const;
		virtual SizeI		_minSize() const;
		virtual SizeI		_maxSize() const;

		inline RectI		_contentRect() const { return m_pSkin ? m_pSkin->_contentRect(m_size, m_state) : RectI( m_size ); }
		inline RectI		_contentRect(const RectI& canvas) const { return m_pSkin ? m_pSkin->_contentRect(canvas, m_state) : canvas; }
		inline SizeI		_contentPaddingSize() const { return m_pSkin ? m_pSkin->_contentPaddingSize() : SizeI(); }

		// Convenient calls to holder



		inline void			_requestRender() { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot ); }
		inline void			_requestRender( const RectI& rect ) { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot, rect ); }
		inline void			_requestResize() { if( m_pHolder ) m_pHolder->_childRequestResize( m_pSlot ); }
		inline void			_requestInView() const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot ); }
		inline void			_requestInView( const RectI& mustHaveArea, const RectI& niceToHaveArea ) const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot, mustHaveArea, niceToHaveArea ); }

		inline Widget *		_nextSibling() const { if( m_pHolder ) return m_pHolder->_nextChild( m_pSlot ); else return nullptr; }
		inline Widget *		_prevSibling() const { if( m_pHolder ) return m_pHolder->_prevChild( m_pSlot ); else return nullptr; }
		inline Container *	_parent() const { if( m_pHolder ) return m_pHolder->_container(); else return nullptr; }

		inline RectI		_windowSection() const { if( m_pHolder ) return m_pHolder->_childWindowSection( m_pSlot ); return RectI(); }

		// To be overloaded by Widget

		virtual void		_collectPatches( Patches& container, const RectI& geo, const RectI& clip );
		virtual void		_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode );

		Widget *			_clone() const;
		virtual void		_cloneContent( const Widget * _pOrg );

		virtual void    	_preRender();
		virtual void		_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window );

		virtual void		_refresh();
		virtual void		_resize( const SizeI& size );
		virtual void		_setSkin( Skin * pSkin );
		virtual void		_setState( State state );

		virtual void		_receive( Msg * pMsg );
		virtual	bool		_alphaTest( const CoordI& ofs );

		virtual SizeI		_windowPadding() const;	// Padding of window before we get to (scrollable) content.

		// Methods for components to access

		virtual Object * 	_object() override;
		virtual const Object * _object() const override;

		virtual CoordI		_componentPos( const GeoComponent * pComponent ) const override;
		virtual SizeI		_componentSize( const GeoComponent * pComponent ) const override;
		virtual RectI		_componentGeo( const GeoComponent * pComponent ) const override;
		virtual CoordI		_globalComponentPos( const GeoComponent * pComponent ) const override;
		virtual RectI		_globalComponentGeo( const GeoComponent * pComponent ) const override;

		virtual void		_componentRequestRender( const GeoComponent * pComponent ) override;
		virtual void		_componentRequestRender( const GeoComponent * pComponent, const RectI& rect ) override;
		virtual void		_componentRequestResize( const GeoComponent * pComponent ) override;

 		virtual void		_componentRequestFocus( const GeoComponent * pComponent ) override;
		virtual void		_componentRequestInView( const GeoComponent * pComponent ) override;
		virtual void		_componentRequestInView( const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave ) override;

		virtual void		_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData ) override;

		//

		int				m_id;
		BasicPayload_p	m_pPayload;

		SlotHolder *	m_pHolder;
		StaticSlot *	m_pSlot;

		Skin_p			m_pSkin;
		PointerStyle	m_pointerStyle;

		String			m_tooltip;
		int				m_markOpacity;

		bool			m_bOpaque;			// Set if widget is totally opaque, no need to render anything behind.
		bool			m_bTabLock;			// If set, the widget prevents focus shifting away from it with tab.
		bool			m_bSelectable;		// Set if widget is allowed to be selected.

		State			m_state;			// Current state of widget.
		SizeI			m_size;				// Current size of widget.

		bool            m_bPickable;        // Set if this widget accepts to be the source of drag-n-drop operations.
		uint8_t         m_pickCategory;     // Category of drag-n-drop operations. User defined.

		bool            m_bDropTarget;      // Set if this widget accepts to be the target of drag-n-drop operations.

//	private:
		bool			m_bPressed;			// Keeps track of pressed button when mouse leaves/re-enters widget.

	};



	//____ Inline methods __________________________________________________________


	//____ setId() __________________________
	/**
	* @brief	Set a user specified ID for this widget.
	*
	* Sets a user specified ID, which can be read with id() and used to identify this
	* widget. It has no other purpose. It is not used internally and does not need to be unique.
	*
	* Default ID is 0.
	*/

	void Widget::setId(int id)
	{
		m_id = id;
	}

	//____ id() __________________________
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



	//____ pos() __________________________
	/**
	 * @brief	Get the local position of widget.
	 *
	 * Get the local position of the widget, which is the position 
	 * of its north-west corner relative to parents north-west corner.
	 *
	 * @return Widgets position relative to its parent.
	 */
	Coord Widget::pos() const
	{
		if( m_pHolder )
			return Util::qpixToMU(m_pHolder->_childPos( m_pSlot ));
		return Coord(0,0);
	}


	//____ size() __________________________
	/**
	 * @brief	Get the size of of widget.
	 *
	 * Get the size (width and height) of widget.
	 *
	 * @return Size of widget.
	 */
	Size Widget::size() const
	{
		return Util::qpixToMU(m_size);
	}

	//____ geo() __________________________
	/**
	 * @brief	Get the local geometry of widget.
	 *
	 * Get widgets geometry, e.g. position and size of widget relative to its parent. If widget doesn't have a parent, it's position
	 * defaults to (0,0).
	 *
	 * @return Position and size of widget relative to its parent.
	 */
	Rect Widget::geo() const
	{
		if( m_pHolder )
			return Rect(Util::qpixToMU(m_pHolder->_childPos( m_pSlot )),Util::qpixToMU(m_size));
		return Rect(0,0,m_size);
	}

	//____ globalPos() __________________________
	/**
	 * @brief	Get the global position of widget.
	 *
	 * Get position of widgets top-left corner in the global coordinate space.
	 *
	 * @return Position of widget in global coordinate space.
	 */
	Coord Widget::globalPos() const
	{
		return Util::qpixToMU(_globalPos());
	}

	//____ globalGeo() __________________________
	/**
	 * @brief	Get the global geometry of widget.
	 *
	 * Get position and size of widget in the global coordinate space.
	 * If the widget doesn't have a parent, it's position defaults to (0,0).
	 *
	 * @return Geometry of widget in the global coordinate space.
	 */
	Rect Widget::globalGeo() const
	{
		return Util::qpixToMU(_globalGeo());
	}

	//____ toGlobal() __________________________
	/**
	 * @brief Convert coordinate from local to global space
	 *
	 * Convert coordinate from widgets local coordinate space to the global coordinate space.
	 *
	 * @param coord		Coordinate in widgets local coordinate system.
	 *
	 * Please note that the widgets local coordinate space originates from the top-left
	 * corner of its geometry box and is NOT the same as the (parents) local coordinate
	 * space in which it lives.
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 *
	 * @return Coordinate in gobal coordinate space.
	 */

	Coord Widget::toGlobal(const Coord& coord) const
	{
		return Util::qpixToMU(_toGlobal(Util::MUToQpix(coord)) );
	}

	//____ toLocal() ____________________________________________________________
	/**
	 * @brief Convert coordinate from global to local space
	 *
	 * Convert coordinate from the global coordinate space to widgets local coordinate space.
	 *
	 * @param coord		Coordinate in widgets local coordinate system.
	 *
	 * Please note that the widgets local coordinate space originates from the top-left
	 * corner of its geometry box and is NOT the same as the (parents) local coordinate
	 * space in which it lives.
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 *
	 * @return Coordinate in local coordinate space.
	 */

	Coord Widget::toLocal(const Coord& coord) const
	{
		return Util::qpixToMU(_toLocal(Util::MUToQpix(coord)));
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

	MU Widget::matchingHeight(MU width) const
	{
		return MU::fromRaw(_matchingHeight(width.qpix));
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

	MU Widget::matchingWidth(MU height) const
	{
		return MU::fromRaw(_matchingWidth(height.qpix));
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
		return Util::qpixToMU(_preferredSize());
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
		return Util::qpixToMU(_minSize());
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

	Size Widget::maxSize() const
	{
		return Util::qpixToMU(_maxSize());
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
		return _markTest(Util::MUToQpix(ofs));
	}


	//____ state() ____________________________________________________________
	/**
	 * @brief	Return the state of widget.
	 *
	 * Returns a reference to the widgets state object, containing basic state information
	 * such as if it is disabled, has mouse pointer inside its geometry, is pressed or selected.
	 *
	 * This is just for information. WonderGUI doesn't allow the state to be manipulated except
	 * as allowed by specific API:s, like setEnabled().
	 *
	 * @return Reference to state object representing the current state of the widget.
	 */

	const State& Widget::state() const
	{
		return m_state;
	}

	//____ isEnabled() ____________________________________________________________
	/**
	 * @brief	Get the enabled/disabled state of widget.
	 *
	 * @return 	True if widget is enabled, otherwise false.
	 */

	bool Widget::isEnabled() const
	{
		return m_state.isEnabled();
	}

	//____ isSelectable() ____________________________________________________________
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



	//____ grabFocus() ____________________________________________________________
	/** 
	* @brief Get keyboard focus.
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

	//____ releaseFocus() ____________________________________________________________
	/** 
	* @brief Release keyboard focus.
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

	//____ isFocused() ____________________________________________________________
	/** 
	 * @brief Check if widget has keyboard focus.
	 *
	 * Check if widget has keyboard focus.
	 *
	 * @return True if widget has focus, otherwise false.
	 */

	bool Widget::isFocused()
	{
		return m_state.isFocused();
	}

	//____ nextSibling() ____________________________________________________________
	/** 
	 * @brief Get next sibling.
	 *
	 * Get the widgets next sibling. Order of siblings is determined by parent
	 * but is generally either left to right, top to bottom or background to foreground.
	 *
	 * @return Pointer to next sibling or null if none.
	 */

	Widget_p Widget::nextSibling() const
	{
		if( m_pHolder )
			return m_pHolder->_nextChild( m_pSlot );
		return nullptr;
	}

	//____ prevSibling() ____________________________________________________________
	/** 
	 * @brief Get previous sibling.
	 *
	 * Get the widgets previous sibling. Order of siblings is determined by parent
	 * but is generally either left to right, top to bottom or background to foreground.
	 *
	 * @return Pointer to previous sibling or null if none.
	 */

	Widget_p Widget::prevSibling() const
	{
		if( m_pHolder )
			return m_pHolder->_prevChild( m_pSlot );
		return 0;
	}

	//____ skin() _____________________________________________________________
	/**
	 * @brief Get the skin used by widget.
	 *
	 * Get the skin used by this widget. If a widget has several skins, this is the background skin,
	 * covering the area of the widget and padding its content.
	 *
	 * @return Pointer to the skin used by this widget.
	 */

	Skin_p Widget::skin() const 
	{ 
		return m_pSkin; 
	}

	//____ setTooltip() _______________________________________________________
	/**
	 * @brief Set widgets tooltip string.
	 *
	 * Sets a string that is used as the tooltip for the widget. A tooltip with this
	 * text will be displayed by the TooltipLayer (if one is present in the hierarchy) when 
	 * the mouse hovers over the widget.
	 *
	 * Note: A widget may override the tooltip set by this method and display another 
	 * text, depending on the situation and exact position of the pointer.
	 * 
	 * @return void
	 */

	void Widget::setTooltip(const String& str) 
	{ 
		m_tooltip = str; 
	}

	//____ tooltip() __________________________________________________________
	/**
	 * @brief Get widgets tooltip string.
	 *
	 * Gets the string to be displayed in a tooltip when the mouse hovers over the widget.
	 *
	 * Note: This string might be different from what has been set using setTooltip().
	 * Some widgets can decide to display something else depending on the situation 
	 * and exact position of the pointer.
	 *
	 * @return void
	 */

	String Widget::tooltip() const 
	{ 
		return m_tooltip; 
	}

	//____ setPointerStyle() __________________________________________________

	void Widget::setPointerStyle(PointerStyle style) 
	{ 
		m_pointerStyle = style; 
	}

	//____ setMarkOpacity() ___________________________________________________

	void Widget::setMarkOpacity(int opacity) 
	{ 
		m_markOpacity = opacity; 
	}

	//____ markOpacity() ______________________________________________________

	int Widget::markOpacity() const 
	{ 
		return m_markOpacity; 
	}

	//____ setTabLock() _______________________________________________________

	void Widget::setTabLock(bool bLock) 
	{ 
		m_bTabLock = bLock; 
	}

	//____ isTabLocked() ______________________________________________________

	bool Widget::isTabLocked() const 
	{ 
		return m_bTabLock; 
	}

	//____ isPickable() _______________________________________________________

	bool Widget::isPickable() const 
	{ 
		return m_bPickable; 
	}

	//____ pickCategory() _____________________________________________________

	int Widget::pickCategory() const 
	{ 
		return m_pickCategory; 
	}

	//____ isDropTarget() ____________________________________________________

	bool Widget::isDropTarget() const 
	{ 
		return m_bDropTarget; 
	}

	//____ refresh() __________________________________________________________

	void Widget::refresh() 
	{ 
		_refresh(); 
	}

	//____ clone() ____________________________________________________________

	Widget_p Widget::clone() const 
	{ 
		return _clone(); 
	}

	//____ newOfMyType() ______________________________________________________
	/**
	 * @brief Create and return a new widget of the same type.
	 */

	Widget_p Widget::newOfMyType() const 
	{ 
		return _newOfMyType(); 
	}

	//____ isOpaque() _________________________________________________________

	bool Widget::isOpaque() const 
	{ 
		return m_bOpaque; 
	}

} // namespace wg
#endif
