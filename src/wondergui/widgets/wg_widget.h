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
#include <wg_slotholder.h>
#include <wg_skinslot.h>
#include <wg_patches.h>
#include <wg_util.h>

namespace wg
{
	class GfxDevice;
	class Container;
	class List;
	class Capsule;
	class Layer;
	class MsgRouter;
	class SlotHolder;
    class Component;

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

	class Widget : public Receiver, protected SkinSlot::Holder
	{
		friend class Container;
		friend class Component;
		friend class StaticSlot;
		friend class DynamicSlot;
		friend class Base;

		template<class S> friend class StaticSlotVector;
		template<class S> friend class DynamicSlotVector;
		template<class S, int X> friend class SlotArray;

	public:

		//.____ Blueprint _____________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			bool			selectable = true;
			Skin_p			skin;
			bool			stickyFocus = false;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Identification ________________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		inline void			setId(int id);
		inline int			id() const;

		virtual bool		isContainer() const;

		//.____ Geometry ______________________________________________________

		inline int			scale() const;
	
		inline Coord		pos() const;
		inline const Size	size() const;
		inline Rect			geo() const;
		inline Coord		globalPos() const;
		inline Rect			globalGeo() const;

		inline Coord		toGlobal( const Coord& coord ) const;
		inline Rect			toGlobal(const Rect& rect) const;
		inline Coord		toLocal( const Coord& coord ) const;
		inline Rect			toLocal(const Rect& rect) const;

		inline pts			matchingHeight( pts width ) const;
		inline pts			matchingWidth( pts height ) const;

		inline Size			defaultSize() const;
		inline Size			minSize() const;
		inline Size			maxSize() const;

		inline CoordSPX			spxPos() const { return _pos(); }
		inline const SizeSPX& spxSize() const { return _size(); }
		inline RectSPX			spxGeo() const { return _geo(); }
		inline CoordSPX			spxGlobalPos() const { return _globalPos(); }
		inline RectSPX			spxGlobalGeo() const { return _globalGeo(); }

		//.____ Hierarchy _____________________________________________________

		inline Widget_p		nextSibling() const;
		inline Widget_p		prevSibling() const;
		Container_p			parent() const;

		void				releaseFromParent();

		Widget_p			commonAncestor(Widget* pOtherWidget);

		//.____ State _________________________________________________________

		inline const State&	state() const;

		void				setEnabled(bool bEnabled);
		inline bool			isEnabled() const;

		void				setSelectable(bool bSelectable);
		inline bool			isSelectable() const;

		bool				grabFocus(bool bRaiseWindow = false);
		bool				releaseFocus();
		bool				isFocused();


		//.____ Appearance ____________________________________________________

		virtual void		setSkin(Skin* pSkin);
		inline Skin_p		skin() const;

		inline void			setTooltip(const String& str);
		inline virtual String tooltip() const;

		inline void			setPointerStyle(PointerStyle style);
		virtual PointerStyle pointerStyle() const;


		//.____ Behavior ______________________________________________________

		inline void			setMarkPolicy(MarkPolicy policy);
		inline MarkPolicy	markPolicy() const;

		inline void			setTabLock(bool bLock);
		inline bool			isTabLocked() const;

		void                setPickable( bool bPickable, int category = 0 );
		inline bool         isPickable() const;
		inline int          pickCategory() const;

		void                setDropTarget( bool bDropTarget );
		inline bool         isDropTarget() const;

		void				setStickyFocus(bool bSticky);
		inline bool			hasStickyFocus() const { return m_bStickyFocus; }


		//.____ Misc __________________________________________________________

		bool				markTest( const Coord& ofs );
		void 				receive( Msg * pMsg ) override final;

		inline bool			isOpaque() const;

		void				setBaggage(Object * pBaggage);
		bool				hasBaggage() const;
		Object_p			baggage() const;

		//.____ Internal ______________________________________________________

		inline Container* _parent() const { return m_pHolder ? m_pHolder->_container() : nullptr; }
		inline Widget* _nextSibling() const { return m_pHolder ? m_pHolder->_nextChild(m_pSlot) : nullptr; }
		inline Widget* _prevSibling() const { return m_pHolder ? m_pHolder->_prevChild(m_pSlot) : nullptr; }

		virtual spx			_matchingHeight(spx width, int scale) const;
		virtual spx			_matchingWidth(spx height, int scale) const;

