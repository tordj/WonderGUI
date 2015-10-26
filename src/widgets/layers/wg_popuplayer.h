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

#ifndef WG_POPUPLAYER_DOT_H
#define WG_POPUPLAYER_DOT_H

#ifndef WG_LAYER_DOT_H
#	include <wg_layer.h>
#endif

namespace wg 
{
	
	
	class PopupLayer;
	typedef	WgStrongPtr<PopupLayer,Layer_p>		PopupLayer_p;
	typedef	WgWeakPtr<PopupLayer,Layer_wp>	PopupLayer_wp;
	
	class PopupHook;
	typedef	HookTypePtr<PopupHook,LayerHook_p>	PopupHook_p;
	
	class PopupHook : public LayerHook, protected Link
	{
		friend class PopupLayer;
		friend class Chain<PopupHook>;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static PopupHook_p	cast( const Hook_p& pInterface );
	
		// Standard MyHook methods
	
		PopupHook_p	prev() const { return _prev(); }
		PopupHook_p	next() const { return _next(); }
	
		PopupLayer_p	parent() const;
	
	protected:
		// TODO: Constructor should in the future call setHook() on Widget, once we are totally rid of widgets...
	
		PROTECTED_LINK_METHODS( PopupHook );
	
		PopupHook( PopupLayer * pParent, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint, Size maxSize );
	
		void			_requestRender();
		void			_requestRender( const Rect& rect );
		void			_requestResize();
	
		LayerHook *	_prevLayerHook() const;			// Iterate through all hooks except the base hook
		LayerHook *	_nextLayerHook() const;			// Iterate through all hooks except the base hook
		Container*	_parent() const;
	
		bool			_updateGeo();
	
	
		PopupLayer *	m_pParent;
	
		Rect			m_launcherGeo;		// Launcher geo relative sibling or parent.
		Origo			m_attachPoint;
		Size			m_maxSize;
		Widget_wp	m_pOpener;			// Widget that opened this popup.
		Widget_wp	m_pKeyFocus;		// Pointer at widget that held focus when this popup was ontop.
	};
	
	
	//____ PopupLayer ____________________________________________________________
	
	class PopupLayer : public Layer
	{
		friend class PopupHook;
	
	public:
		static PopupLayer_p	create() { return PopupLayer_p(new PopupLayer()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static PopupLayer_p	cast( const Object_p& pObject );
	
		PopupHook_p	openPopup( const Widget_p& pPopup, const Widget_p& pOpener, const Rect& launcherGeo, Origo attachPoint = Origo::NorthEast, Size maxSize = Size(INT_MAX,INT_MAX) );
	
		bool			closePopup( const Widget_p& pPopup );
		bool			closeAllPopups();
	
		PopupHook_p	firstPopupHook();
		PopupHook_p	lastPopupHook();
	
		// Overloaded from Panel
	
		bool			removeWidget( const Widget_p& pWidget ) { return false; }
		bool			clear() { return false; }
	
	protected:
		PopupLayer();
		virtual ~PopupLayer();
		virtual Widget* _newOfMyType() const { return new PopupLayer(); };
	
		Widget *		_findWidget( const Coord& ofs, SearchMode mode );
	
	private:
	
		// Overloaded from Layer
	
		LayerHook *	_firstLayerHook() const { return m_popupHooks.first(); }
		LayerHook *	_lastLayerHook() const { return m_popupHooks.last(); }
	
		//
	
		PopupLayer *	_getPopupLayer() const { return const_cast<PopupLayer*>(this); }
	
		void			_stealKeyboardFocus();
		void			_restoreKeyboardFocus();
	
		//
	
		void			_onCloneContent( const Widget * _pOrg );
		void			_onNewSize( const Size& size );
		void			_onMsg( const Msg_p& pMsg );
	
		void			_onRequestRender( const Rect& rect, const PopupHook * pHook );	// rect is in our coordinate system.
	
		Chain<PopupHook>		m_popupHooks;		// First popup lies at the bottom.
	
		Widget_wp	m_pKeyFocus;		// Pointer at child that held focus before any menu was opened.
	
	
	};
	

} // namespace wg
#endif //WG_POPUPLAYER_DOT_H
