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

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

#ifndef WG_HOOK_DOT_H
#	include <wg_hook.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_RECEIVER_DOT_H
#	include <wg_receiver.h>
#endif

#ifndef WG_ITEMHOLDER_DOT_H
#	include <wg_itemholder.h>
#endif

#ifndef WG_WIDGETHOLDER_DOT_H
#	include <wg_widgetholder.h>
#endif

namespace wg 
{
	class Item;
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
	typedef	StrongPtr<Widget,Receiver_p>	Widget_p;
	typedef	WeakPtr<Widget,Receiver_wp>		Widget_wp;
	
	class Container;
	typedef	StrongPtr<Container,Widget_p>	Container_p;
	typedef	WeakPtr<Container,Widget_wp>	Container_wp;
	
	class Msg;
	typedef	StrongPtr<Msg,Object_p>			Msg_p;
	typedef	WeakPtr<Msg,Object_wp>			Msg_wp;
	
	
	/**
	 * @brief Base class for widgets.
	 * 
	 * Widget is the base class for all widgets, providing common functionality.
	 */
	
	class Widget : public Receiver, protected ItemHolder
	{
	friend class MsgRouter;
	friend class InputHandler;
	
	friend class Hook;
	friend class FlexHook;
	friend class ModalHook;
	friend class ListHook;
	friend class StackHook;
	
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
	friend class PanelHook;
	friend class VectorPanel;
	friend class PackPanel;
	friend class ShaderCapsule;
	friend class PopupLayer;
		
	friend class Item;
	template<class Slot> friend class ManagedSlot;

	public:
		Widget();
		virtual ~Widget();
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Widget_p		cast( const Object_p& pObject );
	
		inline int			id() const { return m_id; }
		inline void			setId( int id ) { m_id = id; }
	
		virtual String		tooltip() const { return m_tooltip; }
		inline void			setTooltip( const String& str ) { m_tooltip = str; }
	
		inline void			refresh() { _refresh(); }
		void				setEnabled(bool bEnabled);
		inline bool			isEnabled() const { return m_state.isEnabled(); }
	
		inline State		state() const { return m_state; }
	
		inline Widget_p		clone() const { return Widget_p(_clone()); }
	
		void				setPointerStyle( PointerStyle style )	{ m_pointerStyle = style; }
		virtual PointerStyle	pointerStyle() const;
	
		void				setTabLock( bool bLock ) { m_bTabLock = bLock; }
		bool				isTabLocked() const { return m_bTabLock; }
	
		void				setMarkOpacity( int opacity ) { m_markOpacity = opacity; }
		int					markOpacity() const { return m_markOpacity; }
		bool				markTest( const Coord& ofs );
	
		virtual void		setSkin( const Skin_p& pSkin );
		Skin_p				skin( ) const	{ return m_pSkin; }
		
		Widget_p			newOfMyType() const { return Widget_p(_newOfMyType() ); } ///< @brief Create and return a new widget of the same type.
	
		void 				receive( const Msg_p& pMsg );
	
	
		// Convenient calls to hook
	
		inline Coord		pos() const;
		inline Size			size() const;
		inline Rect			geo() const;
		inline Coord		globalPos() const;
		inline Rect			globalGeo() const;
		inline bool			grabFocus();
		inline bool			releaseFocus();
		inline bool			isFocused();
		Container_p			parent() const;
	
		inline Widget_p		nextSibling() const;
		inline Widget_p		prevSibling() const;
	
		Coord				toGlobal( const Coord& coord ) const;
		Coord				toLocal( const Coord& coord ) const; 
		
		// To be overloaded by Widget
	
		virtual int			matchingHeight( int width ) const;
		virtual int			matchingWidth( int height ) const;
	
		virtual Size		preferredSize() const;	
		virtual Size		minSize() const;
		virtual Size		maxSize() const;
	
		virtual bool		isContainer() const { return false; }	///< @brief Check if this widget is a container.
																	///<
																	///< Check if widget is a container.
																	///< This method is a quicker way to check if the widget
																	///< is a container than calling isInstanceOf(Container::CLASS).
																	///< @return True if the widget is a subclass of Container.
	
	protected:	
		inline void			_setHolder( WidgetHolder * pHolder, void * pHoldersRef ) { m_pHolder = pHolder; m_pHoldersRef = pHoldersRef; }
		WidgetHolder *		_holder() const { return m_pHolder; }
		void *				_holdersRef() const { return m_pHoldersRef; }

		virtual BlendMode	_getBlendMode() const;
	
	
		virtual Widget* 	_newOfMyType() const = 0;
	

		// Convenient calls to holder
	
