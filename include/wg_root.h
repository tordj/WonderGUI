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
#ifndef WG_ROOT_DOT_H
#define WG_ROOT_DOT_H

#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_PATCHES_DOT_H
#	include <wg_patches.h>
#endif

class WgGfxDevice;
class WgGizmo;

class WgRoot : private WgGizmoParent
{
public:
	WgRoot();
	WgRoot( WgGfxDevice * pGfxDevice );
	~WgRoot();

	bool					SetGfxDevice( WgGfxDevice * pDevice );
	inline WgGfxDevice * 	GfxDevice() const { return m_pGfxDevice; };

	inline WgEventHandler *	EventHandler() const { return m_pEventHandler; }

	bool					SetGeo( const WgRect& geo );
	WgRect					Geo() const;

	inline WgGizmo *		Child() const { return m_hook.Gizmo(); }
	bool					SetChild( WgGizmo * pGizmo );
	inline void				DeleteChild() { SetChild(0); }
	WgGizmo * 				ReleaseChild();

	// Inherited from WgGizmoParent

	bool					DeleteChild( WgGizmo * pGizmo );
	WgGizmo *				ReleaseChild( WgGizmo * pGizmo );

	bool					DeleteAllChildren();
	bool					ReleaseAllChildren();

	bool					IsGizmo() const { return false; }
	bool					IsContainer() const { return false; }
	bool					IsRoot() const { return true; }

	WgGizmo *				CastToGizmo() { return 0; }
	const WgGizmo *			CastToGizmo() const { return 0; }
	WgGizmoContainer *		CastToContainer() { return 0; }
	const WgGizmoContainer *CastToContainer() const { return 0; }
	WgRoot *				CastToRoot() { return this; }
	const WgRoot *			CastToRoot() const { return this; }


	inline int				NbDirtyRects() const { return m_dirtyPatches.Size(); }
	inline const WgRect*	FirstDirtyRect() const { return m_dirtyPatches.IsEmpty() ? 0 : m_dirtyPatches.Begin(); }

	bool	Render();
	bool	Render( const WgRect& clip );

	bool	BeginRender();
	bool	RenderSection( const WgRect& clip, int layer = 0xFF );
	bool	EndRender();

	inline void	AddDirtyPatch( const WgRect& rect ) { m_dirtyPatches.Add( rect ); }

	WgGizmo *	FindGizmo( const WgCoord& ofs, WgSearchMode mode );

protected:
	class Hook : public WgHook
	{
		friend class WgRoot;
	public:
		Hook() : m_pRoot(0) {};				// So we can make them members and then make placement new...
		~Hook();

		WgCoord			Pos() const;
		WgSize			Size() const;
		WgRect			Geo() const;
		WgCoord			ScreenPos() const;
		WgRect			ScreenGeo() const;

		bool			SetHidden( bool bHide );
		WgRoot*			Root() const;

	protected:

		void			_requestRender();
		void			_requestRender( const WgRect& rect );
		void			_requestResize();

		WgHook *		_prevHook() const;
		WgHook *		_nextHook() const;
		WgGizmoParent * _parent() const;

		WgRoot *		m_pRoot;
	};


	WgHook*				_firstHook() const { return m_hook.Gizmo()? const_cast<Hook*>(&m_hook):0; }
	WgHook*				_lastHook() const { return m_hook.Gizmo()? const_cast<Hook*>(&m_hook):0; }

	bool 				_focusRequested( WgHook * pBranch, WgGizmo * pGizmoRequesting );
	bool 				_focusReleased( WgHook * pBranch, WgGizmo * pGizmoReleasing );

	WgGizmoModalLayer *		_getModalLayer() const { return 0; }
	WgGizmoMenuLayer*	_getMenuLayer() const { return 0; }

	WgPatches			m_dirtyPatches;

	WgGfxDevice *		m_pGfxDevice;
	WgEventHandler *	m_pEventHandler;
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
};



#endif //WG_ROOT_DOT_H
