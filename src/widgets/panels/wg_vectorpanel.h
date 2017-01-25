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
	
	class VectorPanel;
	typedef	StrongPtr<VectorPanel,Panel_p>		VectorPanel_p;
	typedef	WeakPtr<VectorPanel,Panel_wp>	VectorPanel_wp;
	
	class VectorHook;
	typedef	HookTypePtr<VectorHook,PanelHook_p>	VectorHook_p;
	
	//____ VectorHook ___________________________________________________________
	
	class VectorHook : public PanelHook, protected Link
	{
		friend class VectorPanel;
		friend class Chain<VectorHook>;
	
	
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static VectorHook_p	cast( const Hook_p& pInterface );
	
		Coord	pos() const;
		Size	size() const;
		Rect	geo() const;
		Coord	globalPos() const;
		Rect	globalGeo() const;
	
		VectorHook_p	prev() const { return _prev(); }
		VectorHook_p	next() const { return _next(); }
		VectorPanel_p parent() const;
	
		bool			moveForward();
		bool			moveBackward();
		bool			moveBefore( const VectorHook_p& pSibling );
		bool			moveAfter( const VectorHook_p& pSibling );
		bool			moveFirst();
		bool			moveLast();
	
		bool			setVisible( bool bVisible );
	
	protected:
		PROTECTED_LINK_METHODS( VectorHook );
	
		VectorHook();
		~VectorHook();
	
		void	_requestRender();
		void	_requestRender( const Rect& rect );
		void	_requestResize();
	
		Hook *	_prevHook() const;
		Hook *	_nextHook() const;
	};
	
	//____ VectorPanel __________________________________________________________
	
	class VectorPanel : public Panel
	{
		friend class VectorHook;
	public:
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static VectorPanel_p	cast( const Object_p& pObject );
	
		VectorHook_p addWidget( const Widget_p& pWidget ) { return _addWidget(pWidget.rawPtr() ); }
		VectorHook_p insertWidget( const Widget_p& pWidget, const Widget_p& pSibling ) { return _insertWidget( pWidget.rawPtr(), pSibling.rawPtr() ); }
	
		bool			removeWidget( const Widget_p& pWidget );
		bool			clear();
	
	protected:
		VectorPanel();
		virtual ~VectorPanel();
	
		VectorHook *	_addWidget( Widget * pWidget );
		VectorHook *	_insertWidget( Widget * pWidget, Widget * pSibling );
	
	
		void			_cloneContent( const Widget * _pOrg );
	
		Hook*			_firstHook() const { return m_hooks.first(); }
		Hook*			_lastHook() const { return m_hooks.last(); }
	
		//

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;



		// To be overloaded by subclasses
	
		virtual Rect	_hookGeo( const VectorHook * pHook ) = 0;
		virtual void	_requestResizeRequested( VectorHook * pHook ) = 0;
		virtual void	_renderRequested( VectorHook * pHook ) = 0;
		virtual void	_renderRequested( VectorHook * pHook, const Rect& rect ) = 0;
		virtual void	_onWidgetAppeared( VectorHook * pInserted ) = 0;				// so parent can update geometry and possibly request render.
		virtual void	_onWidgetDisappeared( VectorHook * pToBeRemoved ) = 0;		// so parent can update geometry and possibly request render.
		virtual void	_onWidgetsReordered() = 0;
		virtual void	_refreshAllWidgets() = 0;
		virtual VectorHook * _newHook() = 0;
	
		//
	
		Chain<VectorHook>	m_hooks;
	
		int					m_nChildPanels;					// Number of VISIBLE children that are panels.
	};
	
	
	

} // namespace wg
#endif //WG_VECTORPANEL_DOT_H
