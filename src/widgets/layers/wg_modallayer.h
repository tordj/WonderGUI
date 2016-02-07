/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_MODALLAYER_DOT_H
#define WG_MODALLAYER_DOT_H

#ifndef WG_LAYER_DOT_H
#	include <wg_layer.h>
#endif

namespace wg 
{
	
	
	class ModalLayer;
	typedef	StrongPtr<ModalLayer,Layer_p>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer,Layer_wp>	ModalLayer_wp;
	
	class ModalHook;
	typedef	HookTypePtr<ModalHook,LayerHook_p>	ModalHook_p;
	
	class ModalHook : public LayerHook, protected Link
	{
		friend class ModalLayer;
		friend class Chain<ModalHook>;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static ModalHook_p	cast( const Hook_p& pInterface );
	
		void	top();								// Put us ontop of all our siblings.
	
		bool	setGeo( const Rect& geometry, Origo origo = Origo::NorthWest );
		bool	setGeo( const Coord& ofs, Origo origo = Origo::NorthWest );
	
		bool	setOfs( const Coord& ofs );
		bool	setOfsX( int x );
		bool	setOfsY( int y );
	
		bool	setSize( Size sz );
		bool	setWidth( int width );
		bool	setHeight( int height );
	
		bool	move( const Coord& ofs );
		bool	moveX( int x );
		bool	moveY( int y );
	
	
		// Standard MyHook methods
	
		ModalHook_p	prev() const { return _prev(); }
		ModalHook_p	next() const { return _next(); }
	
		ModalLayer_p	parent() const;
	
	protected:
		// TODO: Constructor should in the future call setHook() on Widget, once we are totally rid of widgets...
	
		PROTECTED_LINK_METHODS( ModalHook );
	
		ModalHook( ModalLayer * pParent );
	
		bool		_refreshRealGeo();	// Return false if we couldn't get exactly the requested (floating) geometry.
		void		_requestResize();
	
		LayerHook *	_prevLayerHook() const;			// Iterate through all hooks except the base hook
		LayerHook *	_nextLayerHook() const;			// Iterate through all hooks except the base hook
		Container *	_parent() const;
	
	
		ModalLayer *	m_pParent;
	
		Origo	m_origo;
		Rect			m_placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
											// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
	
		Widget_wp	m_pKeyFocus;		// Pointer at child that held focus when this modal was last on top.
	};
	
	
	class ModalLayer : public Layer
	{
		friend class ModalHook;
	
	public:
		static ModalLayer_p	create() { return ModalLayer_p(new ModalLayer()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ModalLayer_p	cast( const Object_p& pObject );
	
		ModalHook_p	addModalWidget( const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest );
		ModalHook_p	addModalWidget( const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest ) { return addModalWidget( pWidget, Rect(pos,0,0), origo); }
	
		bool			removeModalWidgets();
	
		bool			removeWidget( const Widget_p& pWidget );
		bool			clear();
	
		ModalHook_p	firstModalHook();
		ModalHook_p	lastModalHook();
	
	
		// Overloaded from Widget
	
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;
	
	protected:
		ModalLayer();
		virtual ~ModalLayer();
		virtual Widget* _newOfMyType() const { return new ModalLayer(); };
	
	
	private:
	
		ModalLayer *	_getModalLayer() const { return const_cast<ModalLayer*>(this); }
	
		void			_updateKeyboardFocus();
	
		// Overloaded from Panel
	
		Widget *		_findWidget( const Coord& ofs, SearchMode mode );
	
	
		// Overloaded from Layer
	
		LayerHook *	_firstLayerHook() const { return m_modalHooks.first(); }
		LayerHook *	_lastLayerHook() const { return m_modalHooks.last(); }
	
		//
	
		void			_onCloneContent( const Widget * _pOrg );
		void			_onNewSize( const Size& size );
		void			_onMsg( const Msg_p& pMsg );
	
		Chain<ModalHook>	m_modalHooks;		// First modal widget lies at the bottom.
	
		Widget_wp			m_pBaseKeyFocus;
	
	};
	

} // namespace wg
#endif //WG_MODALLAYER_DOT_H