		virtual SizeSPX		_defaultSize(int scale) const;
		virtual SizeSPX		_minSize(int scale) const;
		virtual SizeSPX		_maxSize(int scale) const;

		inline const SizeSPX& _size() const;
		inline int			_scale() const override;

		inline CoordSPX		_pos() const;
		inline RectSPX		_geo() const;
		inline CoordSPX		_globalPos() const;
		inline RectSPX		_globalGeo() const;

		inline CoordSPX		_toGlobal(const CoordSPX& coord) const;
		inline RectSPX		_toGlobal(const RectSPX& rect) const;
		inline CoordSPX		_toLocal(const CoordSPX& coord) const;
		inline RectSPX		_toLocal(const RectSPX& rect) const;

		virtual	bool		_markTest(const CoordSPX& ofs);

		virtual void		_collectPatches(PatchesSPX& container, const RectSPX& geo, const RectSPX& clip);
		virtual void		_maskPatches(PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode);

		virtual void    	_preRender();
		virtual void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window);

		virtual void		_resize(const SizeSPX& size, int scale);
		virtual void		_setState(State state);

		virtual void		_receive(Msg* pMsg);
		virtual	bool		_alphaTest(const CoordSPX& ofs);

		virtual SizeSPX		_windowPadding() const;	// Padding of window before we get to (scrollable) content.

		StaticSlot* 		_slot() const { return m_pSlot; }

	protected:
		Widget();
		template<class BP> Widget( const BP& bp ) : m_skin(this)
		{
			m_pBaggage		= bp.baggage;
			m_bDropTarget	= bp.dropTarget;
			m_id			= bp.id;
			m_markPolicy	= bp.markPolicy;
			m_bPickable		= bp.pickable;
			m_pickCategory	= bp.pickCategory;
			m_pointerStyle	= bp.pointer;
			m_bSelectable	= bp.selectable;
			m_bStickyFocus	= bp.stickyFocus;
			m_bTabLock		= bp.tabLock;
			m_tooltip		= bp.tooltip;
			
			if( bp.finalizer )
				setFinalizer(bp.finalizer);

			if (bp.skin)
			{
				m_skin.set(bp.skin);
				m_bOpaque = bp.skin->isOpaque(m_state);
			}

			if( !bp.enabled )
				setEnabled(bp.enabled);
		}

		virtual ~Widget();

		void				_setSlot(StaticSlot * pSlot);
		SlotHolder *		_holder() { return m_pHolder; }
		const SlotHolder *	_holder() const { return m_pHolder; }

		virtual BlendMode	_getBlendMode() const;

		int					_listAncestors(Widget* array[], int max);

		int64_t				_startReceiveUpdates();
		void				_stopReceiveUpdates();

		bool            	_requestPreRenderCall();

		inline RectSPX		_contentRect() const { return m_skin.contentRect(m_size, m_scale, m_state); }
		inline RectSPX		_contentRect(const RectSPX& canvas) const { return m_skin.contentRect(canvas, m_scale, m_state); }
		inline SizeSPX		_contentBorderSize() const { return m_skin.contentBorderSize(m_scale); }

		// Convenient calls to holder

