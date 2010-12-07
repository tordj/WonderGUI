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

#ifndef WG_GIZMO_HOOK_DOT_H
#	include <wg_gizmo_hook.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif


class WgInputDevice;
class WgGfxDevice;
class WgGizmo;

class WgRoot
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
	bool					SetGizmo( WgGizmo * pGizmo );
	inline void				DeleteGizmo() { SetGizmo(0); }
	WgGizmo * 				ReleaseGizmo();


	void	Update( int msTime );

//	inline int	NbDirtyRects() const { return m_dirtyRects....  TODO: Implement when WgDirtyRectObj has a Size() method
	int		ExportDirtyRects( WgRect * pDest, int maxRects ) const;

	int		Render( WgRect * pDirtyRects, int nRects, int maxRects );
	int		Render( WgRect * pDirtyRects, int nRects, int maxRects, const WgRect& clip );

	int		BeginRender( WgRect * pDirtyRects, int nRects, int maxRects );
	int		BeginRender( WgRect * pDirtyRects, int nRects, int maxRects, const WgRect& clip );
	void	RenderLayer( int layer );
	void	EndRender( void );

	void	AddDirtyRect( WgRect rect );

protected:

	class Hook : public WgGizmoHook
	{
		friend class WgRoot;
	public:
		Hook() : WgGizmoHook( 0 ), m_pRoot(0) {};				// So we can make them members and then make placement new...
		Hook( WgGizmo * pGizmo, WgRoot * pRoot);
		~Hook();

		WgCord			Pos() const;
		WgSize			Size() const;
		WgRect			Geo() const;
		WgCord			ScreenPos() const;
		WgRect			ScreenGeo() const;

		WgGizmoHook *	PrevHook() const;
		WgGizmoHook *	NextHook() const;

		WgGizmoContainer* Parent() const;
		WgRoot*			Root() const;

		WgWidget*		GetRoot();			// Should in the future not return a widget, but a gizmo.

	protected:

		void			RequestRender();
		void			RequestRender( const WgRect& rect );
		void			RequestResize();

		bool			RequestFocus();
		bool			ReleaseFocus();

		WgRoot *		m_pRoot;
	};

	WgDirtyRectObj		m_dirtyRects;

	WgGfxDevice *		m_pGfxDevice;
	WgInputDevice *		m_pInputDevice;
	Hook				m_hook;
	WgRect				m_geo;
	bool				m_bHasGeo;
};



#endif //WG_ROOT_DOT_H
