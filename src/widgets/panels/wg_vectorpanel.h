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

#ifndef WG_VECTORPANEL_DOT_H
#define WG_VECTORPANEL_DOT_H

#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

namespace wg 
{
	
	class WgVectorPanel;
	typedef	WgStrongPtr<WgVectorPanel,WgPanel_p>		WgVectorPanel_p;
	typedef	WgWeakPtr<WgVectorPanel,WgPanel_wp>	WgVectorPanel_wp;
	
	class WgVectorHook;
	typedef	WgHookTypePtr<WgVectorHook,WgPanelHook_p>	WgVectorHook_p;
	
	//____ WgVectorHook ___________________________________________________________
	
	class WgVectorHook : public WgPanelHook, protected WgLink
	{
		friend class WgVectorPanel;
		friend class WgChain<WgVectorHook>;
	
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static WgVectorHook_p	cast( const WgHook_p& pInterface );
	
		WgCoord	pos() const;
		WgSize	size() const;
		WgRect	geo() const;
		WgCoord	globalPos() const;
		WgRect	globalGeo() const;
	
		WgVectorHook_p	prev() const { return _prev(); }
		WgVectorHook_p	next() const { return _next(); }
		WgVectorPanel_p parent() const;
	
		bool			moveForward();
		bool			moveBackward();
		bool			moveBefore( const WgVectorHook_p& pSibling );
		bool			moveAfter( const WgVectorHook_p& pSibling );
		bool			moveFirst();
		bool			moveLast();
	
		bool			setVisible( bool bVisible );
	
	protected:
		PROTECTED_LINK_METHODS( WgVectorHook );
	
		WgVectorHook();
		~WgVectorHook();
	
		void	_requestRender();
		void	_requestRender( const WgRect& rect );
		void	_requestResize();
	
		WgHook *	_prevHook() const;
		WgHook *	_nextHook() const;
	};
	
	//____ WgVectorPanel __________________________________________________________
	
	class WgVectorPanel : public WgPanel
	{
		friend class WgVectorHook;
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgVectorPanel_p	cast( const WgObject_p& pObject );
	
		WgVectorHook_p addWidget( const WgWidget_p& pWidget ) { return _addWidget(pWidget.rawPtr() ); }
		WgVectorHook_p insertWidget( const WgWidget_p& pWidget, const WgWidget_p& pSibling ) { return _insertWidget( pWidget.rawPtr(), pSibling.rawPtr() ); }
	
		bool			removeWidget( const WgWidget_p& pWidget );
		bool			clear();
	
	protected:
		WgVectorPanel();
		virtual ~WgVectorPanel();
	
		WgVectorHook *	_addWidget( WgWidget * pWidget );
		WgVectorHook *	_insertWidget( WgWidget * pWidget, WgWidget * pSibling );
	
	
		void			_onCloneContent( const WgWidget * _pOrg );
	
		WgHook*			_firstHook() const { return m_hooks.first(); }
		WgHook*			_lastHook() const { return m_hooks.last(); }
	
		// To be overloaded by subclasses
	
		virtual WgRect	_hookGeo( const WgVectorHook * pHook ) = 0;
		virtual void	_onResizeRequested( WgVectorHook * pHook ) = 0;
		virtual void	_onRenderRequested( WgVectorHook * pHook ) = 0;
		virtual void	_onRenderRequested( WgVectorHook * pHook, const WgRect& rect ) = 0;
		virtual void	_onWidgetAppeared( WgVectorHook * pInserted ) = 0;				// so parent can update geometry and possibly request render.
		virtual void	_onWidgetDisappeared( WgVectorHook * pToBeRemoved ) = 0;		// so parent can update geometry and possibly request render.
		virtual void	_onWidgetsReordered() = 0;
		virtual void	_refreshAllWidgets() = 0;
		virtual WgVectorHook * _newHook() = 0;
	
		//
	
		WgChain<WgVectorHook>	m_hooks;
	
		int					m_nChildPanels;					// Number of VISIBLE children that are panels.
	};
	
	
	

} // namespace wg
#endif //WG_VECTORPANEL_DOT_H
