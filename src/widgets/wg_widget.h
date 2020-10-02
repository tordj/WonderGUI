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
#include <wg_receiver.h>
#include <wg_geocomponent.h>
#include <wg_slotholder.h>
#include <wg_skinslot.h>

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


	/**
	 * @brief Base class for widgets.
	 *
	 * Widget is the base class for all widgets, providing common functionality.
	 */

	class Widget : public Receiver, protected GeoComponent::Holder, protected SkinSlot::Holder
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
		inline const Size&	size() const;
		inline Rect			geo() const;
		inline Coord		globalPos() const;
		inline Rect			globalGeo() const;

		inline Coord		toGlobal( const Coord& coord ) const;
		inline Rect			toGlobal(const Rect& rect) const;
		inline Coord		toLocal( const Coord& coord ) const;
		inline Rect			toLocal(const Rect& rect) const;

		virtual MU			matchingHeight( MU width ) const;
		virtual MU			matchingWidth( MU height ) const;

		virtual Size		preferredSize() const;
		virtual Size		minSize() const;
		virtual Size		maxSize() const;


		//.____ Hierarchy _________________________________________________

		inline Widget_p		nextSibling() const;
		inline Widget_p		prevSibling() const;
		Container_p			parent() const;

		void				releaseFromParent();

		Widget_p			commonAncestor(Widget* pOtherWidget);

		//.____ State _________________________________________________

		inline const State&	state() const;

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

		virtual bool		markTest( const Coord& ofs );
		void 				receive( Msg * pMsg ) override final;

		inline void			refresh();

		inline Widget_p		clone() const;
		inline Widget_p		newOfMyType() const;

		inline bool			isOpaque() const;

		void				setBaggage(Object * pBaggage);
		bool				hasBaggage() const;
		Object_p			baggage() const;



	protected:
		Widget();
		virtual ~Widget();

		void				_setSlot(StaticSlot * pSlot);
		SlotHolder *		_holder() { return m_pHolder; }
		const SlotHolder *	_holder() const { return m_pHolder; }
		StaticSlot *		_slot() const { return m_pSlot; }

		virtual BlendMode	_getBlendMode() const;

		int					_listAncestors(Widget* array[], int max);

		virtual Widget* 	_newOfMyType() const = 0;

		bool            	_requestPreRenderCall();

		inline Rect			_contentRect() const { return m_pSkin ? m_pSkin->contentRect(m_size, m_state) : Rect( m_size ); }
		inline Rect			_contentRect(const Rect& canvas) const { return m_pSkin ? m_pSkin->contentRect(canvas, m_state) : canvas; }
		inline Size			_contentPaddingSize() const { return m_pSkin ? m_pSkin->contentPaddingSize() : Size(); }


		// Convenient calls to holder



		inline void			_requestRender() { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot ); }
		inline void			_requestRender( const Rect& rect ) { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot, rect ); }
		inline void			_requestResize() { if( m_pHolder ) m_pHolder->_childRequestResize( m_pSlot ); }
		inline void			_requestInView() const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot ); }
		inline void			_requestInView( const Rect& mustHaveArea, const Rect& niceToHaveArea ) const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot, mustHaveArea, niceToHaveArea ); }

		inline Widget *		_nextSibling() const { if( m_pHolder ) return m_pHolder->_nextChild( m_pSlot ); else return nullptr; }
		inline Widget *		_prevSibling() const { if( m_pHolder ) return m_pHolder->_prevChild( m_pSlot ); else return nullptr; }
		inline Container *	_parent() const { if( m_pHolder ) return m_pHolder->_container(); else return nullptr; }

		inline Rect			_windowSection() const { if( m_pHolder ) return m_pHolder->_childWindowSection( m_pSlot ); return Rect(); }

		// To be overloaded by Widget

		virtual void		_collectPatches( Patches& container, const Rect& geo, const Rect& clip );
		virtual void		_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );

		Widget *			_clone() const;
		virtual void		_cloneContent( const Widget * _pOrg );

		virtual void    	_preRender();
		virtual void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window );

		virtual void		_refresh();
		virtual void		_resize( const Size& size );
		virtual void		_setState( State state );

		virtual void		_receive( Msg * pMsg );
		virtual	bool		_alphaTest( const Coord& ofs );

		virtual Size		_windowPadding() const;	// Padding of window before we get to (scrollable) content.

		// Methods for components to access

		Object * 	_object() override;
		const Object * _object() const override;

		Coord			_componentPos( const GeoComponent * pComponent ) const override;
		Size			_componentSize( const GeoComponent * pComponent ) const override;
		Rect			_componentGeo( const GeoComponent * pComponent ) const override;
		Coord			_globalComponentPos( const GeoComponent * pComponent ) const override;
		Rect			_globalComponentGeo( const GeoComponent * pComponent ) const override;

		void			_componentRequestRender( const GeoComponent * pComponent ) override;
		void			_componentRequestRender( const GeoComponent * pComponent, const Rect& rect ) override;
		void			_componentRequestResize( const GeoComponent * pComponent ) override;

 		void			_componentRequestFocus( const GeoComponent * pComponent ) override;
		void			_componentRequestInView( const GeoComponent * pComponent ) override;
		void			_componentRequestInView( const GeoComponent * pComponent, const Rect& mustHave, const Rect& niceToHave ) override;

		void			_receiveComponentNotif( GeoComponent * pComponent, ComponentNotif notification, int value, void * pData ) override;

		// Methods for skin to access

		void	_skinRequestRender(const SkinSlot* pSlot) override;
		void	_skinRequestRender(const SkinSlot* pSlot, const Rect& rect) override;

		Size	_skinSize(const SkinSlot* pSlot) const override;
		Coord	_skinGlobalPos(const SkinSlot* pSlot) const override;

		State	_skinState(const SkinSlot* pSlot) const override;


		int				m_id;
		Object_p		m_pBaggage;

		SlotHolder *	m_pHolder;
		StaticSlot *	m_pSlot;

		SkinSlot		m_pSkin;
		PointerStyle	m_pointerStyle;

		String			m_tooltip;
		int				m_markOpacity;

		bool			m_bOpaque;			// Set if widget is totally opaque, no need to render anything behind.
		bool			m_bTabLock;			// If set, the widget prevents focus shifting away from it with tab.
		bool			m_bSelectable;		// Set if widget is allowed to be selected.

		State			m_state;			// Current state of widget.
		Size			m_size;				// Current size of widget.

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
			return m_pHolder->_childPos( m_pSlot );
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
	const Size& Widget::size() const
	{
		return m_size;
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
			return Rect(m_pHolder->_childPos( m_pSlot ), m_size);
		return Rect(m_size);
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
		return m_pHolder ? m_pHolder->_childGlobalPos(m_pSlot) : Coord();
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
		return  m_pHolder ? Rect(m_pHolder->_childGlobalPos(m_pSlot), m_size) : Rect(m_size);
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
		return globalPos() + coord;
	}

	//____ toGlobal() __________________________
	/**
	 * @brief Convert rectangle from local to global space
	 *
	 * Convert rectangle from widgets local coordinate space to the global coordinate space.
	 *
	 * @param rect		Rectangle in widgets local coordinate system.
	 *
	 * Please note that the widgets local coordinate space originates from the top-left
	 * corner of its geometry box and is NOT the same as the (parents) local coordinate
	 * space in which it lives.
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 *
	 * @return Rectangle in gobal coordinate space.
	 */

	Rect Widget::toGlobal(const Rect& rect) const
	{
		return rect + globalPos();
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
		return coord - globalPos();
	}

	//____ toLocal() ____________________________________________________________
	/**
	 * @brief Convert rectangle from global to local space
	 *
	 * Convert rectangle from the global coordinate space to widgets local coordinate space.
	 *
	 * @param rect		Rectangle in widgets local coordinate system.
	 *
	 * Please note that the widgets local coordinate space originates from the top-left
	 * corner of its geometry box and is NOT the same as the (parents) local coordinate
	 * space in which it lives.
	 * The coordinate (0,0) is always the top-left corner of the widget.
	 *
	 * @return Rectangle in local coordinate space.
	 */

	Rect Widget::toLocal(const Rect& rect) const
	{
		return rect - globalPos();
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
