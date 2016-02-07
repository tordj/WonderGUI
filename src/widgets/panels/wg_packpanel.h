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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H

#ifndef	WG_SIZEBROKER_DOT_H
#	include <wg_sizebroker.h>
#endif

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif

namespace wg 
{
	
	class PackPanel;
	typedef	StrongPtr<PackPanel,VectorPanel_p>		PackPanel_p;
	typedef	WeakPtr<PackPanel,VectorPanel_wp>	PackPanel_wp;
	
	class PackHook;
	typedef	HookTypePtr<PackHook,VectorHook_p>	PackHook_p;
	
	class PackHook : public VectorHook
	{
		friend class PackPanel;
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static PackHook_p	cast( const Hook_p& pInterface );
		
		bool	setWeight( float weight );
		float	weight() { return m_weight; }
	
		PackHook_p	prev() const { return _prev(); }
		PackHook_p	next() const { return _next(); }
		PackPanel_p	parent() const;
	
	protected:
		PROTECTED_LINK_METHODS( PackHook );
	
		PackHook( PackPanel * pParent );
	
		Container * _parent() const;
		
		float			m_weight;			// Weight for space allocation.
		Rect			m_geo;				// Real geo of child (no padding included).
		Size			m_preferredSize;	// Cached padded preferred size from the child.
		PackPanel *	m_pParent;
	};
	
	
	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */
	
	class PackPanel : public VectorPanel
	{
		friend class PackHook;
	
	public:
		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( const Object_p& pObject );
	
		inline PackHook_p addWidget( const Widget_p& pWidget ) { return static_cast<PackHook*>(VectorPanel::_addWidget(pWidget.rawPtr())); }
		inline PackHook_p insertWidget( const Widget_p& pWidget, const Widget_p& pSibling ) { return static_cast<PackHook*>(VectorPanel::_insertWidget(pWidget.rawPtr(),pSibling.rawPtr())); }
	    
		void			setOrientation( Orientation orientaiton );
		Orientation	orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }
		
		PackHook_p	firstHook() const { return static_cast<PackHook*>(_firstHook()); }
		PackHook_p	lastHook() const { return static_cast<PackHook*>(_lastHook()); }
	
		void			setSizeBroker( const SizeBroker_p& pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }
	
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;
		
	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const { return new PackPanel(); };
	
	    // Overloaded from Widget
	    
		void			_onNewSize( const Size& size );
	 
	    
		// Overloaded from Container
		
		Hook*			_firstHookWithGeo( Rect& geo ) const;
		Hook*			_nextHookWithGeo( Rect& geo, Hook * pHook ) const;
		
		Hook*			_lastHookWithGeo( Rect& geo ) const;
		Hook*			_prevHookWithGeo( Rect& geo, Hook * pHook ) const;
		
		
		// Overloaded from VectorPanel
		
		Rect			_hookGeo( const VectorHook * pHook );
		void			_onResizeRequested( VectorHook * pHook );
		void			_onRenderRequested( VectorHook * pHook );
		void			_onRenderRequested( VectorHook * pHook, const Rect& rect );
		void			_onWidgetAppeared( VectorHook * pInserted );				// so parent can update geometry and possibly request render.
		void			_onWidgetDisappeared( VectorHook * pToBeRemoved );		// so parent can update geometry and possibly request render.
		void			_onWidgetsReordered();
		void			_refreshAllWidgets();
		VectorHook *	_newHook();
		
		//
		
		inline PackHook *	_firstHook() const { return static_cast<PackHook*>(m_hooks.first()); }
		inline PackHook *	_lastHook() const { return static_cast<PackHook*>(m_hooks.last()); }
	
	
		void			_refreshChildGeo();
		void			_updatePreferredSize();
		int				_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int				_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;
	
	
		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		Size			m_preferredSize;
	
	};
	
	

} // namespace wg
#endif //WG_PACKPANEL_DOT_H
