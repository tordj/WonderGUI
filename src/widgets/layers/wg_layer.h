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
	
	class Layer;
	typedef	StrongPtr<Layer,Container_p>	Layer_p;
	typedef	WeakPtr<Layer,Container_p>	Layer_wp;
	
	class LayerHook;
	typedef	HookTypePtr<LayerHook,Hook_p>	LayerHook_p;
	
	
	class LayerHook : public Hook
	{
		friend class Layer;
		friend class ModalLayer;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static LayerHook_p	cast( const Hook_p& pInterface );
	
		Coord			pos() const { return m_geo.pos(); }
		Size			size() const { 	return m_geo.size(); }
		Rect			geo() const { return m_geo; }
	
		Coord			globalPos() const;
		Rect			globalGeo() const;
	
		LayerHook_p	prev() const { return _prevLayerHook(); }
		LayerHook_p	next() const { return _nextLayerHook(); }
		Layer_p		parent() const;
	
	protected:
	
		virtual LayerHook *	_prevLayerHook() const = 0;			// Iterate through all hooks except the base hook
		virtual LayerHook *	_nextLayerHook() const = 0;			// Iterate through all hooks except the base hook
		
		Hook *		_prevHook() const;							// Iterates through all hooks, including the base hook
		Hook *		_nextHook() const;							// Iterates through all hooks, including the base hook
		
		void			_requestRender();
		void			_requestRender( const Rect& rect );
	
		Rect		m_geo;
	};
	
	
	/**
	 * @brief	Base class for containers that provides layers of different kinds.
	 *
	 * Layer is the base class for widget containers that are used as layers.
	 *
	 * The concept of Layers in WonderGUI is that a layer is a widget that provides
	 * a special purpose layer on top of the rest of the GUI hierarchy. The placement of
	 * this layer in the widget hierarchy decides on what layer things such as popups,
	 * modal dialog boxes, tooltips, notifications etc appear. (note: not all of these
	 * mentioned conceptual layers are implemented yet).
	 *
	 * For example, a Menubar widget that opens a menu will place the menu widget in the
	 * first PopupLayer it encounters while traversing it ancestors from itself upwards.
	 * In similar way, placing a widget in a ModalLayer will make it modal only against the
	 * children of that specific layer.
	 *
	 * All Layers have a BaseChild (or BaseWidget), which is the child through which the normal hierarchy continues.
	 * Children that resides within the layer itself are usually referred to as Layer Children.
	 *
	 **/
	
	class Layer : public Container
	{
		friend class LayerHook;
	
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Layer_p	cast( const Object_p& pObject );
	
		Hook_p			setBaseWidget( const Widget_p& pWidget );
		Widget_p			baseWidget();
		bool				removeBaseWidget();
		inline Hook_p	baseHook() { return &m_baseHook; }
	
		inline LayerHook_p	firstLayerHook() const { return _firstLayerHook(); }
		inline LayerHook_p	lastLayerHook() const { return _lastLayerHook(); }
	
		// Overloaded from Widget
	
		int					matchingHeight( int width ) const;
		int					matchingWidth( int height ) const;
	
		Size				preferredSize() const;
	
	protected:
		Layer();
	
		class _BaseHook : public Hook
		{
			friend class Layer;
			friend class LayerHook;
			friend class ModalLayer;
			friend class PopupLayer;
	
		public:
	
			// Standard MyHook methods
	
			Coord		pos() const { return m_pParent->pos(); }
			Size		size() const { 	return m_pParent->size(); }
			Rect		geo() const { return m_pParent->geo(); }
	
			Coord		globalPos() const { return m_pParent->globalPos(); }
			Rect		globalGeo() const { return m_pParent->globalGeo(); }
	
		protected:
	
			void		_requestRender();
			void		_requestRender( const Rect& rect );
			void		_requestResize();
	
			Hook *		_prevHook() const { return 0; }
			Hook *		_nextHook() const { return m_pParent->_firstLayerHook(); }
			Container*	_parent() const { return m_pParent; }
	
			Layer * 	m_pParent;
		};
	
		_BaseHook		m_baseHook;
	
		virtual	void	_onRequestRender( const Rect& rect, const LayerHook * pHook );	// rect is in our coordinate system.
		virtual LayerHook * _firstLayerHook() const = 0;
		virtual LayerHook * _lastLayerHook() const = 0;
	
		Hook *			_firstHook() const;		// Fist MyHook returned is for the base, then follows the LayerHooks.
		Hook *			_lastHook() const;		//
	
		Hook *		_firstHookWithGeo( Rect& geo ) const;
		Hook *		_nextHookWithGeo( Rect& geo, Hook * pHook ) const;
	
		Hook *		_lastHookWithGeo( Rect& geo ) const;
		Hook *		_prevHookWithGeo( Rect& geo, Hook * pHook ) const;
	
	
		virtual void	 _onBaseChanged();
	
		Size			m_size;
	
	};
	
	

} // namespace wg
#endif //WG_LAYER_DOT_H
