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
#ifndef WG_LAYER_DOT_H
#define WG_LAYER_DOT_H

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

namespace wg 
{
	
	class WgLayer;
	typedef	WgStrongPtr<WgLayer,WgContainer_p>	WgLayer_p;
	typedef	WgWeakPtr<WgLayer,WgContainer_p>	WgLayer_wp;
	
	class WgLayerHook;
	typedef	WgHookTypePtr<WgLayerHook,WgHook_p>	WgLayerHook_p;
	
	
	class WgLayerHook : public WgHook
	{
		friend class WgLayer;
		friend class WgModalLayer;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgLayerHook_p	cast( const WgHook_p& pInterface );
	
		WgCoord			pos() const { return m_geo.pos(); }
		WgSize			size() const { 	return m_geo.size(); }
		WgRect			geo() const { return m_geo; }
	
		WgCoord			globalPos() const;
		WgRect			globalGeo() const;
	
		WgLayerHook_p	prev() const { return _prevLayerHook(); }
		WgLayerHook_p	next() const { return _nextLayerHook(); }
		WgLayer_p		parent() const;
	
	protected:
	
		virtual WgLayerHook *	_prevLayerHook() const = 0;			// Iterate through all hooks except the base hook
		virtual WgLayerHook *	_nextLayerHook() const = 0;			// Iterate through all hooks except the base hook
		
		WgHook *		_prevHook() const;							// Iterates through all hooks, including the base hook
		WgHook *		_nextHook() const;							// Iterates through all hooks, including the base hook
		
		void			_requestRender();
		void			_requestRender( const WgRect& rect );
	
		WgRect		m_geo;
	};
	
	
	/**
	 * @brief	Base class for containers that provides layers of different kinds.
	 *
	 * WgLayer is the base class for widget containers that are used as layers.
	 *
	 * The concept of Layers in WonderGUI is that a layer is a widget that provides
	 * a special purpose layer on top of the rest of the GUI hierarchy. The placement of
	 * this layer in the widget hierarchy decides on what layer things such as popups,
	 * modal dialog boxes, tooltips, notifications etc appear. (note: not all of these
	 * mentioned conceptual layers are implemented yet).
	 *
	 * For example, a WgMenubar widget that opens a menu will place the menu widget in the
	 * first WgPopupLayer it encounters while traversing it ancestors from itself upwards.
	 * In similar way, placing a widget in a WgModalLayer will make it modal only against the
	 * children of that specific layer.
	 *
	 * All Layers have a BaseChild (or BaseWidget), which is the child through which the normal hierarchy continues.
	 * Children that resides within the layer itself are usually referred to as Layer Children.
	 *
	 **/
	
	class WgLayer : public WgContainer
	{
		friend class WgLayerHook;
	
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static WgLayer_p	cast( const WgObject_p& pObject );
	
		WgHook_p			setBaseWidget( const WgWidget_p& pWidget );
		WgWidget_p			baseWidget();
		bool				removeBaseWidget();
		inline WgHook_p	baseHook() { return &m_baseHook; }
	
		inline WgLayerHook_p	firstLayerHook() const { return _firstLayerHook(); }
		inline WgLayerHook_p	lastLayerHook() const { return _lastLayerHook(); }
	
		// Overloaded from WgWidget
	
		int					matchingHeight( int width ) const;
		int					matchingWidth( int height ) const;
	
		WgSize				preferredSize() const;
	
	protected:
		WgLayer();
	
		class _BaseHook : public WgHook
		{
			friend class WgLayer;
			friend class WgLayerHook;
			friend class WgModalLayer;
			friend class WgPopupLayer;
	
		public:
	
			// Standard Hook methods
	
			WgCoord		pos() const { return m_pParent->pos(); }
			WgSize		size() const { 	return m_pParent->size(); }
			WgRect		geo() const { return m_pParent->geo(); }
	
			WgCoord		globalPos() const { return m_pParent->globalPos(); }
			WgRect		globalGeo() const { return m_pParent->globalGeo(); }
	
		protected:
	
			void		_requestRender();
			void		_requestRender( const WgRect& rect );
			void		_requestResize();
	
			WgHook *		_prevHook() const { return 0; }
			WgHook *		_nextHook() const { return m_pParent->_firstLayerHook(); }
			WgContainer*	_parent() const { return m_pParent; }
	
			WgLayer * 	m_pParent;
		};
	
		_BaseHook		m_baseHook;
	
		virtual	void	_onRequestRender( const WgRect& rect, const WgLayerHook * pHook );	// rect is in our coordinate system.
		virtual WgLayerHook * _firstLayerHook() const = 0;
		virtual WgLayerHook * _lastLayerHook() const = 0;
	
		WgHook *			_firstHook() const;		// Fist Hook returned is for the base, then follows the LayerHooks.
		WgHook *			_lastHook() const;		//
	
		WgHook *		_firstHookWithGeo( WgRect& geo ) const;
		WgHook *		_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
		WgHook *		_lastHookWithGeo( WgRect& geo ) const;
		WgHook *		_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;
	
	
		virtual void	 _onBaseChanged();
	
		WgSize			m_size;
	
	};
	
	

} // namespace wg
#endif //WG_LAYER_DOT_H
