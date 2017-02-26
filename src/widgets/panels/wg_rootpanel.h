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

#ifndef WG_WIDGETHOLDER_DOT_H
#	include <wg_widgetholder.h>
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

#ifndef WG_CHILDENTRY_DOT_H
#	include <wg_childentry.h>
#endif

namespace wg 
{
	
	class Widget;
	
	
	class RootPanel;
	typedef	StrongPtr<RootPanel,Object_p>	RootPanel_p;
	typedef	WeakPtr<RootPanel,Object_wp>	RootPanel_wp;
	
	
	//____ RootPanel ____________________________________________________________
	
	/**
	 * @brief	Special widget, being the root of all widgets on screen.
	 *
	 * Special widget, being the root of all widgets on screen.
	 *
	 *
	 */
	
	
	class RootPanel : public Object, protected WidgetHolder, protected SlotCanHolder
	{
		friend class Container;
		friend class InputHandler;
	
	public:
		static RootPanel_p	create() { return RootPanel_p(new RootPanel()); }
		static RootPanel_p	create( const GfxDevice_p& pDevice ) { return RootPanel_p(new RootPanel(pDevice)); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static RootPanel_p	cast( const Object_p& pObject );
	
		//____ Interfaces ___________________________________________________________

		ChildEntry<Slot>	child;

		bool				setGfxDevice( const GfxDevice_p& pDevice );
		inline GfxDevice_p 	gfxDevice() const { return m_pGfxDevice; }
	
		bool				setGeo( const Rect& geo );
		Rect				geo() const;
	
	
		void				setDebugMode( bool onOff );
		bool				isDebugMode() const { return m_bDebugMode; }
		void 				setDebugOverlay(const Skin_p& pOverlaySkin );
		void				setDebugAfterglow(int frames);
		Skin_p				debugOverlay() const { return m_pDebugOverlay;  }
		int					debugAfterglow() const { return m_afterglowFrames;  }
	
		bool				setVisible( bool bVisible );							//TODO: Implement!!!
		bool				isVisible() const { return m_bVisible; }
	
		
		inline Widget_p		focusedChild() const { return _focusedChild(); }
	
	
		Widget_p			findWidget( const Coord& ofs, SearchMode mode ) { return Widget_p(_findWidget(ofs-m_geo.pos(),mode)); }
	
		inline int			nbDirtyRects() const { return m_dirtyPatches.size(); }
		inline const Rect*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? 0 : m_dirtyPatches.begin(); }
	
		inline int			nbUpdatedRects() const { return m_updatedPatches.size(); }
		inline const Rect*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? 0 : m_updatedPatches.begin(); }
	
	
		bool		render();
		bool		render( const Rect& clip );
	
		bool		beginRender();
		bool		renderSection( const Rect& clip );
		bool		endRender();
	
		inline void	addDirtyPatch( const Rect& rect ) { m_dirtyPatches.add( rect ); }
	
	
	protected:
		RootPanel();
		RootPanel( const GfxDevice_p& pGfxDevice );
		~RootPanel();
	

		// WidgetHolder methods

		Coord			_childPos( void * pChildRef ) const;
		Coord			_childGlobalPos( void * pChildRef ) const;

		Size			_childSize( void * pChildRef ) const;

		bool			_isChildVisible( void * pChildRef ) const;
		Rect			_childWindowSection( void * pChildRef ) const;

		Container *  	_childParent();
		RootPanel *		_root();

		void			_childRequestRender( void * pChildRef );
		void			_childRequestRender( void * pChildRef, const Rect& rect );
		void			_childRequestResize( void * pChildRef );

		bool			_childRequestFocus( void * pChildRef, Widget * pWidget );
		bool			_childReleaseFocus( void * pChildRef, Widget * pWidget );

		void			_childRequestInView( void * pChildRef );
		void			_childRequestInView( void * pChildRef, const Rect& mustHaveArea, const Rect& niceToHaveArea );

		Widget *		_prevChild( void * pChildRef ) const;
		Widget *		_nextChild( void * pChildRef ) const;
	

		// SlotCanHolder methods

		Object *		_object();
		const Object *	_object() const;

		void			_setWidget( Slot * pSlot, Widget * pNewWidget );

		//

		Widget *			_findWidget( const Coord& ofs, SearchMode mode );

//		void				_setFocusedChild( Widget * pWidget );
		Widget *			_focusedChild() const;

	
		Patches				m_dirtyPatches;		// Dirty patches that needs to be rendered.
		Patches				m_updatedPatches;	// Patches that were updated in last rendering session.


		bool				m_bDebugMode;
		Skin_p				m_pDebugOverlay;
		int					m_afterglowFrames;
		std::deque<Patches>	m_afterglowRects;	// Afterglow rects are placed in this queue.
	
		GfxDevice_p			m_pGfxDevice;
		SlotCan<Slot>		m_child;
		Rect				m_geo;
		bool				m_bHasGeo;
		bool				m_bVisible;
		
		Widget_wp			m_pFocusedChild;
	};
	
	
	

} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
