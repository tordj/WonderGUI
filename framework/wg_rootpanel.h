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

#include <deque>

#ifndef WG_CONTAINER_DOT_H
#	include <wg_container.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_PATCHES_DOT_H
#	include <wg_patches.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

class WgWidget;

class WgRootPanel : public WgWidgetHolder
{
public:
	WgRootPanel();
    WgRootPanel( wg::GfxDevice * pGfxDevice );
	~WgRootPanel();

    bool					SetGfxDevice( wg::GfxDevice * pDevice );
    inline wg::GfxDevice_p 	GfxDevice() const { return m_pGfxDevice; };

	inline WgEventHandler *	EventHandler() const { return m_pEventHandler; }

	bool					SetPixelGeo( const WgRect& geo );
	WgRect					PixelGeo() const;

	bool					SetVisible( bool bVisible );
	bool					IsVisible() const { return m_bVisible; }

	inline WgWidget *		Child() const { return m_hook.Widget(); }
	bool					SetChild( WgWidget * pWidget );
	inline void				DeleteChild() { SetChild(0); }
	WgWidget * 				ReleaseChild();

	void					SetScale( int scale );
	int						Scale() const { return m_scale; }

	// Inherited from WgWidgetHolder

	bool					DeleteChild( WgWidget * pWidget );
	WgWidget *				ReleaseChild( WgWidget * pWidget );

	bool					DeleteAllChildren();
	bool					ReleaseAllChildren();

	bool					IsRoot() const { return true; }

	WgRootPanel *			CastToRoot() { return this; }
	const WgRootPanel *		CastToRoot() const { return this; }


	inline int				NbDirtyRects() const { return m_dirtyPatches.Size(); }
	inline const WgRect*	FirstDirtyRect() const { return m_dirtyPatches.IsEmpty() ? 0 : m_dirtyPatches.Begin(); }

	inline int				NbUpdatedRects() const { return m_updatedPatches.Size(); }
	inline const WgRect*	FirstUpdatedRect() const { return m_updatedPatches.IsEmpty() ? 0 : m_updatedPatches.Begin(); }


	bool	Render();
	bool	Render( const WgRect& clip );

	bool	BeginRender();
	bool	RenderSection( const WgRect& clip );
	bool	EndRender();

	void	SetUpdatedRectOverlay( const WgSkinPtr& pUpdatedRectOverlay, int afterglowFrames = 3 );

	inline void	AddDirtyPatch( const WgRect& rect ) { m_dirtyPatches.Add( rect ); }

	WgWidget *	FindWidget( const WgCoord& ofs, WgSearchMode mode );

protected:
	class Hook : public WgHook
	{
		friend class WgRootPanel;
	public:
		Hook() : m_pRoot(0) {};				// So we can make them members and then make placement new...
		~Hook();

		const char *Type( void ) const;
		static const char * ClassType();

		WgCoord			PixelPos() const;
		WgSize			PixelSize() const;
		WgRect			PixelGeo() const;
		WgCoord			ScreenPixelPos() const;
		WgRect			ScreenPixelGeo() const;

		WgRootPanel*	Root() const;

	protected:

		void			_requestRender();
		void			_requestRender( const WgRect& rect );
		void			_requestResize();

		WgHook *		_prevHook() const;
		WgHook *		_nextHook() const;
		WgWidgetHolder * _holder() const;
		WgContainer *	_parent() const;

		WgRootPanel *		m_pRoot;
	};



	WgHook*				_firstHook() const { return m_hook.Widget()? const_cast<Hook*>(&m_hook):0; }
	WgHook*				_lastHook() const { return m_hook.Widget()? const_cast<Hook*>(&m_hook):0; }

	bool 				_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting );
	bool 				_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing );

    void                _inViewRequested( WgHook * pChild );
    void                _inViewRequested( WgHook * pChild, const WgRect& mustHaveArea, const WgRect& niceToHaveArea );

    
	WgPatches			m_dirtyPatches;		// Dirty patches that needs to be rendered.
	WgPatches			m_updatedPatches;	// Patches that were updated in last rendering session.

	WgSkinPtr			m_pUpdatedRectOverlay;
	int					m_afterglowFrames;

	std::deque<WgPatches>	m_afterglowRects;	// Afterglow rects are placed in this queue.
	

    wg::GfxDevice *		m_pGfxDevice;
	WgEventHandler *	m_pEventHandler;
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
	bool				m_bVisible;
	int					m_scale;
};



#endif //WG_ROOTPANEL_DOT_H