		inline void		_requestRender() { if( m_pHolder ) m_pHolder->_childRequestRender( m_pHoldersRef ); }
		inline void		_requestRender( const Rect& rect ) { if( m_pHolder ) m_pHolder->_childRequestRender( m_pHoldersRef, rect ); }
		inline void		_requestResize() { if( m_pHolder ) m_pHolder->_childRequestResize( m_pHoldersRef ); }
		inline void		_requestInView() const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pHoldersRef ); }
		inline void		_requestInView( const Rect& mustHaveArea, const Rect& niceToHaveArea ) const { if( m_pHolder ) m_pHolder->_childRequestInView( m_pHoldersRef, mustHaveArea, niceToHaveArea ); }
		
	
		inline Widget *	_nextSibling() const { if( m_pHolder ) return m_pHolder->_nextChild( m_pHoldersRef ); else return nullptr; }
		inline Widget *	_prevSibling() const { if( m_pHolder ) return m_pHolder->_prevChild( m_pHoldersRef ); else return nullptr; }
		inline Container *	_parent() const { if( m_pHolder ) return m_pHolder->_childParent(); else return nullptr; }
	
		inline Rect		_windowSection() const { if( m_pHolder ) return m_pHolder->_childWindowSection( m_pHoldersRef ); return Rect(); }
	
		// To be overloaded by Widget
	
		virtual void	_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
		virtual void	_collectPatches( Patches& container, const Rect& geo, const Rect& clip );
		virtual void	_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );

		Widget *		_clone() const;
		virtual void	_cloneContent( const Widget * _pOrg );
		virtual void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
	
		virtual void	_refresh();
		virtual void	_setSize( const Size& size );
		virtual void	_setSkin( const Skin_p& pSkin );
		virtual void	_setState( State state );
	
		virtual void	_receive( const Msg_p& pMsg );
		virtual	bool	_alphaTest( const Coord& ofs );
	
		virtual Size	_windowPadding() const;	// Padding of window before we get to (scrollable) content.

		// Methods for items to access

		virtual Object * _itemObject();
		virtual const Object * _itemObject() const;

		virtual Coord	_itemPos( const Item * pItem ) const;
		virtual Size	_itemSize( const Item * pItem ) const;
		virtual Rect	_itemGeo( const Item * pItem ) const;
		virtual Coord	_itemGlobalPos( const Item * pItem ) const;
		virtual Rect	_itemGlobalGeo( const Item * pItem ) const;

		virtual void	_itemRenderRequested( const Item * pItem );
		virtual void	_itemRenderRequested( const Item * pItem, const Rect& rect );
		virtual void	_itemResizeRequested( const Item * pItem );

 		virtual void	_itemFocusRequested( const Item * pItem );
		virtual void	_itemVisibilityRequested( const Item * pItem );
		virtual void	_itemVisibilityRequested( const Item * pItem, const Rect& preferred, const Rect& prio );
		
		virtual void	_itemNotified( Item * pItem, ItemNotif notification, void * pData );
	
		//
	
		int				m_id;

		WidgetHolder *	m_pHolder;
		void *			m_pHoldersRef;
	
		Skin_p			m_pSkin;
		PointerStyle	m_pointerStyle;
	
		String			m_tooltip;
		int				m_markOpacity;
	
		bool			m_bOpaque;
		bool			m_bTabLock;		// If set, the widget prevents focus shifting away from it with tab.
	
		State			m_state;
		Size			m_size;

	private:
		bool			m_bPressed;		// Keeps track of pressed button when mouse leaves/re-enters widget.
	
	};
	
	typedef	int(*WidgetSortFunc)(const Widget *,const Widget *);
	
	
	
	//____ Inline methods __________________________________________________________
	
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
			return m_pHolder->_childPos( m_pHoldersRef ); 
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
		return m_size; 
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
			return Rect(m_pHolder->_childPos( m_pHoldersRef ),m_size); 
		return Rect(0,0,m_size); 
	}
	
	/**
	 * @brief	Get the global position of widget.
	 * 
	 * Get the global position of widet, which is the position relative to RootPanel.
	 * 
	 * @return Global position in pixels. 
	 */
	Coord Widget::globalPos() const 
	{ 
		if( m_pHolder ) 
			return m_pHolder->_childGlobalPos( m_pHoldersRef ); 
		return Coord(0,0); 
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
		if( m_pHolder ) 
			return Rect(m_pHolder->_childGlobalPos( m_pHoldersRef ), m_size); 
		return Rect(0,0,m_size); 
	}
	
	
	bool Widget::grabFocus() 
	{ 
		if( m_pHolder ) 
			return m_pHolder->_childRequestFocus( m_pHoldersRef, this ); 
		return false; 
	}
	
	bool Widget::releaseFocus() 
	{ 
		if( m_pHolder ) 
			return m_pHolder->_childReleaseFocus( m_pHoldersRef, this ); 
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
			return m_pHolder->_nextChild( m_pHoldersRef );
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
			return m_pHolder->_prevChild( m_pHoldersRef );
		return 0; 
	}
	
	

} // namespace wg
#endif
