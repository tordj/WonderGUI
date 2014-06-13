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

#ifndef WG_EVENTHANDLER_DOT_H
#	include <wg_eventhandler.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgWidget;


class WgRootPanel;
typedef	WgSmartPtr<WgRootPanel,WgObjectPtr>		WgRootPanelPtr;
typedef	WgWeakPtr<WgRootPanel,WgObjectWeakPtr>	WgRootPanelWeakPtr;


//____ WgRootPanel ____________________________________________________________

/**
 * @brief	Special widget, being the root of all widgets on screen.
 * 
 */


class WgRootPanel : public WgObject
{
	friend class WgContainer;

public:
	static WgRootPanelPtr	Create() { return WgRootPanelPtr(new WgRootPanel()); }
	static WgRootPanelPtr	Create( const WgGfxDevicePtr& pDevice ) { return WgRootPanelPtr(new WgRootPanel(pDevice)); }

	bool					IsInstanceOf( const char * pClassName ) const;
	const char *			ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgRootPanelPtr	Cast( const WgObjectPtr& pObject );

	bool					SetGfxDevice( const WgGfxDevicePtr& pDevice );
	inline WgGfxDevicePtr 	GfxDevice() const { return m_pGfxDevice; }

	inline WgEventHandlerPtr EventHandler() const { return m_pEventHandler; }

	bool					SetGeo( const WgRect& geo );
	WgRect					Geo() const;

	bool					SetVisible( bool bVisible );
	bool					IsVisible() const { return m_bVisible; }

	inline WgWidgetPtr		Widget() const { return m_hook._widget(); }
	WgHookPtr				SetWidget( const WgWidgetPtr& pWidget );
	bool					RemoveWidget();
	bool					Clear();

	WgWidgetPtr				FindWidget( const WgCoord& ofs, WgSearchMode mode ) { return WgWidgetPtr(_findWidget(ofs,mode)); }

	inline int				NbDirtyRects() const { return m_dirtyPatches.Size(); }
	inline const WgRect*	FirstDirtyRect() const { return m_dirtyPatches.IsEmpty() ? 0 : m_dirtyPatches.Begin(); }

	inline int				NbUpdatedRects() const { return m_updatedPatches.Size(); }
	inline const WgRect*	FirstUpdatedRect() const { return m_updatedPatches.IsEmpty() ? 0 : m_updatedPatches.Begin(); }


	bool	Render();
	bool	Render( const WgRect& clip );

	bool	BeginRender();
	bool	RenderSection( const WgRect& clip );
	bool	EndRender();

	inline void	AddDirtyPatch( const WgRect& rect ) { m_dirtyPatches.Add( rect ); }


protected:
	WgRootPanel();
	WgRootPanel( const WgGfxDevicePtr& pGfxDevice );
	~WgRootPanel();

	class Hook : public WgHook
	{
		friend class WgRootPanel;
	public:
		Hook() : m_pRoot(0) {};				// So we can make them members and then make placement new...
		~Hook();

		const char *Type( void ) const;
		static const char * ClassType();

		WgCoord			Pos() const;
		WgSize			Size() const;
		WgRect			Geo() const;
		WgCoord			GlobalPos() const;
		WgRect			GlobalGeo() const;


	protected:

		void			_requestRender();
		void			_requestRender( const WgRect& rect );
		void			_requestResize();

		WgHook *		_prevHook() const;
		WgHook *		_nextHook() const;
		WgContainer *	_parent() const;
		WgRootPanel *	_root() const;

		WgRootPanel *	m_pRoot;
	};

	WgWidget *			_findWidget( const WgCoord& ofs, WgSearchMode mode );

	WgHook*				_firstHook() const { return m_hook._widget()? const_cast<Hook*>(&m_hook):0; }
	WgHook*				_lastHook() const { return m_hook._widget()? const_cast<Hook*>(&m_hook):0; }

	bool 				_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting );
	bool 				_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing );

	WgPatches			m_dirtyPatches;		// Dirty patches that needs to be rendered.
	WgPatches			m_updatedPatches;	// Patches that were updated in last rendering session.

	WgGfxDevicePtr		m_pGfxDevice;
	WgEventHandlerPtr	m_pEventHandler;
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
	bool				m_bVisible;
};



#endif //WG_ROOTPANEL_DOT_H
