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

class WgWidget;


class WgRootPanel;
typedef	WgStrongPtr<WgRootPanel,WgObjectPtr>		WgRootPanelPtr;
typedef	WgWeakPtr<WgRootPanel,WgObjectWeakPtr>	WgRootPanelWeakPtr;


//____ WgRootPanel ____________________________________________________________

/**
 * @brief	Special widget, being the root of all widgets on screen.
 *
 * Special widget, being the root of all widgets on screen.
 *
 *
 */


class WgRootPanel : public WgObject
{
	friend class WgContainer;

public:
	static WgRootPanelPtr	create() { return WgRootPanelPtr(new WgRootPanel()); }
	static WgRootPanelPtr	create( const WgGfxDevicePtr& pDevice ) { return WgRootPanelPtr(new WgRootPanel(pDevice)); }

	bool					isInstanceOf( const char * pClassName ) const;
	const char *			className( void ) const;
	static const char		CLASSNAME[];
	static WgRootPanelPtr	cast( const WgObjectPtr& pObject );

	bool					setGfxDevice( const WgGfxDevicePtr& pDevice );
	inline WgGfxDevicePtr 	gfxDevice() const { return m_pGfxDevice; }

	bool					setGeo( const WgRect& geo );
	WgRect					geo() const;

	bool					setVisible( bool bVisible );
	bool					isVisible() const { return m_bVisible; }

	inline WgWidgetPtr		widget() const { return m_hook._widget(); }
	WgHookPtr				setWidget( const WgWidgetPtr& pWidget );
	bool					removeWidget();
	bool					clear();

	WgWidgetPtr				findWidget( const WgCoord& ofs, WgSearchMode mode ) { return WgWidgetPtr(_findWidget(ofs-m_geo.pos(),mode)); }

	inline int				nbDirtyRects() const { return m_dirtyPatches.size(); }
	inline const WgRect*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? 0 : m_dirtyPatches.begin(); }

	inline int				nbUpdatedRects() const { return m_updatedPatches.size(); }
	inline const WgRect*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? 0 : m_updatedPatches.begin(); }


	bool	render();
	bool	render( const WgRect& clip );

	bool	beginRender();
	bool	renderSection( const WgRect& clip );
	bool	endRender();

	inline void	addDirtyPatch( const WgRect& rect ) { m_dirtyPatches.add( rect ); }


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

		const char *type( void ) const;
		static const char * classType();

		WgCoord			pos() const;
		WgSize			size() const;
		WgRect			geo() const;
		WgCoord			globalPos() const;
		WgRect			globalGeo() const;


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
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
	bool				m_bVisible;
};



#endif //WG_ROOTPANEL_DOT_H
