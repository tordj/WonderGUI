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
#ifndef WG_ROOTPANEL_DOT_H
#define WG_ROOTPANEL_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_PATCHES_DOT_H
#	include <wg_patches.h>
#endif

#ifndef WG_MSGROUTER_DOT_H
#	include <wg_msgrouter.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

namespace wg 
{
	
	class Widget;
	
	
	class RootPanel;
	typedef	WgStrongPtr<RootPanel,Object_p>		RootPanel_p;
	typedef	WgWeakPtr<RootPanel,Object_wp>	RootPanel_wp;
	
	
	//____ RootPanel ____________________________________________________________
	
	/**
	 * @brief	Special widget, being the root of all widgets on screen.
	 *
	 * Special widget, being the root of all widgets on screen.
	 *
	 *
	 */
	
	
	class RootPanel : public Object
	{
		friend class Container;
		friend class InputHandler;
	
	public:
		static RootPanel_p	create() { return RootPanel_p(new RootPanel()); }
		static RootPanel_p	create( const GfxDevice_p& pDevice ) { return RootPanel_p(new RootPanel(pDevice)); }
	
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static RootPanel_p	cast( const Object_p& pObject );
	
		bool					setGfxDevice( const GfxDevice_p& pDevice );
		inline GfxDevice_p 	gfxDevice() const { return m_pGfxDevice; }
	
		bool					setGeo( const Rect& geo );
		Rect					geo() const;
	
		bool					setVisible( bool bVisible );
		bool					isVisible() const { return m_bVisible; }
	
		Hook_p					setWidget( const Widget_p& pWidget );
		inline Widget_p			widget() const { return m_hook._widget(); }
		bool					removeWidget();
		bool					clear();
		
		inline Widget_p			focusedChild() const { return _focusedChild(); }
		
		
	
		Widget_p				findWidget( const Coord& ofs, WgSearchMode mode ) { return Widget_p(_findWidget(ofs-m_geo.pos(),mode)); }
	
		inline int				nbDirtyRects() const { return m_dirtyPatches.size(); }
		inline const Rect*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? 0 : m_dirtyPatches.begin(); }
	
		inline int				nbUpdatedRects() const { return m_updatedPatches.size(); }
		inline const Rect*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? 0 : m_updatedPatches.begin(); }
	
	
		bool	render();
		bool	render( const Rect& clip );
	
		bool	beginRender();
		bool	renderSection( const Rect& clip );
		bool	endRender();
	
		inline void	addDirtyPatch( const Rect& rect ) { m_dirtyPatches.add( rect ); }
	
	
	protected:
		RootPanel();
		RootPanel( const GfxDevice_p& pGfxDevice );
		~RootPanel();
	
		class MyHook : public Hook
		{
			friend class RootPanel;
		public:
			MyHook() : m_pRoot(0) {};				// So we can make them members and then make placement new...
			~MyHook();
	
			const char *type( void ) const;
			static const char * classType();
	
			Coord			pos() const;
			Size			size() const;
			Rect			geo() const;
			Coord			globalPos() const;
			Rect			globalGeo() const;
	
	
		protected:
	
			void			_requestRender();
			void			_requestRender( const Rect& rect );
			void			_requestResize();

			bool			_requestFocus( Widget * pWidget );
			bool			_releaseFocus( Widget * pWidget );

	
			Hook *		_prevHook() const;
			Hook *		_nextHook() const;
			Container *	_parent() const;
			RootPanel *	_root() const;
	
			RootPanel *	m_pRoot;
		};
	
		Widget *			_findWidget( const Coord& ofs, WgSearchMode mode );
	
		Hook*				_firstHook() const { return m_hook._widget()? const_cast<MyHook*>(&m_hook):0; }
		Hook*				_lastHook() const { return m_hook._widget()? const_cast<MyHook*>(&m_hook):0; }
	
		bool 				_focusRequested( Widget * pWidgetRequesting );
		bool 				_focusReleased( Widget * pWidgetReleasing );

//		void				_setFocusedChild( Widget * pWidget );
		Widget *			_focusedChild() const;

	
		Patches			m_dirtyPatches;		// Dirty patches that needs to be rendered.
		Patches			m_updatedPatches;	// Patches that were updated in last rendering session.
	
		GfxDevice_p		m_pGfxDevice;
		MyHook				m_hook;
		Rect				m_geo;
		bool				m_bHasGeo;
		bool				m_bVisible;
		
		Widget_wp			m_pFocusedChild;
	};
	
	
	

} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