		inline void			_requestRender() { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot ); }
		inline void			_requestRender( const RectSPX& rect ) { if( m_pHolder ) m_pHolder->_childRequestRender( m_pSlot, rect ); }
		inline void			_requestResize() { if( m_pHolder ) m_pHolder->_childRequestResize( m_pSlot ); }
		inline void			_requestInView() const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot ); }
		inline void			_requestInView( const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea ) const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pSlot, mustHaveArea, niceToHaveArea ); }

		inline RectSPX		_windowSection() const { if( m_pHolder ) return m_pHolder->_childWindowSection( m_pSlot ); return RectSPX(); }


		// Methods for components to access

		virtual State		_componentState(const Component* pComponent) const;
		virtual CoordSPX	_componentPos( const Component * pComponent ) const;
		virtual SizeSPX		_componentSize( const Component * pComponent ) const;
		virtual RectSPX		_componentGeo( const Component * pComponent ) const;
		virtual CoordSPX	_globalComponentPos( const Component * pComponent ) const;
		virtual RectSPX		_globalComponentGeo( const Component * pComponent ) const;

		virtual void		_componentRequestRender( const Component * pComponent );
		virtual void		_componentRequestRender( const Component * pComponent, const RectSPX& rect );
		virtual void		_componentRequestResize( const Component * pComponent );

		virtual void		_componentRequestFocus( const Component * pComponent, bool bRaiseWindow );
		virtual void		_componentRequestInView( const Component * pComponent );
		virtual void		_componentRequestInView( const Component * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave );

		virtual void		_receiveComponentNotif( Component * pComponent, ComponentNotif notification, int value, void * pData );

		// Methods for skin to access

		float			_skinValue(const SkinSlot* pSlot) const override;
		float			_skinValue2(const SkinSlot* pSlot) const override;

		State			_skinState(const SkinSlot* pSlot) const override;

		SizeSPX			_skinSize(const SkinSlot* pSlot) const override;

		void			_skinRequestRender(const SkinSlot* pSlot) override;
		void			_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect) override;


		Object_p		m_pBaggage;

		SkinSlot		m_skin;

		SlotHolder *	m_pHolder = nullptr;
		StaticSlot *	m_pSlot = nullptr;

		String			m_tooltip;

		int				m_id = 0;
		PointerStyle	m_pointerStyle = PointerStyle::Default;
		MarkPolicy		m_markPolicy = MarkPolicy::AlphaTest;
		State			m_state;						// Current state of widget.

		uint16_t		m_scale = 64;

		bool            m_bPickable = false;			// Set if this widget accepts to be the source of drag-n-drop operations.
		uint8_t         m_pickCategory = 0;				// Category of drag-n-drop operations. User defined.

		bool            m_bDropTarget = false;			// Set if this widget accepts to be the target of drag-n-drop operations.

		bool			m_bOpaque = false;				// Set if widget is totally opaque, no need to render anything behind.
		bool			m_bTabLock = false;				// If set, the widget prevents focus shifting away from it with tab.
		bool			m_bSelectable = true;			// Set if widget is allowed to be selected.
		bool			m_bReceivingUpdates = false;	//
		bool			m_bPressed = false;				// Keeps track of pressed button when mouse leaves/re-enters widget.
		bool			m_bStickyFocus = true;			// Set if widget should keep keyboard focus when mouse button pressed outside it.


		SizeSPX			m_size = { 256 * 64,256 * 64 };	// Current size of widget.
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

	//____ scale() _______________________________________
	/**
	 * @brief Get scale of widget.
	 *
	 * Get the scale of the widget.
	 * 
	 * The scale value specifies the size of a point in subpixels (1/64th pixel),
	 * thus is 64 the default scale where one point equals one pixel. A scale
	 * value of 128 doubles the size of the widget (if allowed by the parent)
	 * and all content within, including fonts and graphics.
	 * 
	 * @return Scale measured in subpixels per point.
	 */

	int Widget::scale() const
	{
		return m_scale;
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
		return Util::spxToPts(_pos(), m_scale);
	}

	//____ size() __________________________
	/**
	 * @brief	Get the size of of widget.
	 *
	 * Get the size (width and height) of widget.
	 *
	 * @return Size of widget.
	 */
	const Size Widget::size() const
	{
		return Util::spxToPts(_size(), m_scale);
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
		return Util::spxToPts(_geo(), m_scale);
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
		return Util::spxToPts(_globalPos(), m_scale);
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
		return  Util::spxToPts(_globalGeo(), m_scale);
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

	//____ matchingHeight() _______________________________________________________
	/**
	 * @brief Get the height recommended by the widget for the specified width.
	 *
	 * Get the height wanted by the widget for the specified width.
	 *
	 * @param width		Width in points.
	 *
	 * This method is used by containers to get the recommended height of a widget for which
	 * it has already decided the width.
	 *
	 * @return The recommended height for the given width.
	 */

	pts Widget::matchingHeight( pts width ) const
	{
		return Util::spxToPts(_matchingHeight( Util::ptsToSpx(width, m_scale), m_scale ), m_scale);
	}

	//____ matchingWidth() _______________________________________________________
	/**
	 * @brief Get the width recommended by the widget for the specified height.
	 *
	 * Get the width wanted by the widget for the specified height.
	 *
	 * @param height	Height in points.
	 *
	 * This method is used by containers to get the recommended width of a widget for which
	 * it has already decided the height.
	 *
	 * @return The recommended width for the given height.
	 */
	 
	pts Widget::matchingWidth( pts height ) const
	{
		return Util::spxToPts(_matchingWidth(Util::ptsToSpx(height, m_scale), m_scale), m_scale);		// Default is to stick with default height no matter what width.
	}

	//____ defaultSize() ________________________________________________________
	/**
	 * @brief Get the widgets default size.
	 *
	 * Get the widgets default size.
	 *
	 * Each widget has its own default size, which is depending on things such as
	 * skinning, content and (in the case of containers) size and layout of children.
	 *
	 * A container holding a widget will strive to provide the widget its default size, given
	 * the constraints and limitations the container needs to work with. If a container can't
	 * provide a widget its default size, it is likely to decide the closest width or height
	 * that it can provide and then make a second call to either matchingWidth() or matchingHeight()
	 * after which it will decide the size of the child.
	 *
	 * @return The default size of the widget.
	 */

	Size Widget::defaultSize() const
	{
		return Util::spxToPts(_defaultSize(m_scale), m_scale);
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
	 * The minimum size is only a hint for the widgets parent, which should strive to not
	 * make a child smaller than its minimum size, but is allowed to set the child to
	 * any size it decides, including 0.0.
	 *
	 * @return The minimum size of the widget.
	 */

	Size Widget::minSize() const
	{
		return Util::spxToPts(_minSize(m_scale), m_scale);
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
	 * The maximum size is only a hint for the widgets parent, which should strive to not
	 * make a child larger than its maximum size, but is allowed to set the child to
	 * any reasonable size it decides.
	 *
	 * @return The maximum size of the widget.
	 */

	//____ maxSize() ______________________________________________________________

	Size Widget::maxSize() const
	{
		return Util::spxToPts(_maxSize(m_scale), m_scale);
	}

	//____ _pos() __________________________

	CoordSPX Widget::_pos() const
	{
		if (m_pHolder)
			return m_pHolder->_childPos(m_pSlot);
		return CoordSPX(0, 0);
	}

	//____ size() __________________________

	const SizeSPX& Widget::_size() const
	{
		return m_size;
	}

	//____ _scale() ___________________________________________________________

	int Widget::_scale() const
	{
		return m_scale;
	}

	//____ _geo() __________________________

	RectSPX Widget::_geo() const
	{
		if (m_pHolder)
			return RectSPX(m_pHolder->_childPos(m_pSlot), m_size);
		return RectSPX(m_size);
	}

	//____ _globalPos() __________________________

	CoordSPX Widget::_globalPos() const
	{
		return m_pHolder ? m_pHolder->_childGlobalPos(m_pSlot) : CoordSPX();
	}

	//____ _globalGeo() __________________________

	RectSPX Widget::_globalGeo() const
	{
		return  m_pHolder ? RectSPX(m_pHolder->_childGlobalPos(m_pSlot), m_size) : RectSPX(m_size);
	}

	//____ _toGlobal() __________________________

	CoordSPX Widget::_toGlobal(const CoordSPX& coord) const
	{
		return _globalPos() + coord;
	}

	RectSPX Widget::_toGlobal(const RectSPX& rect) const
	{
		return rect + _globalPos();
	}

	//____ _toLocal() ____________________________________________________________

	CoordSPX Widget::_toLocal(const CoordSPX& coord) const
	{
		return coord - _globalPos();
	}

	RectSPX Widget::_toLocal(const RectSPX& rect) const
	{
		return rect - _globalPos();
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

	//____ setTooltip() _______________________________________________________
	/**
	 * @brief Set widgets tooltip string.
	 *
	 * Sets a string that is used as the tooltip for the widget. A tooltip with this
	 * text will be displayed by the TooltipOverlay (if one is present in the hierarchy) when 
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

	//____ skin() _____________________________________________________________

	Skin_p Widget::skin() const
	{
		return m_skin.get();
	}

	//____ setPointerStyle() __________________________________________________

	void Widget::setPointerStyle(PointerStyle style) 
	{ 
		m_pointerStyle = style; 
	}

	//____ setMarkPolicy() ___________________________________________________

	void Widget::setMarkPolicy(MarkPolicy policy) 
	{ 
		m_markPolicy = policy; 
	}

	//____ markPolicy() ______________________________________________________

	MarkPolicy Widget::markPolicy() const 
	{ 
		return m_markPolicy; 
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

	//____ isOpaque() _________________________________________________________

	bool Widget::isOpaque() const 
	{ 
		return m_bOpaque; 
	}

} // namespace wg
#endif
