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

#ifndef WG_RECTCHAIN_DOT_H
#	include <wg_rectchain.h>
#endif

class WgInputDevice;
class WgGfxDevice;
class WgGizmo;

class WgRoot : private WgGizmoParent
{
public:
	WgRoot();
	WgRoot( WgGfxDevice * pGfxDevice, WgInputDevice * pInputDevice );
	~WgRoot();

	bool					SetGfxDevice( WgGfxDevice * pDevice );
	inline WgGfxDevice * 	GfxDevice() const { return m_pGfxDevice; };

	bool 					SetInputDevice( WgInputDevice * pDevice );
	inline WgInputDevice *	InputDevice() const { return m_pInputDevice; }

	bool					SetGeo( const WgRect& geo );
	WgRect					Geo() const;

	inline WgGizmo *		Gizmo() const { return m_hook.Gizmo(); }
	bool					SetGizmo( WgGizmoContainer * pGizmo );
	inline void				DeleteGizmo() { SetGizmo(0); }
	WgGizmo * 				ReleaseGizmo();

	// Inherited from WgGizmoParent

	bool					DeleteGizmo( WgGizmo * pGizmo );
	WgGizmo *				ReleaseGizmo( WgGizmo * pGizmo );

	bool					DeleteAllGizmos();
	bool					ReleaseAllGizmos();

	bool					IsGizmo() const { return false; }
	bool					IsRoot() const { return true; }

	WgGizmo *				CastToGizmo() { return 0; }
	WgRoot *				CastToRoot() { return this; }


//	inline int	NbDirtyRects() const { return m_dirtyRects....  TODO: Implement when WgRectChain has a Size() method
	int		ExportDirtyRects( WgRect * pDest, int maxRects ) const;

	bool	Render();
	bool	Render( const WgRect& clip );

	bool	BeginRender( const WgRect& clip );
	bool	RenderSection( int layer = 0xFF );
	bool	EndRender();

	void	AddDirtyRect( WgRect rect );

	WgGizmo *	FindGizmo( const WgCoord& ofs, WgSearchMode mode );

protected:
	class Hook : public WgGizmoHook
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

		WgRoot*			Root() const;

		WgWidget*		GetRoot();			// Should in the future not return a widget, but a gizmo.

	protected:

		void			RequestRender();
		void			RequestRender( const WgRect& rect );
		void			RequestResize();

		WgGizmoHook *	_prevHook() const;
		WgGizmoHook *	_nextHook() const;
		WgGizmoContainer * _parent() const;			// Always returns 0.

		WgRoot *		m_pRoot;
	};


	WgGizmoHook*	_firstHook() const { return m_hook.Gizmo()? const_cast<Hook*>(&m_hook):0; }
	WgGizmoHook*	_lastHook() const { return m_hook.Gizmo()? const_cast<Hook*>(&m_hook):0; }

	bool 			_focusRequested( WgGizmoHook * pBranch, WgGizmo * pGizmoRequesting );
	bool 			_focusReleased( WgGizmoHook * pBranch, WgGizmo * pGizmoReleasing );

	WgRectChain			m_dirtyRects;

	WgGfxDevice *		m_pGfxDevice;
	WgInputDevice *		m_pInputDevice;
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
};



#endif //WG_ROOT_DOT_H
